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

#include "Entity.h"
#include "EventSystem.h"
#include "PositionSystem.h"
#include "RenderSystem.h"
#include <SER/ECS/Manager.h>
#include <Engine/Graphics/Font.h>
#include <SDL.h>

#include <Engine/Templates/Stock_CTextureData.h>

#define SE_COL_ORANGE_LIGHT 0xffd70000
#define SE_COL_ORANGE_NEUTRAL 0xee9c0000
#define SE_COL_ORANGE_DARK 0x9b4b0000
#define SE_COL_ORANGE_DARK_LT 0xbc6a0000
#define SE_COL_BLUE_DARK_HV 0x151c2300
#define SE_COL_BLUE_DARK 0x2a384600
#define SE_COL_BLUE_DARK_LT 0x43596f00
#define SE_COL_BLUE_NEUTRAL 0x5c7a9900
#define SE_COL_BLUE_NEUTRAL_LT 0x6097cc00
#define SE_COL_BLUE_LIGHT 0x64b4ff00
#define SE_COL_BLUEGREEN_LT 0x6cff6c00

#define SER_WINDOW_FALLBACK_COUNT 3
#define SE_MAINWINDOW_FLAGS_NULL 0x0
#define SE_WINDOW_RECOVERY_W 640
#define SE_WINDOW_RECOVERY_H 480

#define KEYBIND_ADD_NOARG(_key, _event) \
    event_system->addKeybind(EventKeybind { _event, _key, new BYTE(0), sizeof(BYTE) })

#define KEYBIND_ADD(_key, _event, _arg, _type) \
    event_system->addKeybind(EventKeybind { _event, _key, _arg, sizeof(_type) })

#define KEYBIND_ADD_ARRAY(_key, _event, _arg, _type, _number) \
    event_system->addKeybind(EventKeybind { _event, _key, _arg, sizeof(_type) * _number })

using namespace SER;

extern UINT g_list_resolution[][2];

void load_all_game_setting()
{
    SER_ADD_SETTING(SC_DEBUG_BORDER, FALSE, BOOL);
    SER_ADD_SETTING(SC_DEBUG_POSITION, FALSE, BOOL);
    SER_ADD_SETTING(SC_DEBUG_ENTITYID, FALSE, BOOL);
    SER_ADD_SETTING(SC_DEBUG_FPS, FALSE, BOOL);
    SER_ADD_SETTING(SC_DEBUG_CURSOR, FALSE, BOOL);
    SER_ADD_SETTING(SC_DEBUG_GRABMOUSE, FALSE, BOOL);
    SER_ADD_SETTING_ARRAY(SC_RESOLUTION, g_list_resolution[2], UINT, 2);
    SER_ADD_SETTING_ARRAY(SC_VIRTUAL_RESOLUTION, g_list_resolution[3], UINT, 2);
    SER_ADD_SETTING(SC_FULLSCREEN, FALSE, BOOL);
}

void load_all_game_system()
{
    RenderSystem* render_system = new RenderSystem;
    Manager::setRenderSystem((System*)render_system);

    EventSystem* event_system = new EventSystem;
    KEYBIND_ADD_NOARG(SDLK_F1, EC_FULLSCREEN_CHANGE);
    KEYBIND_ADD_ARRAY(SDLK_F2, EC_RESOLUTION_CHANGE, g_list_resolution[0], UINT, 2);
    KEYBIND_ADD_ARRAY(SDLK_F3, EC_RESOLUTION_CHANGE, g_list_resolution[1], UINT, 2);
    KEYBIND_ADD_ARRAY(SDLK_F4, EC_RESOLUTION_CHANGE, g_list_resolution[2], UINT, 2);
    KEYBIND_ADD_ARRAY(SDLK_F5, EC_RESOLUTION_CHANGE, g_list_resolution[3], UINT, 2);
    KEYBIND_ADD_NOARG(SDLK_a, EC_CAMERA_RIGHT);
    KEYBIND_ADD_NOARG(SDLK_d, EC_CAMERA_LEFT);
    KEYBIND_ADD_NOARG(SDLK_w, EC_CAMERA_FORWARD);
    KEYBIND_ADD_NOARG(SDLK_s, EC_CAMERA_BACK);
    KEYBIND_ADD_NOARG(SDLK_F6, EC_DEBUG_BORDER);
    KEYBIND_ADD_NOARG(SDLK_F7, EC_DEBUG_ENTITYID);
    KEYBIND_ADD_NOARG(SDLK_F8, EC_DEBUG_POSITION);
    KEYBIND_ADD_NOARG(SDLK_F9, EC_DEBUG_FPS);
    KEYBIND_ADD_NOARG(SDLK_F10, EC_DEBUG_CURSOR);
    KEYBIND_ADD_NOARG(SDLK_F11, EC_DEBUG_GRABMOUSE);
    KEYBIND_ADD_NOARG(SDLK_ESCAPE, EC_EXIT);

    Manager::setEventSystem((System*)event_system);

    PositionSystem* position_system = new PositionSystem;
    Manager::addSystem((System*)position_system);
}

void load_all_game_entity()
{

    FLOAT3D world_start_position = FLOAT3D(9, 6, 32);
    ANGLE3D world_start_rotation = ANGLE3D(0, 20, 0);

    Camera* e_camera = new Camera();
    e_camera->cam_fov = 90.0f;
    e_camera->cam_pos = world_start_position;
    e_camera->cam_rot = world_start_rotation;
    e_camera->velocity = FLOAT3D(1.0f, 1.0f, 1.0f);
    e_camera->cam_speed = 0.20f;
    SER_ADD_ENTITY(e_camera, Camera);

    MenuImage* logosam = new MenuImage();
    logosam->pos_x = 480;
    logosam->pos_y = 10;
    logosam->pos_w = 1024;
    logosam->pos_h = 256;
    SER_ADD_ENTITY_FEM_STRING(logosam, MenuImage, "Textures\\Logo\\logo.tex", logosam->tex_file);

    MenuImage* logoct = new MenuImage();
    logoct->pos_x = 16;
    logoct->pos_y = 864;
    logoct->pos_w = 200;
    logoct->pos_h = 200;
    SER_ADD_ENTITY_FEM_STRING(logoct, MenuImage, "Textures\\Logo\\LogoCT.tex", logoct->tex_file);

    MenuImage* logose = new MenuImage();
    logose->pos_x = 1704;
    logose->pos_y = 864;
    logose->pos_w = 200;
    logose->pos_h = 200;
    SER_ADD_ENTITY_FEM_STRING(logose, MenuImage, "Textures\\Logo\\GodGamesLogo.tex", logose->tex_file);

    MenuButton* menu_button_sp = new MenuButton;
    menu_button_sp->pos_y = 300;
    menu_button_sp->pos_w = 300;
    menu_button_sp->pos_h = 50;
    menu_button_sp->align = 0;
    menu_button_sp->align_x = 0;
    //menu_button_sp->fontdata = font_big;
    menu_button_sp->fontsize = 2;
    menu_button_sp->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_sp->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_sp, MenuButton, TRANS("SINGLE PLAYER"), menu_button_sp->text);

    MenuButton* menu_button_net = new MenuButton;
    menu_button_net->pos_y = 375;
    menu_button_net->pos_w = 300;
    menu_button_net->pos_h = 50;
    menu_button_net->align = 0;
    menu_button_net->align_x = 0;
    //menu_button_net->fontdata = font_big;
    menu_button_net->fontsize = 2;
    menu_button_net->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_net->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_net, MenuButton, TRANS("NETWORK"), menu_button_net->text);

    MenuButton* menu_button_split = new MenuButton;
    menu_button_split->pos_y = 450;
    menu_button_split->pos_w = 300;
    menu_button_split->pos_h = 50;
    menu_button_split->align = 0;
    menu_button_split->align_x = 0;
    //menu_button_split->fontdata = font_big;
    menu_button_split->fontsize = 2;
    menu_button_split->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_split->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_split, MenuButton, TRANS("SPLIT SCREEN"), menu_button_split->text);

    MenuButton* menu_button_demo = new MenuButton;
    menu_button_demo->pos_y = 525;
    menu_button_demo->pos_w = 300;
    menu_button_demo->pos_h = 50;
    menu_button_demo->align = 0;
    menu_button_demo->align_x = 0;
    //menu_button_demo->fontdata = font_big;
    menu_button_demo->fontsize = 2;
    menu_button_demo->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_demo->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_demo, MenuButton, TRANS("DEMO"), menu_button_demo->text);

    MenuButton* menu_button_mod = new MenuButton;
    menu_button_mod->pos_y = 600;
    menu_button_mod->pos_w = 300;
    menu_button_mod->pos_h = 50;
    menu_button_mod->align = 0;
    menu_button_mod->align_x = 0;
    //menu_button_mod->fontdata = font_big;
    menu_button_mod->fontsize = 2;
    menu_button_mod->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_mod->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_mod, MenuButton, TRANS("MODS"), menu_button_mod->text);

    MenuButton* menu_button_hs = new MenuButton;
    menu_button_hs->pos_y = 675;
    menu_button_hs->pos_w = 300;
    menu_button_hs->pos_h = 50;
    menu_button_hs->align = 0;
    menu_button_hs->align_x = 0;
    //menu_button_hs->fontdata = font_big;
    menu_button_hs->fontsize = 2;
    menu_button_hs->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_hs->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_hs, MenuButton, TRANS("HIGH SCORES"), menu_button_hs->text);

    MenuButton* menu_button_opt = new MenuButton;
    menu_button_opt->pos_y = 750;
    menu_button_opt->pos_w = 300;
    menu_button_opt->pos_h = 50;
    menu_button_opt->align = 0;
    menu_button_opt->align_x = 0;
    //menu_button_opt->fontdata = font_big;
    menu_button_opt->fontsize = 2;
    menu_button_opt->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_opt->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY_FEM_STRING(menu_button_opt, MenuButton, TRANS("OPTIONS"), menu_button_opt->text);

    MenuButton* menu_button_quit = new MenuButton;
    menu_button_quit->pos_y = 825;
    menu_button_quit->pos_w = 300;
    menu_button_quit->pos_h = 50;
    menu_button_quit->align = 0;
    menu_button_quit->align_x = 0;
    //menu_button_quit->fontdata = font_big;
    menu_button_quit->fontsize = 2;
    menu_button_quit->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_quit->color_focus = SE_COL_ORANGE_DARK | 255;
    menu_button_quit->onclick = EC_EXIT;
    SER_ADD_ENTITY_FEM_STRING(menu_button_quit, MenuButton, TRANS("QUIT"), menu_button_quit->text);

    Cursor* e_cursor = new Cursor;
    e_cursor->w = 32;
    e_cursor->h = 32;
    SER_ADD_ENTITY_FEM_STRING(e_cursor, Cursor, "TexturesMP\\General\\Pointer.tex", e_cursor->texture);
}