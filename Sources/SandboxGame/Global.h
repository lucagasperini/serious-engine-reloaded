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

#ifndef SER_SBGAME_GLOBAL_H
#define SER_SBGAME_GLOBAL_H

#include "Define.h"

#include <ECS/Manager.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/FileName.h>
#include <Engine/Base/Types.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Graphics/ViewPort.h>
#include <Engine/World/World.h>

CTFileName g_world_file = CTFILENAME("Levels\\TestGame.wld");
CWorld* g_world_data = NULL;
CTString g_gamename = "serioussammf";

CDrawPort* g_drawport = NULL;
CViewPort* g_viewport = NULL;

CTFileName g_logfile = CTFILENAME("fast.log");
CTFileStream g_logstream;

COLOR g_fb_color = C_BLACK | 0xff;

BOOL g_dbg_draw_border = FALSE;
BOOL g_dbg_draw_id = FALSE;
BOOL g_dbg_draw_position = FALSE;
BOOL g_dbg_draw_fps = FALSE;
BOOL g_dbg_draw_cursor = FALSE;

UINT g_vresolution_width = 1920;
UINT g_vresolution_height = 1080;

BOOL g_game_started = FALSE;

#endif