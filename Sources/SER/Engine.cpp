// Copyright (C) 2020 Luca Gasperini <luca.gasperini@xsoftware.it>
//
// This file is part of Serious Engine Reloaded.
//
// Serious Engine Reloaded is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Serious Engine Reloaded is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Serious Engine Reloaded.  If not, see <http://www.gnu.org/licenses/>.

#include "Engine.h"
#include <Engine/Templates/Stock_CAnimData.h>
#include <Engine/Templates/Stock_CAnimSet.h>
#include <Engine/Templates/Stock_CEntityClass.h>
#include <Engine/Templates/Stock_CMesh.h>
#include <Engine/Templates/Stock_CModelData.h>
#include <Engine/Templates/Stock_CShader.h>
#include <Engine/Templates/Stock_CSkeleton.h>
#include <Engine/Templates/Stock_CSoundData.h>
#include <Engine/Templates/Stock_CTextureData.h>

#include <Engine/Base/CRC.h>
#include <Engine/Base/CRCTable.h>

#include "Version.h"

using namespace SER;

// global string with current MOD path
CTFileName Engine::mod_path;
// global string with current name (the parameter that is passed on cmdline)
CTString Engine::mod_name;
// global string with url to be shown to users that don't have the mod installed
// (should be set by game.dll)
CTString Engine::mod_url;
// global string with current MOD extension (for adding to dlls)
CTString Engine::mod_ext;
// global string with CD path (for minimal installations)
CTFileName Engine::cd_path;

// OS info
CTString Engine::sys_OS = "";
INDEX Engine::sys_OSMajor = 0;
INDEX Engine::sys_OSMinor = 0;
INDEX Engine::sys_OSBuild = 0;
CTString Engine::sys_OSMisc = "";

// CPU info
CTString Engine::sys_CPUVendor = "";
INDEX Engine::sys_CPUType = 0;
INDEX Engine::sys_CPUFamily = 0;
INDEX Engine::sys_CPUModel = 0;
INDEX Engine::sys_CPUStepping = 0;
BOOL Engine::sys_CPUHasMMX = 0;
BOOL Engine::sys_CPUHasCMOV = 0;
INDEX Engine::sys_CPUMHz = 0;
INDEX Engine::sys_CPUMisc = 0;

// RAM info
INDEX Engine::sys_RAMPhys = 0;
INDEX Engine::sys_RAMSwap = 0;

// HDD info
INDEX Engine::sys_HDDSize = 0;
INDEX Engine::sys_HDDFree = 0;
INDEX Engine::sys_HDDMisc = 0;

// MOD info
CTString Engine::sys_ModName = "";
CTString Engine::sys_ModExt = "";

FileSystem* Engine::file_system = NULL;

CTCriticalSection zip_csLock; // critical section for access to zlib functions

//TODO: Why not on CTString???
// reverses string
CTString StrRev(const CTString& str)
{
    UINT length = str.Length();

    char* tmp_str = new char[length];
    strcpy(tmp_str, str.str_String);

    char ctmp;
    char* pch0 = tmp_str;
    char* pch1 = tmp_str + length - 1;
    while (pch1 > pch0) {
        ctmp = *pch0;
        *pch0 = *pch1;
        *pch1 = ctmp;
        pch0++;
        pch1--;
    }

    return tmp_str;
}

//TODO: Do this on FileSystem constructor when StrRev will be into CTString or something
void Engine::AnalyzeApplicationPath()
{
    CTString strTmpPath = file_system->getAppExe();

    StrRev(strTmpPath);

    int result = strTmpPath.FindSubstr(FileSystem::getDirSeparator());
    if (result < 0)
        strTmpPath = FileSystem::getDirSeparator();
    else
        strTmpPath.TrimLeft(result);

    FileSystem::setAppPath(strTmpPath);
}

// startup engine
void Engine::init(const char* argv0, CTString strGameID)
{ /*
    SanityCheckTypes();

    PlatformSpecificInit();
*/
    const char* gamename = "UnknownGame";
    if (strGameID != "")
        gamename = (const char*)strGameID;

    file_system = new FileSystem(argv0, gamename); // rcg10082001
    /*
#pragma message(">> Remove this from SE_InitEngine : _bWorldEditorApp")
    if (strGameID == "SeriousEditor") {
        _bWorldEditorApp = TRUE;
    }
*/
    AnalyzeApplicationPath();

    try {
        _fnmApplicationExe.RemoveApplicationPath_t();
    } catch (char* strError) {
        (void)strError;
        ASSERT(FALSE);
    }
    /*
    _pConsole = new CConsole;
    if (_strLogFile == "") {
        _strLogFile = CTFileName(CTString(strExePath)).FileName();
        // chop off end of Unix executable filename... --ryan.
        _strLogFile.ReplaceSubstr(CTString("-bin"), CTString(""));
    }
    _pConsole->Initialize(_fnmUserDir + _strLogFile + ".log", 90, 512);
*/
    _pAnimStock = new CStock_CAnimData;
    _pTextureStock = new CStock_CTextureData;
    _pSoundStock = new CStock_CSoundData;
    _pModelStock = new CStock_CModelData;
    _pEntityClassStock = new CStock_CEntityClass;
    _pMeshStock = new CStock_CMesh;
    _pSkeletonStock = new CStock_CSkeleton;
    _pAnimSetStock = new CStock_CAnimSet;
    _pShaderStock = new CStock_CShader;

    // rcg11232001 I moved this here so I can register platform-specific cvars.
    // init main shell
    _pShell = new CShell;
    _pShell->Initialize();

    _pTimer = new CTimer;
    _pGfx = new CGfxLibrary;
    _pSound = new CSoundLibrary;
    _pInput = new CInput;
    _pNetwork = new CNetworkLibrary;

    CRCT_Init();

    _strEngineBuild.PrintF(TRANS("SeriousEngine Build: %d.%d"), _SE_BUILD_MAJOR, _SE_BUILD_MINOR);

    // print basic engine info
    CPrintF(TRANSV("--- Serious Engine Startup ---\n"));
    CPrintF("  %s\n\n", (const char*)_strEngineBuild);

    // print info on the started application
    CPrintF(TRANSV("Executable: %s\n"), FileSystem::getAppExe());
    CPrintF(TRANSV("Assumed engine directory: %s\n"), (const char*)_fnmApplicationPath);

    CPrintF("\n");

    // report os info
    CPrintF(TRANSV("Examining underlying OS...\n"));

    PlatformIdentification();

    CPrintF("\n");

    // (rcg11232001 this is where _pShell was originally created.)

    // report CPU
    CPrintF(TRANSV("Detecting CPU...\n"));
    DetectCPU();
    CPrintF("\n");

    // report memory info
    //extern void ReportGlobalMemoryStatus(void);
    //ReportGlobalMemoryStatus();

    //SetupMemoryManager();
    // initialize zip semaphore
    zip_csLock.cs_iIndex = -1; // not checked for locking order

    // rcg10082001 Honestly, all of this is meaningless in a multitasking OS.
    //  That includes Windows, too.
    //SetupSecondaryStorage(); /// FIXME: does that name make sense
    /*
    // add console variables
    extern INDEX con_bNoWarnings;
    extern INDEX wld_bFastObjectOptimization;
    extern INDEX fil_bPreferZips;
    extern FLOAT mth_fCSGEpsilon;
    _pShell->DeclareSymbol("user INDEX con_bNoWarnings;", (void*)&con_bNoWarnings);
    _pShell->DeclareSymbol("user INDEX wld_bFastObjectOptimization;", (void*)&wld_bFastObjectOptimization);
    _pShell->DeclareSymbol("user FLOAT mth_fCSGEpsilon;", (void*)&mth_fCSGEpsilon);
    _pShell->DeclareSymbol("persistent user INDEX fil_bPreferZips;", (void*)&fil_bPreferZips);
    // OS info
    _pShell->DeclareSymbol("user const CTString sys_strOS    ;", (void*)&sys_strOS);
    _pShell->DeclareSymbol("user const INDEX sys_iOSMajor    ;", (void*)&sys_iOSMajor);
    _pShell->DeclareSymbol("user const INDEX sys_iOSMinor    ;", (void*)&sys_iOSMinor);
    _pShell->DeclareSymbol("user const INDEX sys_iOSBuild    ;", (void*)&sys_iOSBuild);
    _pShell->DeclareSymbol("user const CTString sys_strOSMisc;", (void*)&sys_strOSMisc);
    // CPU info
    _pShell->DeclareSymbol("user const CTString sys_strCPUVendor;", (void*)&sys_strCPUVendor);
    _pShell->DeclareSymbol("user const INDEX sys_iCPUType       ;", (void*)&sys_iCPUType);
    _pShell->DeclareSymbol("user const INDEX sys_iCPUFamily     ;", (void*)&sys_iCPUFamily);
    _pShell->DeclareSymbol("user const INDEX sys_iCPUModel      ;", (void*)&sys_iCPUModel);
    _pShell->DeclareSymbol("user const INDEX sys_iCPUStepping   ;", (void*)&sys_iCPUStepping);
    _pShell->DeclareSymbol("user const INDEX sys_bCPUHasMMX     ;", (void*)&sys_bCPUHasMMX);
    _pShell->DeclareSymbol("user const INDEX sys_bCPUHasCMOV    ;", (void*)&sys_bCPUHasCMOV);
    _pShell->DeclareSymbol("user const INDEX sys_iCPUMHz        ;", (void*)&sys_iCPUMHz);
    _pShell->DeclareSymbol("     const INDEX sys_iCPUMisc       ;", (void*)&sys_iCPUMisc);
    // RAM info
    _pShell->DeclareSymbol("user const INDEX sys_iRAMPhys;", (void*)&sys_iRAMPhys);
    _pShell->DeclareSymbol("user const INDEX sys_iRAMSwap;", (void*)&sys_iRAMSwap);
    _pShell->DeclareSymbol("user const INDEX sys_iHDDSize;", (void*)&sys_iHDDSize);
    _pShell->DeclareSymbol("user const INDEX sys_iHDDFree;", (void*)&sys_iHDDFree);
    _pShell->DeclareSymbol("     const INDEX sys_iHDDMisc;", (void*)&sys_iHDDMisc);
    // MOD info
    _pShell->DeclareSymbol("user const CTString sys_strModName;", (void*)&sys_strModName);
    _pShell->DeclareSymbol("user const CTString sys_strModExt;", (void*)&sys_strModExt);

    // Stock clearing
    extern void FreeUnusedStock(void);
    _pShell->DeclareSymbol("user void FreeUnusedStock(void);", (void*)&FreeUnusedStock);

    // Timer tick quantum
    _pShell->DeclareSymbol("user const FLOAT fTickQuantum;", (FLOAT*)&_pTimer->TickQuantum);
*/
    // init MODs and stuff ...
    InitStreams();
    // keep mod name in sys cvar
    sys_ModName = _strModName;
    sys_ModExt = _strModExt;

// checking of crc
#if 0
  ULONG ulCRCActual = -2;
  SLONG ulCRCExpected = -1;
  try {
    // get the checksum of engine
#ifndef NDEBUG
#define SELFFILE "Bin\\Debug\\EngineD.dll"
#define SELFCRCFILE "Bin\\Debug\\EngineD.crc"
#else
#define SELFFILE "Bin\\Engine.dll"
#define SELFCRCFILE "Bin\\Engine.crc"
#endif
    ulCRCActual = GetFileCRC32_t(CTString(SELFFILE));
    // load expected checksum from the file on disk
    ulCRCExpected = 0;
    LoadIntVar(CTString(SELFCRCFILE), ulCRCExpected);
  } catch (char *strError) {
    CPrintF("%s\n", strError);
  }
  // if not same
  if (ulCRCActual!=ulCRCExpected) {
    // don't run
    //FatalError(TRANS("Engine CRC is invalid.\nExpected %08x, but found %08x.\n"), ulCRCExpected, ulCRCActual);
  }
#endif

    _pInput->Initialize();

    _pGfx->Init();
    _pSound->Init();

    if (strGameID != "") {
        _pNetwork->Init(strGameID);
        // just make classes declare their shell variables
        try {
            CEntityClass* pec = _pEntityClassStock->Obtain_t(CTString("Classes\\Player.ecl"));
            ASSERT(pec != NULL);
            _pEntityClassStock->Release(pec); // this must not be a dependency!
            // if couldn't find player class
        } catch (char* strError) {
            FatalError(TRANS("Cannot initialize classes:\n%s"), strError);
        }
    } else {
        _pNetwork = NULL;
    }

    // mark that default fonts aren't loaded (yet)
    _pfdDisplayFont = NULL;
    _pfdConsoleFont = NULL;

    //InitSystemGammaSettings();
    //InitIFeel(); // on non win32 platforms this will be optimized out if we play our cards right
}

BOOL Engine::FileMatchesList(CDynamicStackArray<CTFileName>& afnm, const CTFileName& fnm)
{
    for (INDEX i = 0; i < afnm.Count(); i++) {
        if (fnm.Matches(afnm[i]) || fnm.HasPrefix(afnm[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

static CTFileName _fnmApp;

void Engine::InitStreams()
{ /*
    // obtain information about system
// !!! FIXME: Move this into an abstraction of some sort...
#ifdef PLATFORM_WIN32
    SYSTEM_INFO siSystemInfo;
    GetSystemInfo(&siSystemInfo);
    // and remember page size
    g_page_size = siSystemInfo.dwPageSize * 16; // cca. 64kB on WinNT/Win95
#else
    g_page_size = PAGESIZE;
#endif

    // keep a copy of path for setting purposes
    _fnmApp = g_app_path;

    // if no mod defined yet
    if (g_app_mod_path == "") {
        // check for 'default mod' file
        LoadStringVar(CTString("DefaultMod.txt"), g_app_mod_path);
    }

    CPrintF(TRANSV("Current mod: %s\n"),
        (g_app_mod_path == "") ? TRANS("<none>") : (const char*)(CTString&)g_app_mod_path);
    // if there is a mod active
    if (g_app_mod_path != "") {
        // load mod's include/exclude lists
        CPrintF(TRANSV("Loading mod include/exclude lists...\n"));
        BOOL bOK = FALSE;
        bOK |= LoadFileList(g_fs_base_write_include, CTString("BaseWriteInclude.lst"));
        bOK |= LoadFileList(g_fs_base_write_exclude, CTString("BaseWriteExclude.lst"));
        bOK |= LoadFileList(g_fs_base_browse_include, CTString("BaseBrowseInclude.lst"));
        bOK |= LoadFileList(g_fs_base_browse_exclude, CTString("BaseBrowseExclude.lst"));

        // if none found
        if (!bOK) {
            // the mod is not valid
            g_app_mod_path = CTString("");
            CPrintF(TRANSV("Error: MOD not found!\n"));
            // if mod is ok
        } else {
            // remember mod name (the parameter that is passed on cmdline)
            g_app_mod_name = g_app_mod_path;
            g_app_mod_name.DeleteChar(g_app_mod_name.Length() - 1);
            g_app_mod_name = CTFileName(g_app_mod_name).FileName();
        }
    }
    // find eventual extension for the mod's dlls
    g_app_mod_ext = "";
    // DG: apparently both ModEXT.txt and ModExt.txt exist in the wild.
    CTFileName tmp;
    if (ExpandFilePath(EFP_READ, CTString("ModEXT.txt"), tmp) != EFP_NONE) {
        LoadStringVar(CTString("ModEXT.txt"), g_app_mod_ext);
    } else {
        LoadStringVar(CTString("ModExt.txt"), g_app_mod_ext);
    }

    CPrintF(TRANSV("Loading group files...\n"));

    CDynamicArray<CTString>* files = NULL;

    // for each group file in base directory
    files = _pFileSystem->FindFiles(g_app_path, "*.gro");
    int max = files->Count();
    int i;

    // for each .gro file in the directory
    for (i = 0; i < max; i++) {
        // add it to active set
        UNZIPAddArchive(g_app_path + ((*files)[i]));
    }
    delete files;

    // if there is a mod active
    if (g_app_mod_path != "") {
        // for each group file in mod directory
        files = _pFileSystem->FindFiles(g_app_path + g_app_mod_path, "*.gro");
        max = files->Count();
        for (i = 0; i < max; i++) {
            UNZIPAddArchive(g_app_path + g_app_mod_path + ((*files)[i]));
        }
        delete files;
    }

    // if there is a CD path
    if (g_app_cd_path != "") {
        // for each group file on the CD
        files = _pFileSystem->FindFiles(g_app_cd_path, "*.gro");
        max = files->Count();
        for (i = 0; i < max; i++) {
            UNZIPAddArchive(g_app_cd_path + ((*files)[i]));
        }
        delete files;

        // if there is a mod active
        if (g_app_mod_path != "") {
            // for each group file in mod directory
            files = _pFileSystem->FindFiles(g_app_cd_path + g_app_mod_path, "*.gro");
            max = files->Count();
            for (i = 0; i < max; i++) {
                UNZIPAddArchive(g_app_cd_path + g_app_mod_path + ((*files)[i]));
            }
            delete files;
        }
    }

    // try to
    try {
        // read the zip directories
        UNZIPReadDirectoriesReverse_t();
        // if failed
    } catch (char* strError) {
        // report warning
        CPrintF(TRANS("There were group file errors:\n%s"), strError);
    }
    CPrintF("\n");

    const char* dirsep = CFileSystem::GetDirSeparator();
    LoadFileList(g_fs_nocrc, CTFILENAME("Data") + CTString(dirsep) + CTString("NoCRC.lst"));

    _pShell->SetINDEX(CTString("sys") + "_iCPU" + "Misc", 1);
    */
}

void Engine::EndStreams()
{
}

void UseApplicationPath()
{
    //g_app_path = _fnmApp;
}

void IgnoreApplicationPath()
{
    //g_app_path = CTString("");
}

void Engine::DetectCPU()
{
    char strVendor[12 + 1] = { 0 };
#if (defined USE_PORTABLE_C) // rcg10072001
    CPrintF(TRANSV("  (No CPU detection in this binary.)\n"));
#ifdef PLATFORM_PANDORA
    sys_iCPUMHz = 400; // conservative, ARM -> x86 cpu translation is not 1 to 1.
#endif

#else
    strVendor[12] = 0;
    ULONG ulTFMS = 0;
    ULONG ulFeatures = 0;

#if (defined __MSVC_INLINE__)
    // test MMX presence and update flag
    __asm {
    xor     eax,eax           ; // request for basic id
    cpuid
    mov     dword ptr [strVendor+0], ebx
    mov     dword ptr [strVendor+4], edx
    mov     dword ptr [strVendor+8], ecx
    mov     eax,1           ; // request for TFMS feature flags
    cpuid
    mov     dword ptr [ulTFMS], eax ; // remember type, family, model and stepping
    mov     dword ptr [ulFeatures], edx
    }

#elif (defined __GNU_INLINE_X86__)
    ULONG eax, ebx, ecx, edx;
    // test MMX presence and update flag
    __asm__ __volatile__(
#if (defined __GNU_INLINE_X86_64__)
        "cpuid                    \n\t"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
#else
        "movl    %%ebx, %%esi     \n\t"
        "cpuid                    \n\t"
        "xchgl   %%ebx, %%esi     \n\t"
        : "=a"(eax), "=S"(ebx), "=c"(ecx), "=d"(edx)
#endif
        : "a"(0) // request for basic id
    );
    memcpy(strVendor + 0, &ebx, 4);
    memcpy(strVendor + 4, &edx, 4);
    memcpy(strVendor + 8, &ecx, 4);

    __asm__ __volatile__(
#if (defined __GNU_INLINE_X86_64__)
        "cpuid                    \n\t"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
#else
        "movl    %%ebx, %%esi     \n\t"
        "cpuid                    \n\t"
        "xchgl   %%ebx, %%esi     \n\t"
        : "=a"(eax), "=S"(ebx), "=c"(ecx), "=d"(edx)
#endif
        : "a"(1) // request for TFMS feature flags
    );
    ulTFMS = eax;
    ulFeatures = edx;

#endif

    if (ulTFMS == 0)
    {
        CPrintF(TRANSV("  (No CPU detection in this binary.)\n"));
        return;
    }

    INDEX iType = (ulTFMS >> 12) & 0x3;
    INDEX iFamily = (ulTFMS >> 8) & 0xF;
    INDEX iModel = (ulTFMS >> 4) & 0xF;
    INDEX iStepping = (ulTFMS >> 0) & 0xF;

    CPrintF(TRANSV("  Vendor: %s\n"), strVendor);
    CPrintF(TRANSV("  Type: %d, Family: %d, Model: %d, Stepping: %d\n"),
        iType, iFamily, iModel, iStepping);

    BOOL bMMX = ulFeatures & (1 << 23);
    BOOL bCMOV = ulFeatures & (1 << 15);

    CTString strYes = TRANS("Yes");
    CTString strNo = TRANS("No");

    CPrintF(TRANSV("  MMX : %s\n"), (const char*)(bMMX ? strYes : strNo));
    CPrintF(TRANSV("  CMOV: %s\n"), (const char*)(bCMOV ? strYes : strNo));
    CPrintF(TRANSV("  Clock: %.0fMHz\n"), _pTimer->tm_llCPUSpeedHZ / 1E6);

    sys_CPUVendor = strVendor;
    sys_CPUType = iType;
    sys_CPUFamily = iFamily;
    sys_CPUModel = iModel;
    sys_CPUStepping = iStepping;
    sys_CPUHasMMX = bMMX != 0;
    sys_CPUHasCMOV = bCMOV != 0;
    sys_CPUMHz = INDEX(_pTimer->tm_llCPUSpeedHZ / 1E6);

    if (!bMMX)
        FatalError(TRANS("MMX support required but not present!"));
#endif
}

// don't want to export this function
void Engine::PlatformIdentification()
{
// !!! FIXME: Abstract this somehow.
#if (defined PLATFORM_WIN32)
    OSVERSIONINFO osv;
    memset(&osv, 0, sizeof(osv));
    osv.dwOSVersionInfoSize = sizeof(osv);
    if (GetVersionEx(&osv)) {
        switch (osv.dwPlatformId) {
        case VER_PLATFORM_WIN32s:
            sys_strOS = "Win32s";
            break;
        case VER_PLATFORM_WIN32_WINDOWS:
            sys_strOS = "Win9x";
            break;
        case VER_PLATFORM_WIN32_NT:
            sys_strOS = "WinNT";
            break;
        default:
            sys_strOS = "Unknown\n";
            break;
        }

        sys_iOSMajor = osv.dwMajorVersion;
        sys_iOSMinor = osv.dwMinorVersion;
        sys_iOSBuild = osv.dwBuildNumber & 0xFFFF;
        sys_strOSMisc = osv.szCSDVersion;

        CPrintF(TRANSV("  Type: %s\n"), (const char*)sys_strOS);
        CPrintF(TRANSV("  Version: %d.%d, build %d\n"),
            osv.dwMajorVersion, osv.dwMinorVersion, osv.dwBuildNumber & 0xFFFF);
        CPrintF(TRANSV("  Misc: %s\n"), osv.szCSDVersion);
    } else {
        CPrintF(TRANSV("Error getting OS info: %s\n"), GetWindowsError(GetLastError()));
    }

#elif (defined PLATFORM_MACOSX)
    STUBBED("Use some Gestalt replacement, or whatever");
#if 0
    long osver = 0x0000;
    OSErr err = Gestalt(gestaltSystemVersion, &osver);
    if (err != noErr)
        osver = 0x0000;

    sys_iOSMajor = ((osver & 0x0F00) >> 8) + (((osver & 0xF000) >> 12) * 10);
    sys_iOSMinor = ((osver & 0x00F0) >> 4);
    sys_iOSBuild = ((osver & 0x000F) >> 0);
#else
    sys_iOSMajor = 10; // !!! FIXME: just flatly false.
    sys_iOSMinor = 6;
    sys_iOSBuild = 0;
#endif

    sys_strOS = "Mac OS X";
    sys_strOSMisc = "Mac OS";
    CPrintF(TRANSV("  Type: %s\n"), (const char*)sys_strOS);
    CPrintF(TRANSV("  Version: %d.%d.%d\n"),
        (int)sys_iOSMajor, (int)sys_iOSMinor, (int)sys_iOSBuild);

#elif (defined PLATFORM_UNIX) // !!! FIXME: rcg10082001 what to do with this?
    // FIXME: probably want to use uname function on Linux but it isn't totally applicable...hmm...
    sys_OSMajor = 1;
    sys_OSMinor = 0;
    sys_OSBuild = 0;
    sys_OS = "Unix";
    sys_OSMisc = "Unix";
    CPrintF(TRANSV("  Type: %s\n"), (const char*)sys_OS);

#else
#error Do something with this for your platform.
#endif
}