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

/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "Engine/StdH.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// !!! FIXME : rcg10162001 Need this anymore, since _findfirst() is abstracted?
#ifdef PLATFORM_WIN32
#include <io.h>
#endif

#include "Stream.h"
#include <Engine/Base/Protection.h>

#include <Engine/Base/CRC.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/FileSystem.h>
#include <Engine/Base/ListIterator.inl>
#include <Engine/Base/Memory.h>
#include <Engine/Base/ProgressHook.h>
#include <Engine/Base/Shell.h>
#include <Engine/Base/Unzip.h>
#include <Engine/Templates/DynamicStackArray.cpp>
//#include <Engine/Templates/NameTable_CTFileName.h>
#include <Engine/Base/Stream.h>
#include <Engine/Templates/StaticArray.cpp>

#include <Engine/Templates/Stock_CModelData.h>
#include <Engine/Templates/Stock_CTextureData.h>

using namespace SER;

// default size of page used for stream IO operations (4Kb)
ULONG g_page_size = 0;
// maximum length of file that can be saved (default: 128Mb)
ULONG g_fs_max_save_length = (1UL << 20) * 128;
INDEX g_fs_prefer_zip = FALSE;

// set if current thread has currently enabled stream handling
THREADLOCAL(BOOL, g_thread_can_handle_stream, FALSE);
// list of currently opened streams
ULONG g_mem_virtual_space = 0;
ULONG g_mem_virtual_space_total = 0;
THREADLOCAL(CListHead*, g_mem_open_stream, NULL);

// global string with application path
CTFileName SER::g_app_path;
// global string with filename of the started application
CTFileName SER::g_app_exe;
// global string with user-specific writable directory.
CTFileName SER::g_user_dir;
// global string with current MOD path
CTFileName SER::g_app_mod_path;
// global string with current name (the parameter that is passed on cmdline)
CTString SER::g_app_mod_name;
// global string with url to be shown to users that don't have the mod installed
// (should be set by game.dll)
CTString SER::g_app_mod_url;
// global string with current MOD extension (for adding to dlls)
CTString SER::g_app_mod_ext;
// global string with CD path (for minimal installations)
CTFileName SER::g_app_cd_path;

// include/exclude lists for base dir writing/browsing
CDynamicStackArray<CTFileName> g_fs_base_write_include;
CDynamicStackArray<CTFileName> g_fs_base_write_exclude;
CDynamicStackArray<CTFileName> g_fs_base_browse_include;
CDynamicStackArray<CTFileName> g_fs_base_browse_exclude;
// list of paths or patterns that are not included when making CRCs for network connection
// this is used to enable connection between different localized versions
CDynamicStackArray<CTFileName> g_fs_nocrc;
/*
// load a filelist
static BOOL LoadFileList(CDynamicStackArray<CTFileName>& afnm, const CTFileName& fnmList)
{
    afnm.PopAll();
    try {
        FileStream strm;
        strm.Open_t(fnmList);
        while (!strm.AtEOF()) {
            CTString strLine;
            strm.GetLine_t(strLine);
            strLine.TrimSpacesLeft();
            strLine.TrimSpacesRight();
            if (strLine != "") {
                afnm.Push() = strLine;
            }
        }
        return TRUE;
    } catch (char* strError) {
        CPrintF("%s\n", strError);
        return FALSE;
    }
}
*/
extern BOOL FileMatchesList(CDynamicStackArray<CTFileName>& afnm, const CTFileName& fnm)
{
    for (INDEX i = 0; i < afnm.Count(); i++) {
        if (fnm.Matches(afnm[i]) || fnm.HasPrefix(afnm[i])) {
            return TRUE;
        }
    }
    return FALSE;
}
/*
static CTFileName _fnmApp;

void InitStreams(void)
{
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
}

void EndStreams(void)
{
}

void UseApplicationPath(void)
{
    g_app_path = _fnmApp;
}

void IgnoreApplicationPath(void)
{
    g_app_path = CTString("");
}
*/
/////////////////////////////////////////////////////////////////////////////
// Helper functions

/* Static function enable stream handling. */
void Stream::EnableStreamHandling(void)
{
    ASSERT(!g_thread_can_handle_stream && g_mem_open_stream == NULL);

    g_thread_can_handle_stream = TRUE;
    g_mem_open_stream = new CListHead;
}

/* Static function disable stream handling. */
void Stream::DisableStreamHandling(void)
{
    ASSERT(g_thread_can_handle_stream && g_mem_open_stream != NULL);

    g_thread_can_handle_stream = FALSE;
    delete g_mem_open_stream;
    g_mem_open_stream = NULL;
}

/*
 * Throw an exception of formatted string.
 */
void Stream::Throw_t(const char* strFormat, ...) // throws char *
{
    const SLONG slBufferSize = 256;
    char strFormatBuffer[slBufferSize];
    static char* strBuffer = NULL;

    // ...and yes, you are screwed if you call this in a catch block and
    //  try to access the previous text again.
    delete[] strBuffer;
    strBuffer = new char[slBufferSize];

    // add the stream description to the format string
    _snprintf(strFormatBuffer, slBufferSize, "%s (%s)", strFormat, (const char*)strm_strStreamDescription);
    // format the message in buffer
    va_list arg;
    va_start(arg, strFormat); // variable arguments start after this argument
    _vsnprintf(strBuffer, slBufferSize, strFormatBuffer, arg);
    va_end(arg);
    throw strBuffer;
}

/////////////////////////////////////////////////////////////////////////////
// Binary access methods

/* Get CRC32 of stream */
ULONG Stream::GetStreamCRC32_t(void)
{
    // remember where stream is now
    SLONG slOldPos = GetPos_t();
    // go to start of file
    SetPos_t(0);
    // get size of file
    SLONG slFileSize = GetStreamSize();

    ULONG ulCRC;
    CRC_Start(ulCRC);

    // for each block in file
    const SLONG slBlockSize = 4096;
    for (SLONG slPos = 0; slPos < slFileSize; slPos += slBlockSize) {
        // read the block
        UBYTE aubBlock[slBlockSize];
        SLONG slThisBlockSize = Min(slFileSize - slPos, slBlockSize);
        Read_t(aubBlock, slThisBlockSize);
        // checksum it
        CRC_AddBlock(ulCRC, aubBlock, slThisBlockSize);
    }

    // restore position
    SetPos_t(slOldPos);

    CRC_Finish(ulCRC);
    return ulCRC;
}

/////////////////////////////////////////////////////////////////////////////
// Text access methods

/* Get a line of text from file. */
// throws char *
void Stream::GetLine_t(char* strBuffer, SLONG slBufferSize, char cDelimiter /*='\n'*/)
{
    // check parameters
    ASSERT(strBuffer != NULL && slBufferSize > 0);
    // check that the stream can be read
    ASSERT(IsReadable());
    // letters slider
    INDEX iLetters = 0;
    // test if EOF reached
    if (AtEOF()) {
        ThrowF_t(TRANS("EOF reached, file %s"), (const char*)strm_strStreamDescription);
    }
    // get line from istream
    FOREVER
    {
        char c;
        Read_t(&c, 1);

        if (AtEOF()) {
            // cut off
            strBuffer[iLetters] = 0;
            break;
        }

        // don't read "\r" characters but rather act like they don't exist
        if (c != '\r') {
            strBuffer[iLetters] = c;
            // stop reading when delimiter loaded
            if (strBuffer[iLetters] == cDelimiter) {
                // convert delimiter to zero
                strBuffer[iLetters] = 0;
                // jump over delimiter
                //Seek_t(1, SD_CUR);
                break;
            }
            // jump to next destination letter
            iLetters++;
        }
        // test if maximum buffer lenght reached
        if (iLetters == slBufferSize) {
            return;
        }
    }
}

void Stream::GetLine_t(CTString& strLine, char cDelimiter /*='\n'*/) // throw char *
{
    char strBuffer[1024];
    GetLine_t(strBuffer, sizeof(strBuffer) - 1, cDelimiter);
    strLine = strBuffer;
}

/* Put a line of text into file. */
void Stream::PutLine_t(const char* strBuffer) // throws char *
{
    // check parameters
    ASSERT(strBuffer != NULL);
    // check that the stream is writteable
    ASSERT(IsWriteable());
    // get string length
    INDEX iStringLength = strlen(strBuffer);
    // put line into stream
    Write_t(strBuffer, iStringLength);
    // write "\r\n" into stream
    Write_t("\r\n", 2);
}

void Stream::PutString_t(const char* strString) // throw char *
{
    // check parameters
    ASSERT(strString != NULL);
    // check that the stream is writteable
    ASSERT(IsWriteable());
    // get string length
    INDEX iStringLength = strlen(strString);
    // put line into stream
    for (INDEX iLetter = 0; iLetter < iStringLength; iLetter++) {
        if (*strString == '\n') {
            // write "\r\n" into stream
            Write_t("\r\n", 2);
            strString++;
        } else {
            Write_t(strString++, 1);
        }
    }
}

void Stream::FPrintF_t(const char* strFormat, ...) // throw char *
{
    const SLONG slBufferSize = 2048;
    char strBuffer[slBufferSize];
    // format the message in buffer
    va_list arg;
    va_start(arg, strFormat); // variable arguments start after this argument
    _vsnprintf(strBuffer, slBufferSize, strFormat, arg);
    va_end(arg);
    // print the buffer
    PutString_t(strBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// Chunk reading/writing methods

ChunkID Stream::GetID_t(void) // throws char *
{
    ChunkID cidToReturn;
    Read_t(&cidToReturn.cid_ID[0], CID_LENGTH);
    return (cidToReturn);
}

ChunkID Stream::PeekID_t(void) // throw char *
{
    // read the chunk id
    ChunkID cidToReturn;
    Read_t(&cidToReturn.cid_ID[0], CID_LENGTH);
    // return the stream back
    Seek_t(-CID_LENGTH, SD_CUR);
    return (cidToReturn);
}

void Stream::ExpectID_t(const ChunkID& cidExpected) // throws char *
{
    ChunkID cidToCompare;

    Read_t(&cidToCompare.cid_ID[0], CID_LENGTH);
    if (!(cidToCompare == cidExpected)) {
        ThrowF_t(TRANS("Chunk ID validation failed.\nExpected ID \"%s\" but found \"%s\"\n"),
            cidExpected.cid_ID, cidToCompare.cid_ID);
    }
}
void Stream::ExpectKeyword_t(const CTString& strKeyword) // throw char *
{
    // check that the keyword is present
    const INDEX total = (INDEX)strlen(strKeyword);
    for (INDEX iKeywordChar = 0; iKeywordChar < total; iKeywordChar++) {
        SBYTE chKeywordChar;
        (*this) >> chKeywordChar;
        if (chKeywordChar != strKeyword[iKeywordChar]) {
            ThrowF_t(TRANS("Expected keyword %s not found"), (const char*)strKeyword);
        }
    }
}

SLONG Stream::GetSize_t(void) // throws char *
{
    SLONG chunkSize;

    Read_t((char*)&chunkSize, sizeof(SLONG));
    return (chunkSize);
}

void Stream::ReadRawChunk_t(void* pvBuffer, SLONG slSize) // throws char *
{
    Read_t((char*)pvBuffer, slSize);
}

void Stream::ReadChunk_t(void* pvBuffer, SLONG slExpectedSize) // throws char *
{
    if (slExpectedSize != GetSize_t())
        throw TRANS("Chunk size not equal as expected size");
    Read_t((char*)pvBuffer, slExpectedSize);
}

void Stream::ReadFullChunk_t(const ChunkID& cidExpected, void* pvBuffer,
    SLONG slExpectedSize) // throws char *
{
    ExpectID_t(cidExpected);
    ReadChunk_t(pvBuffer, slExpectedSize);
};

void* Stream::ReadChunkAlloc_t(SLONG slSize) // throws char *
{
    UBYTE* buffer;
    SLONG chunkSize;

    if (slSize != 0)
        chunkSize = slSize;
    else
        chunkSize = GetSize_t(); // throws char *
    buffer = (UBYTE*)AllocMemory(chunkSize);
    if (buffer == NULL)
        throw TRANS("ReadChunkAlloc: Unable to allocate needed amount of memory.");
    Read_t((char*)buffer, chunkSize); // throws char *
    return buffer;
}
void Stream::ReadStream_t(Stream& strmOther) // throw char *
{
    //TODO: do more testing of this method
    strmOther.Write_t(ReadChunkAlloc_t(), GetStreamSize());
}

void Stream::WriteID_t(const ChunkID& cidSave) // throws char *
{
    Write_t(&cidSave.cid_ID[0], CID_LENGTH);
}

void Stream::WriteSize_t(SLONG slSize) // throws char *
{
    Write_t((char*)&slSize, sizeof(SLONG));
}

void Stream::WriteRawChunk_t(void* pvBuffer, SLONG slSize) // throws char *
{
    Write_t((char*)pvBuffer, slSize);
}

void Stream::WriteChunk_t(void* pvBuffer, SLONG slSize) // throws char *
{
    WriteSize_t(slSize);
    WriteRawChunk_t(pvBuffer, slSize);
}

void Stream::WriteFullChunk_t(const ChunkID& cidSave, void* pvBuffer,
    SLONG slSize) // throws char *
{
    WriteID_t(cidSave); // throws char *
    WriteChunk_t(pvBuffer, slSize); // throws char *
}
void Stream::WriteStream_t(Stream& strmOther) // throw char *
{
    //TODO: do more testing of this method
    Write_t(strmOther.ReadChunkAlloc_t(), strmOther.GetStreamSize());
}

// whether or not the given pointer is coming from this stream (mainly used for exception handling)
BOOL Stream::PointerInStream(void* pPointer)
{
    // safe to return FALSE, we're using virtual functions anyway
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// filename dictionary operations

// enable dictionary in writable file from this point
void Stream::DictionaryWriteBegin_t(const CTFileName& fnmImportFrom, SLONG slImportOffset)
{
    /*
    ASSERT(strm_slDictionaryPos == 0);
    ASSERT(strm_dmDictionaryMode == DM_NONE);
    strm_ntDictionary.SetAllocationParameters(100, 5, 5);
    strm_ctDictionaryImported = 0;

    // if importing an existing dictionary to start with
    if (fnmImportFrom != "") {
        // open that file
        FileStream strmOther;
        strmOther.Open_t(fnmImportFrom);
        // read the dictionary in that stream
        strmOther.ReadDictionary_intenal_t(slImportOffset);
        // copy the dictionary here
        CopyDictionary(strmOther);
        // write dictionary importing data
        WriteID_t("DIMP"); // dictionary import
        *this << fnmImportFrom << slImportOffset;
        // remember how many filenames were imported
        strm_ctDictionaryImported = strm_afnmDictionary.Count();
    }

    // write dictionary position chunk id
    WriteID_t("DPOS"); // dictionary position
    // remember where position will be placed
    strm_slDictionaryPos = GetPos_t();
    // leave space for position
    *this << SLONG(0);

    // start dictionary
    strm_dmDictionaryMode = DM_ENABLED;
    */
}

// write the dictionary (usually at the end of file)
void Stream::DictionaryWriteEnd_t(void)
{
    /*
    ASSERT(strm_dmDictionaryMode == DM_ENABLED);
    ASSERT(strm_slDictionaryPos > 0);
    // remember the dictionary position chunk position
    SLONG slDictPos = strm_slDictionaryPos;
    // mark that now saving dictionary
    strm_slDictionaryPos = -1;
    // remember where dictionary begins
    SLONG slDictBegin = GetPos_t();
    // start dictionary processing
    strm_dmDictionaryMode = DM_PROCESSING;

    WriteID_t("DICT"); // dictionary
    // write number of used filenames
    INDEX ctFileNames = strm_afnmDictionary.Count();
    INDEX ctFileNamesNew = ctFileNames - strm_ctDictionaryImported;
    *this << ctFileNamesNew;
    // for each filename
    for (INDEX iFileName = strm_ctDictionaryImported; iFileName < ctFileNames; iFileName++) {
        // write it to disk
        *this << strm_afnmDictionary[iFileName];
    }
    WriteID_t("DEND"); // dictionary end

    // remember where is end of dictionary
    SLONG slContinue = GetPos_t();

    // write the position back to dictionary position chunk
    SetPos_t(slDictPos);
    *this << slDictBegin;

    // stop dictionary processing
    strm_dmDictionaryMode = DM_NONE;
    strm_ntDictionary.Clear();
    strm_afnmDictionary.Clear();

    // return to end of dictionary
    SetPos_t(slContinue);
    strm_slDictionaryPos = 0;
    */
}

// read the dictionary from given offset in file (internal function)
void Stream::ReadDictionary_intenal_t(SLONG slOffset)
{
    /*
    // remember where to continue loading
    SLONG slContinue = GetPos_t();

    // go to dictionary beginning
    SetPos_t(slOffset);

    // start dictionary processing
    strm_dmDictionaryMode = DM_PROCESSING;

    ExpectID_t("DICT"); // dictionary
    // read number of new filenames
    INDEX ctFileNamesOld = strm_afnmDictionary.Count();
    INDEX ctFileNamesNew;
    *this >> ctFileNamesNew;
    // if there are any new filenames
    if (ctFileNamesNew > 0) {
        // create that much space
        strm_afnmDictionary.Push(ctFileNamesNew);
        // for each filename
        for (INDEX iFileName = ctFileNamesOld; iFileName < ctFileNamesOld + ctFileNamesNew; iFileName++) {
            // read it
            *this >> strm_afnmDictionary[iFileName];
        }
    }
    ExpectID_t("DEND"); // dictionary end

    // remember where end of dictionary is
    strm_slDictionaryPos = GetPos_t();

    // return to continuing position
    SetPos_t(slContinue);
    */
}

// copy filename dictionary from another stream
void Stream::CopyDictionary(Stream& strmOther)
{
    /*
    strm_afnmDictionary = strmOther.strm_afnmDictionary;
    for (INDEX i = 0; i < strm_afnmDictionary.Count(); i++) {
        strm_ntDictionary.Add(&strm_afnmDictionary[i]);
    }
    */
}

SLONG Stream::DictionaryReadBegin_t(void)
{
    /*
    ASSERT(strm_dmDictionaryMode == DM_NONE);
    ASSERT(strm_slDictionaryPos == 0);
    strm_ntDictionary.SetAllocationParameters(100, 5, 5);

    SLONG slImportOffset = 0;
    // if there is imported dictionary
    if (PeekID_t() == ChunkID("DIMP")) { // dictionary import
        // read dictionary importing data
        ExpectID_t("DIMP"); // dictionary import
        CTFileName fnmImportFrom;
        *this >> fnmImportFrom >> slImportOffset;

        // open that file
        FileStream strmOther;
        strmOther.Open_t(fnmImportFrom);
        // read the dictionary in that stream
        strmOther.ReadDictionary_intenal_t(slImportOffset);
        // copy the dictionary here
        CopyDictionary(strmOther);
    }

    // if the dictionary is not here
    if (PeekID_t() != ChunkID("DPOS")) { // dictionary position
        // do nothing
        return 0;
    }

    // read dictionary position
    ExpectID_t("DPOS"); // dictionary position
    SLONG slDictBeg;
    *this >> slDictBeg;

    // read the dictionary from that offset in file
    ReadDictionary_intenal_t(slDictBeg);

    // stop dictionary processing - go to dictionary using
    strm_dmDictionaryMode = DM_ENABLED;

    // return offset of dictionary for later cross-file importing
    if (slImportOffset != 0) {
        return slImportOffset;
    } else {
        return slDictBeg;
    }
    */
}

void Stream::DictionaryReadEnd_t(void)
{
    /*
    if (strm_dmDictionaryMode == DM_ENABLED) {
        ASSERT(strm_slDictionaryPos > 0);
        // just skip the dictionary (it was already read)
        SetPos_t(strm_slDictionaryPos);
        strm_slDictionaryPos = 0;
        strm_dmDictionaryMode = DM_NONE;
        strm_ntDictionary.Clear();

        // for each filename
        INDEX ctFileNames = strm_afnmDictionary.Count();
        for (INDEX iFileName = 0; iFileName < ctFileNames; iFileName++) {
            CTFileName& fnm = strm_afnmDictionary[iFileName];
            // if not preloaded
            if (fnm.fnm_pserPreloaded == NULL) {
                // skip
                continue;
            }
            // free preloaded instance
            CTString strExt = fnm.FileExt();
            if (strExt == ".tex") {
                _pTextureStock->Release((CTextureData*)fnm.fnm_pserPreloaded);
            } else if (strExt == ".mdl") {
                _pModelStock->Release((CModelData*)fnm.fnm_pserPreloaded);
            }
        }

        strm_afnmDictionary.Clear();
    }
    */
}
void Stream::DictionaryPreload_t(void)
{
    /*
    INDEX ctFileNames = strm_afnmDictionary.Count();
    // for each filename
    for (INDEX iFileName = 0; iFileName < ctFileNames; iFileName++) {
        // preload it
        CTFileName& fnm = strm_afnmDictionary[iFileName];
        CTString strExt = fnm.FileExt();
        CallProgressHook_t(FLOAT(iFileName) / ctFileNames);
        try {
            if (strExt == ".tex") {
                fnm.fnm_pserPreloaded = _pTextureStock->Obtain_t(fnm);
            } else if (strExt == ".mdl") {
                fnm.fnm_pserPreloaded = _pModelStock->Obtain_t(fnm);
            }
        } catch (char* strError) {
            CPrintF(TRANS("Cannot preload %s: %s\n"), (const char*)(CTString&)fnm, strError);
        }
    }
    */
}

/////////////////////////////////////////////////////////////////////////////
// General construction/destruction

/* Default constructor. */
Stream::Stream(void)
//: strm_ntDictionary(*new CNameTable_CTFileName)
{
    strm_strStreamDescription = "";
    //strm_slDictionaryPos = 0;
    //strm_dmDictionaryMode = DM_NONE;
}

/* Destructor. */
Stream::~Stream(void)
{
    //strm_ntDictionary.Clear();
    //strm_afnmDictionary.Clear();

    //delete &strm_ntDictionary;
}

/////////////////////////////////////////////////////////////////////////////
// File stream opening/closing methods

/*
 * Default constructor.
 */
FileStream::FileStream(void)
{
    fstrm_pFile = NULL;
    // mark that file is created for writing
    fstrm_bReadOnly = TRUE;
    fstrm_iZipHandle = -1;
    fstrm_iZipLocation = 0;
    fstrm_pubZipBuffer = NULL;
}

/*
 * Destructor.
 */
FileStream::~FileStream(void)
{
    // close stream
    if (fstrm_pFile != NULL || fstrm_iZipHandle != -1) {
        Close();
    }
}

/*
 * Open an existing file.
 */
// throws char *
void FileStream::Open_t(const CTFileName& fnFileName, Stream::OpenMode om /*=OM_READ*/)
{
    // if current thread has not enabled stream handling
    if (!g_thread_can_handle_stream) {
        // error
        ::ThrowF_t(TRANS("Cannot open file `%s', stream handling is not enabled for this thread"),
            (const char*)(CTString&)fnFileName);
    }

    // check parameters
    ASSERT(strlen(fnFileName) > 0);
    // check that the file is not open
    ASSERT(fstrm_pFile == NULL && fstrm_iZipHandle == -1);

    // expand the filename to full path
    CTFileName fnmFullFileName;
    INDEX iFile = ExpandFilePath((om == OM_READ) ? EFP_READ : EFP_WRITE, fnFileName, fnmFullFileName);

    // if read only mode requested
    if (om == OM_READ) {
        // initially, no physical file
        fstrm_pFile = NULL;
        // if zip file
        if (iFile == EFP_MODZIP || iFile == EFP_BASEZIP) {
            // open from zip
            fstrm_iZipHandle = UNZIPOpen_t(fnmFullFileName);
            fstrm_slZipSize = UNZIPGetSize(fstrm_iZipHandle);
            // load the file from the zip in the buffer
            fstrm_pubZipBuffer = new UBYTE[fstrm_slZipSize];
            UNZIPReadBlock_t(fstrm_iZipHandle, fstrm_pubZipBuffer, 0, fstrm_slZipSize);
            // if it is a physical file
        } else if (iFile == EFP_FILE) {
            // open file in read only mode
            fstrm_pFile = fopen(fnmFullFileName, "rb");
        }
        fstrm_bReadOnly = TRUE;

        // if write mode requested
    } else if (om == OM_WRITE) {
        // open file for reading and writing
        fstrm_pFile = fopen(fnmFullFileName, "rb+");
        fstrm_bReadOnly = FALSE;
        // if unknown mode
    } else {
        FatalError(TRANS("File stream opening requested with unknown open mode: %d\n"), om);
    }

    // if openning operation was not successfull
    if (fstrm_pFile == NULL && fstrm_iZipHandle == -1) {
        // throw exception
        Throw_t(TRANS("Cannot open file `%s' (%s)"), (const char*)(CTString&)fnmFullFileName,
            strerror(errno));
    }

    // if file opening was successfull, set stream description to file name
    strm_strStreamDescription = fnmFullFileName;
    // add this newly opened file into opened stream list
    g_mem_open_stream->AddTail(strm_lnListNode);
}

static void MakeSureDirectoryPathExists(const CTFileName& fnmFullFileName)
{
    STUBBED("!!! FIXME: get the code back in from Ryan's original port.");
}

/*
 * Create a new file or overwrite existing.
 */
void FileStream::Create_t(const CTFileName& fnFileName,
    enum Stream::CreateMode cm) // throws char *
{
    (void)cm; // OBSOLETE!

    CTFileName fnFileNameAbsolute = fnFileName;
    fnFileNameAbsolute.SetAbsolutePath();

    // if current thread has not enabled stream handling
    if (!g_thread_can_handle_stream) {
        // error
        ::ThrowF_t(TRANS("Cannot create file `%s', stream handling is not enabled for this thread"),
            (const char*)(CTString&)fnFileNameAbsolute);
    }

    CTFileName fnmFullFileName;
    /* INDEX iFile = */ ExpandFilePath(EFP_WRITE, fnFileNameAbsolute, fnmFullFileName);

    // check parameters
    ASSERT(strlen(fnFileNameAbsolute) > 0);
    // check that the file is not open
    ASSERT(fstrm_pFile == NULL);

    // create the directory for the new file if it doesn't exist yet
    MakeSureDirectoryPathExists(fnmFullFileName);

    // open file stream for writing (destroy file context if file existed before)
    fstrm_pFile = fopen(fnmFullFileName, "wb+");
    // if not successfull
    if (fstrm_pFile == NULL) {
        // throw exception
        Throw_t(TRANS("Cannot create file `%s' (%s)"), (const char*)(CTString&)fnmFullFileName,
            strerror(errno));
    }
    // if file creation was successfull, set stream description to file name
    strm_strStreamDescription = fnFileNameAbsolute;
    // mark that file is created for writing
    fstrm_bReadOnly = FALSE;
    // add this newly created file into opened stream list
    g_mem_open_stream->AddTail(strm_lnListNode);
}

/*
 * Close an open file.
 */
void FileStream::Close(void)
{
    // if file is not open
    if (fstrm_pFile == NULL && fstrm_iZipHandle == -1) {
        ASSERT(FALSE);
        return;
    }

    // clear stream description
    strm_strStreamDescription = "";
    // remove file from list of curently opened streams
    strm_lnListNode.Remove();

    // if file on disk
    if (fstrm_pFile != NULL) {
        // close file
        fclose(fstrm_pFile);
        fstrm_pFile = NULL;
        // if file in zip
    } else if (fstrm_iZipHandle >= 0) {
        // close zip entry
        UNZIPClose(fstrm_iZipHandle);
        fstrm_iZipHandle = -1;
        delete[] fstrm_pubZipBuffer;
        g_mem_virtual_space -= fstrm_slZipSize;
        //CPrintF("Freed virtual memory with size ^c00ff00%d KB^C (now %d KB)\n", (fstrm_slZipSize / 1000), (g_mem_virtual_space / 1000));
    }

    // clear dictionary vars
    /*
    strm_dmDictionaryMode = DM_NONE;
    strm_ntDictionary.Clear();
    strm_afnmDictionary.Clear();
    strm_slDictionaryPos = 0;
    */
}

/* Get CRC32 of stream */
ULONG FileStream::GetStreamCRC32_t(void)
{
    // if file on disk
    if (fstrm_pFile != NULL) {
        // use base class implementation (really calculates the CRC)
        return Stream::GetStreamCRC32_t();
        // if file in zip
    } else if (fstrm_iZipHandle >= 0) {
        return UNZIPGetCRC(fstrm_iZipHandle);
    } else {
        ASSERT(FALSE);
        return 0;
    }
}

/* Read a block of data from stream. */
void FileStream::Read_t(void* pvBuffer, SLONG slSize)
{
    if (fstrm_iZipHandle != -1) {
        memcpy(pvBuffer, fstrm_pubZipBuffer + fstrm_iZipLocation, slSize);
        fstrm_iZipLocation += slSize;
        return;
    }

    fread(pvBuffer, slSize, 1, fstrm_pFile);
}

/* Write a block of data to stream. */
void FileStream::Write_t(const void* pvBuffer, SLONG slSize)
{
    if (fstrm_bReadOnly || fstrm_iZipHandle != -1) {
        throw "Stream is read-only!";
    }

    fwrite(pvBuffer, slSize, 1, fstrm_pFile);
}

/* Seek in stream. */
void FileStream::Seek_t(SLONG slOffset, enum SeekDir sd)
{
    if (fstrm_iZipHandle != -1) {
        switch (sd) {
        case SD_BEG:
            fstrm_iZipLocation = slOffset;
            break;
        case SD_CUR:
            fstrm_iZipLocation += slOffset;
            break;
        case SD_END:
            fstrm_iZipLocation = GetSize_t() + slOffset;
            break;
        }
    } else {
        fseek(fstrm_pFile, slOffset, sd);
    }
}

/* Set absolute position in stream. */
void FileStream::SetPos_t(SLONG slPosition)
{
    Seek_t(slPosition, SD_BEG);
}

/* Get absolute position in stream. */
SLONG FileStream::GetPos_t(void)
{
    if (fstrm_iZipHandle != -1) {
        return fstrm_iZipLocation;
    } else {
        return ftell(fstrm_pFile);
    }
}

/* Get size of stream */
SLONG FileStream::GetStreamSize(void)
{
    if (fstrm_iZipHandle != -1) {
        return UNZIPGetSize(fstrm_iZipHandle);
    } else {
        long lCurrentPos = ftell(fstrm_pFile);
        fseek(fstrm_pFile, 0, SD_END);
        long lRet = ftell(fstrm_pFile);
        fseek(fstrm_pFile, lCurrentPos, SD_BEG);
        return lRet;
    }
}

/* Check if file position points to the EOF */
BOOL FileStream::AtEOF(void)
{
    if (fstrm_iZipHandle != -1) {
        return fstrm_iZipLocation >= fstrm_slZipSize;
    } else {
        int eof = feof(fstrm_pFile);
        return eof != 0;
    }
}

// whether or not the given pointer is coming from this stream (mainly used for exception handling)
BOOL FileStream::PointerInStream(void* pPointer)
{
    // we're not using virtual allocation buffers so it's fine to return FALSE here.
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Memory stream construction/destruction

/*
 * Create dynamically resizing stream for reading/writing.
 */
MemoryStream::MemoryStream(void)
{
    // if current thread has not enabled stream handling
    if (!g_thread_can_handle_stream) {
        // error
        ::FatalError(TRANS("Can create memory stream, stream handling is not enabled for this thread"));
    }

    mstrm_ctLocked = 0;
    mstrm_bReadable = TRUE;
    mstrm_bWriteable = TRUE;
    mstrm_slLocation = 0;
    // set stream description
    strm_strStreamDescription = "dynamic memory stream";
    // add this newly created memory stream into opened stream list
    g_mem_open_stream->AddTail(strm_lnListNode);
    // allocate amount of memory needed to hold maximum allowed file length (when saving)
    mstrm_pubBuffer = new UBYTE[g_fs_max_save_length];
    mstrm_pubBufferEnd = mstrm_pubBuffer + g_fs_max_save_length;
    mstrm_pubBufferMax = mstrm_pubBuffer;
}

/*
 * Create static stream from given buffer.
 */
MemoryStream::MemoryStream(void* pvBuffer, SLONG slSize,
    Stream::OpenMode om /*= Stream::OM_READ*/)
{
    // if current thread has not enabled stream handling
    if (!g_thread_can_handle_stream) {
        // error
        ::FatalError(TRANS("Can create memory stream, stream handling is not enabled for this thread"));
    }

    // allocate amount of memory needed to hold maximum allowed file length (when saving)
    mstrm_pubBuffer = new UBYTE[g_fs_max_save_length];
    mstrm_pubBufferEnd = mstrm_pubBuffer + g_fs_max_save_length;
    mstrm_pubBufferMax = mstrm_pubBuffer + slSize;
    // copy given block of memory into memory file
    memcpy(mstrm_pubBuffer, pvBuffer, slSize);

    mstrm_ctLocked = 0;
    mstrm_bReadable = TRUE;
    mstrm_slLocation = 0;
    // if stram is opened in read only mode
    if (om == OM_READ) {
        mstrm_bWriteable = FALSE;
    }
    // otherwise, write is enabled
    else {
        mstrm_bWriteable = TRUE;
    }
    // set stream description
    strm_strStreamDescription = "dynamic memory stream";
    // add this newly created memory stream into opened stream list
    g_mem_open_stream->AddTail(strm_lnListNode);
}

/* Destructor. */
MemoryStream::~MemoryStream(void)
{
    ASSERT(mstrm_ctLocked == 0);
    delete[] mstrm_pubBuffer;
    // remove memory stream from list of curently opened streams
    strm_lnListNode.Remove();
}

/////////////////////////////////////////////////////////////////////////////
// Memory stream buffer operations

/*
 * Lock the buffer contents and it's size.
 */
void MemoryStream::LockBuffer(void** ppvBuffer, SLONG* pslSize)
{
    mstrm_ctLocked++;
    ASSERT(mstrm_ctLocked > 0);

    *ppvBuffer = mstrm_pubBuffer;
    *pslSize = GetSize_t();
}

/*
 * Unlock buffer.
 */
void MemoryStream::UnlockBuffer()
{
    mstrm_ctLocked--;
    ASSERT(mstrm_ctLocked >= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Memory stream overrides from Stream

BOOL MemoryStream::IsReadable(void)
{
    return mstrm_bReadable && (mstrm_ctLocked == 0);
}
BOOL MemoryStream::IsWriteable(void)
{
    return mstrm_bWriteable && (mstrm_ctLocked == 0);
}
BOOL MemoryStream::IsSeekable(void)
{
    return TRUE;
}

/* Read a block of data from stream. */
void MemoryStream::Read_t(void* pvBuffer, SLONG slSize)
{
    memcpy(pvBuffer, mstrm_pubBuffer + mstrm_slLocation, slSize);
    mstrm_slLocation += slSize;
}

/* Write a block of data to stream. */
void MemoryStream::Write_t(const void* pvBuffer, SLONG slSize)
{
    memcpy(mstrm_pubBuffer + mstrm_slLocation, pvBuffer, slSize);
    mstrm_slLocation += slSize;

    if (mstrm_pubBuffer + mstrm_slLocation > mstrm_pubBufferMax) {
        mstrm_pubBufferMax = mstrm_pubBuffer + mstrm_slLocation;
    }
}

/* Seek in stream. */
void MemoryStream::Seek_t(SLONG slOffset, enum SeekDir sd)
{
    switch (sd) {
    case SD_BEG:
        mstrm_slLocation = slOffset;
        break;
    case SD_CUR:
        mstrm_slLocation += slOffset;
        break;
    case SD_END:
        mstrm_slLocation = GetStreamSize() + slOffset;
        break;
    }
}

/* Set absolute position in stream. */
void MemoryStream::SetPos_t(SLONG slPosition)
{
    mstrm_slLocation = slPosition;
}

/* Get absolute position in stream. */
SLONG MemoryStream::GetPos_t(void)
{
    return mstrm_slLocation;
}

/* Get size of stream. */
SLONG MemoryStream::GetSize_t(void)
{
    return GetStreamSize();
}

/* Get size of stream */
SLONG MemoryStream::GetStreamSize(void)
{
    return mstrm_pubBufferMax - mstrm_pubBuffer;
}

/* Get CRC32 of stream */
ULONG MemoryStream::GetStreamCRC32_t(void)
{
    return Stream::GetStreamCRC32_t();
}

/* Check if file position points to the EOF */
BOOL MemoryStream::AtEOF(void)
{
    return mstrm_slLocation >= GetStreamSize();
}

// whether or not the given pointer is coming from this stream (mainly used for exception handling)
BOOL MemoryStream::PointerInStream(void* pPointer)
{
    return pPointer >= mstrm_pubBuffer && pPointer < mstrm_pubBufferEnd;
}
/*
// Test if a file exists.
BOOL FileExists(const CTFileName& fnmFile)
{
    // if no file
    if (fnmFile == "") {
        // it doesn't exist
        return FALSE;
    }
    // try to
    try {
        // open the file for reading
        FileStream strmFile;
        strmFile.Open_t(fnmFile);
        // if successful, it means that it exists,
        return TRUE;
        // if failed, it means that it doesn't exist
    } catch (char* strError) {
        (void)strError;
        return FALSE;
    }
}

// Test if a file exists for writing.
// (this is can be diferent than normal FileExists() if a mod uses basewriteexclude.lst
BOOL FileExistsForWriting(const CTFileName& fnmFile)
{
    // if no file
    if (fnmFile == "") {
        // it doesn't exist
        return FALSE;
    }
    // expand the filename to full path for writing
    CTFileName fnmFullFileName;
    /* INDEX iFile = */
/* ExpandFilePath(EFP_WRITE, fnmFile, fnmFullFileName);

    // check if it exists
    FILE* f = fopen(fnmFullFileName, "rb");
    if (f != NULL) {
        fclose(f);
        return TRUE;
    } else {
        return FALSE;
    }
}

// Get file timestamp
SLONG GetFileTimeStamp_t(const CTFileName& fnm)
{
    // expand the filename to full path
    CTFileName fnmExpanded;
    INDEX iFile = ExpandFilePath(EFP_READ, fnm, fnmExpanded);
    if (iFile != EFP_FILE) {
        return FALSE;
    }

    int file_handle;
    // try to open file for reading
    file_handle = _open(fnmExpanded, _O_RDONLY | _O_BINARY);
    if (file_handle == -1) {
        ThrowF_t(TRANS("Cannot open file '%s' for reading"), (const char*)CTString(fnm));
        return -1;
    }
    struct stat statFileStatus;
    // get file status
    fstat(file_handle, &statFileStatus);
    _close(file_handle);
    ASSERT(statFileStatus.st_mtime <= time(NULL));
    return statFileStatus.st_mtime;
}

// Get CRC32 of a file
ULONG GetFileCRC32_t(const CTFileName& fnmFile) // throw char *
{
    // open the file
    FileStream fstrm;
    fstrm.Open_t(fnmFile);
    // return the checksum
    return fstrm.GetStreamCRC32_t();
}

// Test if a file is read only (also returns FALSE if file does not exist)
BOOL IsFileReadOnly(const CTFileName& fnm)
{
    // expand the filename to full path
    CTFileName fnmExpanded;
    INDEX iFile = ExpandFilePath(EFP_READ, fnm, fnmExpanded);
    if (iFile != EFP_FILE) {
        return FALSE;
    }

    int file_handle;
    // try to open file for reading
    file_handle = _open(fnmExpanded, _O_RDONLY | _O_BINARY);
    if (file_handle == -1) {
        return FALSE;
    }
    struct stat statFileStatus;
    // get file status
    fstat(file_handle, &statFileStatus);
    _close(file_handle);
    ASSERT(statFileStatus.st_mtime <= time(NULL));
    return !(statFileStatus.st_mode & _S_IWRITE);
}

// Delete a file
BOOL RemoveFile(const CTFileName& fnmFile)
{
    // expand the filename to full path
    CTFileName fnmExpanded;
    INDEX iFile = ExpandFilePath(EFP_WRITE, fnmFile, fnmExpanded);
    if (iFile == EFP_FILE) {
        int ires = remove(fnmExpanded);
        return ires == 0;
    } else {
        return FALSE;
    }
}

static BOOL IsFileReadable_internal(CTFileName& fnmFullFileName)
{
    FILE* pFile = fopen(fnmFullFileName, "rb");
    if (pFile != NULL) {
        fclose(pFile);
        return TRUE;
    } else {
        return FALSE;
    }
}

// check for some file extensions that can be substituted
static BOOL SubstExt_internal(CTFileName& fnmFullFileName)
{
    if (fnmFullFileName.FileExt() == ".mp3") {
        fnmFullFileName = fnmFullFileName.NoExt() + ".ogg";
        return TRUE;
    } else if (fnmFullFileName.FileExt() == ".ogg") {
        fnmFullFileName = fnmFullFileName.NoExt() + ".mp3";
        return TRUE;
    } else {
        return TRUE;
    }
}

static INDEX ExpandFilePath_read(ULONG ulType, const CTFileName& fnmFile, CTFileName& fnmExpanded)
{
    // search for the file in zips
    INDEX iFileInZip = UNZIPGetFileIndex(fnmFile);
    //const BOOL userdir_not_basedir = (g_user_dir != g_app_path);

    // if a mod is active
    if (g_app_mod_path != "") {

        // first try in the mod's dir
        if (!g_fs_prefer_zip) {
            fnmExpanded = g_app_path + g_app_mod_path + fnmFile;
            if (IsFileReadable_internal(fnmExpanded)) {
                return EFP_FILE;
            }
        }

        // if not disallowing zips
        if (!(ulType & EFP_NOZIPS)) {
            // if exists in mod's zip
            if (iFileInZip >= 0 && UNZIPIsFileAtIndexMod(iFileInZip)) {
                // use that one
                fnmExpanded = fnmFile;
                return EFP_MODZIP;
            }
        }

        // try in the mod's dir after
        if (g_fs_prefer_zip) {
            fnmExpanded = g_app_path + g_app_mod_path + fnmFile;
            if (IsFileReadable_internal(fnmExpanded)) {
                return EFP_FILE;
            }
        }
    }

    // try in the app's base dir
    if (!g_fs_prefer_zip) {
        CTFileName fnmAppPath = g_app_path;
        fnmAppPath.SetAbsolutePath();

        if (fnmFile.HasPrefix(fnmAppPath)) {
            fnmExpanded = fnmFile;
        } else {
            fnmExpanded = g_app_path + fnmFile;
        }

        if (IsFileReadable_internal(fnmExpanded)) {
            return EFP_FILE;
        }
    }

    // if not disallowing zips
    if (!(ulType & EFP_NOZIPS)) {
        // if exists in any zip
        if (iFileInZip >= 0) {
            // use that one
            fnmExpanded = fnmFile;
            return EFP_BASEZIP;
        }
    }

    // try in the app's base dir
    if (g_fs_prefer_zip) {
        fnmExpanded = g_app_path + fnmFile;
        if (IsFileReadable_internal(fnmExpanded)) {
            return EFP_FILE;
        }
    }

    // finally, try in the CD path
    if (g_app_cd_path != "") {

        // if a mod is active
        if (g_app_mod_path != "") {
            // first try in the mod's dir
            fnmExpanded = g_app_cd_path + g_app_mod_path + fnmFile;
            if (IsFileReadable_internal(fnmExpanded)) {
                return EFP_FILE;
            }
        }

        fnmExpanded = g_app_cd_path + fnmFile;
        if (IsFileReadable_internal(fnmExpanded)) {
            return EFP_FILE;
        }
    }
    return EFP_NONE;
}

// rcg01042002 User dir and children may need to be created on the fly...
static void VerifyDirsExist(const char* _path)
{
    char* path = (char*)AllocMemory(strlen(_path) + 1);
    strcpy(path, _path);
    const char* dirsep = CFileSystem::GetDirSeparator();

    // skip first dirsep. This assumes an absolute path and some other
    //  fundamentals of how a filepath is specified.
    char* ptr = strstr(path, dirsep);
    ASSERT(ptr != NULL);
    if (ptr == NULL)
        return;

    for (ptr = strstr(ptr + 1, dirsep); ptr != NULL; ptr = strstr(ptr + 1, dirsep)) {
        char ch = *ptr;
        *ptr = '\0'; // terminate the path.
        if (!_pFileSystem->IsDirectory(path)) {
            if (_pFileSystem->Exists(path)) {
                CPrintF("Expected %s to be a directory, but it's a file!\n", path);
                break;
            } else {
                CPrintF("Creating directory %s ...\n", path);
                _mkdir(path);
                if (!_pFileSystem->IsDirectory(path)) {
                    CPrintF("Creation of directory %s FAILED!\n", path);
                    break;
                }
            }
        }

        *ptr = ch; // put path char back...
    }

    FreeMemory(path);
}

// Expand a file's filename to full path
INDEX ExpandFilePath(ULONG ulType, const CTFileName& fnmFile, CTFileName& fnmExpanded)
{
    CTFileName fnmFileAbsolute = fnmFile;
    fnmFileAbsolute.SetAbsolutePath();

    // if writing
    if (ulType & EFP_WRITE) {
        // if should write to mod dir
        if (g_app_mod_path != "" && (!FileMatchesList(g_fs_base_write_include, fnmFileAbsolute) || FileMatchesList(g_fs_base_write_exclude, fnmFileAbsolute))) {
            // do that
            fnmExpanded = g_app_path + g_app_mod_path + fnmFileAbsolute;
            fnmExpanded.SetAbsolutePath();
            VerifyDirsExist(fnmExpanded.FileDir());
            return EFP_FILE;
            // if should not write to mod dir
        } else {
            // write to base dir
            fnmExpanded = g_app_path + fnmFileAbsolute;
            fnmExpanded.SetAbsolutePath();
            VerifyDirsExist(fnmExpanded.FileDir());
            return EFP_FILE;
        }

        // if reading
    } else if (ulType & EFP_READ) {

        // check for expansions for reading
        INDEX iRes = ExpandFilePath_read(ulType, fnmFileAbsolute, fnmExpanded);
        // if not found
        if (iRes == EFP_NONE) {
            //check for some file extensions that can be substituted
            CTFileName fnmSubst = fnmFileAbsolute;
            if (SubstExt_internal(fnmSubst)) {
                iRes = ExpandFilePath_read(ulType, fnmSubst, fnmExpanded);
            }
        }

        fnmExpanded.SetAbsolutePath();

        if (iRes != EFP_NONE) {
            return iRes;
        }

        // in other cases
    } else {
        ASSERT(FALSE);
        fnmExpanded = g_app_path + fnmFileAbsolute;
        fnmExpanded.SetAbsolutePath();
        return EFP_FILE;
    }

    fnmExpanded = g_app_path + fnmFileAbsolute;
    fnmExpanded.SetAbsolutePath();
    return EFP_NONE;
}
*/