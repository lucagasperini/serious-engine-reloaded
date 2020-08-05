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

/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SER_FILESYSTEM_H
#define SER_FILESYSTEM_H

#include <Engine/Engine.h>

// !!! FIXME: rcg10142001 This should really be using CTStrings...

namespace SER {
/*
 * This class handles filesystem differences between platforms.
 */
class FileSystem {
private:
    // global string with application path
    static CTFileName app_path;
    // global string with filename of the started application
    static CTFileName app_exe;
    // global string with user-specific writable directory.
    static CTFileName user_dir;

public:
    // Construct a system-dependent version of CFileSystem.
    // (argv0) is argv[0] in your mainline.
    // gamename is a simple token that identifies your game.
    FileSystem(const CTString& argv0, const CTString& gamename);

    ~FileSystem();

    // Get the platform-specific directory separator. This could be
    //  "\\" on win32, "/" on unix, and ":" on MacOS Classic.
    //  Consider the returned pointer to be READ ONLY, as it points to a
    //  static, internal literal string on most platforms.
    //  Some platforms may define a dir separator that is MORE THAN ONE
    //  CHARACTER LONG. You have been warned.
    static CTString getDirSeparator();

    // Returns TRUE if (fname) is not a real file ("." and ".." on win32, etc.)
    //  THIS DOES NOT CHECK IF A SPECIFIC FILE EXISTS IN THE FILESYSTEM!
    static BOOL isDummyFile(const CTString& fname);

    // Returns TRUE if (fname) exists at all. May be a symlink, dir, file, etc.
    static BOOL exists(const CTString& fname);

    // Returns TRUE if (fname) is a directory.
    static BOOL isDirectory(const CTString& fname);

    // Get the path of the binary (.EXE file) being run.
    //  (buf) is where to store the info, and (bufSize) is the size, in bytes,
    //  of what's pointed to by (buf). The buffer is always promised to be
    //  null-terminated.
    void GetExecutablePath(char* buf, ULONG bufSize);

    // Get the user directory. This is the user's home directory on systems
    //  with that concept, and the base (buf) is where to store the info, and
    //  (bufSize) is the size, in bytes, of what's pointed to by (buf). The
    //  buffer is always promised to be null-terminated, and, if there's room,
    //  with have a trailing dir separator. It is likely that you will have
    //  write permission in the user directory tree, and will NOT have write
    //  permission in the base directory. You have been warned.
    void GetUserDirectory(char* buf, ULONG bufSize);

    // Get an array of CTStrings containing the names of files in (dir) that
    //  match (wildcard).
    CDynamicArray<CTString>* findFiles(const char* dir, const char* wildcard);

    static inline void setAppPath(const CTFileName& _app_path) { app_path = _app_path; }
    static inline void setAppExe(const CTFileName& _app_exe) { app_exe = _app_exe; }
    static inline void setUserDir(const CTFileName& _user_dir) { user_dir = _user_dir; }

    static inline CTFileName getAppPath() { return app_path; }
    static inline CTFileName getAppExe() { return app_exe; }
    static inline CTFileName getUserDir() { return user_dir; }
};

}
#endif