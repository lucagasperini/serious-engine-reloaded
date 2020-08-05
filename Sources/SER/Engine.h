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

#include "Base/FileSystem.h"
#include <Engine/Base/CTString.h>
#include <Engine/Base/FileName.h>

namespace SER {

class Engine {
private:
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

    // OS info
    static CTString sys_OS;
    static INDEX sys_OSMajor;
    static INDEX sys_OSMinor;
    static INDEX sys_OSBuild;
    static CTString sys_OSMisc;

    // CPU info
    static CTString sys_CPUVendor;
    static INDEX sys_CPUType;
    static INDEX sys_CPUFamily;
    static INDEX sys_CPUModel;
    static INDEX sys_CPUStepping;
    static BOOL sys_CPUHasMMX;
    static BOOL sys_CPUHasCMOV;
    static INDEX sys_CPUMHz;
    static INDEX sys_CPUMisc;

    // RAM info
    static INDEX sys_RAMPhys;
    static INDEX sys_RAMSwap;

    // HDD info
    static INDEX sys_HDDSize;
    static INDEX sys_HDDFree;
    static INDEX sys_HDDMisc;

    // MOD info
    static CTString sys_ModName;
    static CTString sys_ModExt;

    static FileSystem* file_system;

    static void AnalyzeApplicationPath();
    static void DetectCPU();
    static void PlatformIdentification();
    static BOOL FileMatchesList(CDynamicStackArray<CTFileName>& afnm, const CTFileName& fnm);
    static void InitStreams();
    static void EndStreams();

public:
    static void init(const char* argv0, CTString strGameID);

    static inline void setModPath(const CTFileName& _mod_path) { mod_path = _mod_path; }
    static inline void setModName(const CTString& _mod_name) { mod_name = _mod_name; }
    static inline void setModUrl(const CTString& _mod_url) { mod_url = _mod_url; }
    static inline void setModExt(const CTString& _mod_ext) { mod_ext = _mod_ext; }
    static inline void setCdPath(const CTFileName& _cd_path) { cd_path = _cd_path; }

    static inline CTFileName getModPath() { return mod_path; }
    static inline CTString getModName() { return mod_name; }
    static inline CTString getModUrl() { return mod_url; }
    static inline CTString getModExt() { return mod_ext; }
    static inline CTFileName getCdPath() { return cd_path; }

    static inline FileSystem* getFileSystem() { return file_system; }
};
}