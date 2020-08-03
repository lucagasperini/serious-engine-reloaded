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

using namespace SER;
// startup engine
void init_engine(const char* argv0, CTString strGameID)
{ /*
    SanityCheckTypes();

    PlatformSpecificInit();

    const char* gamename = "UnknownGame";
    if (strGameID != "")
        gamename = (const char*)strGameID;
    _pFileSystem = CFileSystem::GetInstance(argv0, gamename); // rcg10082001

#pragma message(">> Remove this from SE_InitEngine : _bWorldEditorApp")
    if (strGameID == "SeriousEditor") {
        _bWorldEditorApp = TRUE;
    }

    AnalyzeApplicationPath();
    _fnmApplicationPath = CTString(strDirPath);
    _fnmApplicationExe = CTString(strExePath);

    // rcg01012002 calculate user dir.
    char buf[MAX_PATH];
    _pFileSystem->GetUserDirectory(buf, sizeof(buf));
    _fnmUserDir = CTString(buf);

    try {
        _fnmApplicationExe.RemoveApplicationPath_t();
    } catch (char* strError) {
        (void)strError;
        ASSERT(FALSE);
    }

    _pConsole = new CConsole;
    if (_strLogFile == "") {
        _strLogFile = CTFileName(CTString(strExePath)).FileName();
        // chop off end of Unix executable filename... --ryan.
        _strLogFile.ReplaceSubstr(CTString("-bin"), CTString(""));
    }
    _pConsole->Initialize(_fnmUserDir + _strLogFile + ".log", 90, 512);

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
    CPrintF(TRANSV("Executable: %s\n"), strExePath);
    CPrintF(TRANSV("Assumed engine directory: %s\n"), (const char*)_fnmApplicationPath);

    CPrintF("\n");

    // report os info
    CPrintF(TRANSV("Examining underlying OS...\n"));

    PlatformIdentification();

    CPrintF("\n");

    // (rcg11232001 this is where _pShell was originally created.)

    // report CPU
    CPrintF(TRANSV("Detecting CPU...\n"));
    DetectCPUWrapper();
    CPrintF("\n");

    // report memory info
    extern void ReportGlobalMemoryStatus(void);
    ReportGlobalMemoryStatus();

    SetupMemoryManager();
    // initialize zip semaphore
    zip_csLock.cs_iIndex = -1; // not checked for locking order

    // rcg10082001 Honestly, all of this is meaningless in a multitasking OS.
    //  That includes Windows, too.
    SetupSecondaryStorage(); /// FIXME: does that name make sense

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

    // init MODs and stuff ...
    extern void InitStreams(void);
    InitStreams();
    // keep mod name in sys cvar
    sys_strModName = _strModName;
    sys_strModExt = _strModExt;

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

    InitSystemGammaSettings();
    InitIFeel(); // on non win32 platforms this will be optimized out if we play our cards right
*/
}
