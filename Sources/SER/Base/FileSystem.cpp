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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <dirent.h>
#include <pwd.h>
#endif

#ifdef _WIN32
#include <io.h>
#endif

#include <Engine/Engine.h>

#include "SDL.h"

#include "FileSystem.h"

using namespace SER;

// global string with application path
CTFileName FileSystem::app_path;
// global string with filename of the started application
CTFileName FileSystem::app_exe;
// global string with user-specific writable directory.
CTFileName FileSystem::user_dir;

FileSystem::FileSystem(const CTString& argv0, const CTString& gamename)
{
    app_exe = SDL_GetBasePath();
    user_dir = SDL_GetPrefPath("Serious Engine", gamename);
}

//TODO: If we use SDL in windows this is useless?
#ifdef _WIN32
FileSystem::FileSystem(const CTString& argv0, const CTString& gamename)
{
    char buf[MAX_PATH];
    memset(buf, '\0', sizeof(buf));
    GetModuleFileName(NULL, buf, sizeof(buf) - 1);

    char* exePath = new char[strlen(buf) + 1];
    strcpy(exePath, buf);
    app_exe = exePath;

    char* userDir = new char[strlen(buf) + 1];
    strcpy(userDir, buf);
    user_dir = userDir;
    ASSERTALWAYS("We need to chop \\bin\\debug off the string if it's there.\n");
}
#endif

FileSystem::~FileSystem()
{
}

#ifdef __linux__
CTString FileSystem::getDirSeparator()
{
    return ("/");
}
#endif
#ifdef _WIN32
CTString FileSystem::getDirSeparator(void)
{
    return ("\\");
}
#endif

BOOL FileSystem::isDummyFile(const CTString& fname)
{
    return ((strcmp(fname, ".") == 0) || (strcmp(fname, "..") == 0));
}

#ifdef __linux__
BOOL FileSystem::exists(const CTString& fname)
{
    struct stat s;
    if (stat(fname, &s) == -1)
        return (FALSE);

    return (TRUE);
}
#endif
#ifdef _WIN32
BOOL FileSystem::exists(const char* fname)
{
    ASSERTALWAYS("Write me!");
}
#endif

#ifdef __linux__
BOOL FileSystem::isDirectory(const CTString& fname)
{
    struct stat s;
    if (stat(fname, &s) == -1)
        return (FALSE);

    return (S_ISDIR(s.st_mode) ? TRUE : FALSE);
}
#endif

#ifdef _WIN32
BOOL CFileSystem::isDirectory(const char* fname)
{
    ASSERTALWAYS("Write me!");
}
#endif

//TODO: Why an snprintf instead return a string???
void FileSystem::GetExecutablePath(char* buf, ULONG bufSize)
{
    SDL_snprintf(buf, bufSize, "%s", app_exe);
}

//TODO: Why an snprintf instead return a string???
void FileSystem::GetUserDirectory(char* buf, ULONG bufSize)
{
    SDL_snprintf(buf, bufSize, "%s", user_dir);
}

#ifdef __linux__
CDynamicArray<CTString>* FileSystem::findFiles(const char* dir, const char* wildcard)
{
    CDynamicArray<CTString>* retval = new CDynamicArray<CTString>;
    DIR* d = opendir(dir);

    if (d != NULL) {
        struct dirent* dent;
        while ((dent = readdir(d)) != NULL) {
            CTString str(dent->d_name);
            if (str.Matches(wildcard))
                *retval->New() = str;
        }
        closedir(d);
    }
    return (retval);
}
#endif

#ifdef _WIN32
CDynamicArray<CTString>* FileSystem::findFiles(const char* dir, const char* wildcard)
{
    CDynamicArray<CTString>* retval = new CDynamicArray<CTString>;

    CTString str(dir);
    if (dir[strlen(dir) - 1] != '\\')
        str += "\\";

    struct _finddata_t c_file;
    long hFile = _findfirst((const char*)(str + wildcard), &c_file);

    for (BOOL bFileExists = hFile != -1;
         bFileExists;
         bFileExists = _findnext(hFile, &c_file) == 0) {
        *retval->New() = c_file.name;
    }

    _findclose(hFile);
    return (retval);
}
#endif
// end of UnixFileSystem.cpp ...
