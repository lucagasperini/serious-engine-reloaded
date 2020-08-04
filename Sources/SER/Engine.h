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

#include <Engine/Base/CTString.h>
#include <Engine/Base/FileName.h>

namespace SER {

class Engine {
private:
    // global string with application path
    static CTFileName app_path;
    // global string with filename of the started application
    static CTFileName app_exe;
    // global string with user-specific writable directory.
    static CTFileName user_dir;
    // global string with current MOD path
    static CTFileName mod_path;
    // global string with current name (the parameter that is passed on cmdline)
    static CTString mod_name;
    // global string with url to be shown to users that don't have the mod installed
    // (should be set by game.dll)
    static CTString mod_url;
    // global string with current MOD extension (for adding to dlls)
    static CTString mod_ext;
    // global string with CD path (for minimal installations)
    static CTFileName cd_path;

public:
    static void init(const char* argv0, CTString strGameID);

    static inline void setAppPath(const CTFileName& _app_path) { app_path = _app_path; }
    static inline void setAppExe(const CTFileName& _app_exe) { app_exe = _app_exe; }
    static inline void setUserDir(const CTFileName& _user_dir) { user_dir = _user_dir; }
    static inline void setModPath(const CTFileName& _mod_path) { mod_path = _mod_path; }
    static inline void setModName(const CTString& _mod_name) { mod_name = _mod_name; }
    static inline void setModUrl(const CTString& _mod_url) { mod_url = _mod_url; }
    static inline void setModExt(const CTString& _mod_ext) { mod_ext = _mod_ext; }
    static inline void setCdPath(const CTFileName& _cd_path) { cd_path = _cd_path; }

    static inline CTFileName getAppPath() { return app_path; }
    static inline CTFileName getAppExe() { return app_exe; }
    static inline CTFileName getUserDir() { return user_dir; }
    static inline CTFileName getModPath() { return mod_path; }
    static inline CTString getModName() { return mod_name; }
    static inline CTString getModUrl() { return mod_url; }
    static inline CTString getModExt() { return mod_ext; }
    static inline CTFileName getCdPath() { return cd_path; }
};
}