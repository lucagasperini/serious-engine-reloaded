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
#include <ECS/Manager.h>
#include <Engine/Graphics/Font.h>
#include <SDL.h>

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

using namespace SER::ECS;

void quitgame()
{
    //g_game_started = FALSE;
}

void load_all_game_system()
{
    RenderSystem* render_system = new RenderSystem;
    Manager::setRenderSystem((System*)render_system);

    keybind* a_keybind = new keybind[SER_KEYBIND_MAX];
    memset(a_keybind, 0, sizeof(keybind) * SER_KEYBIND_MAX);
    a_keybind[0].event.code = SER_EVENT_FULLSCREEN_CHANGE;
    a_keybind[0].key = SDLK_F1;
    a_keybind[0].event.parameter = NULL;
    UINT* p_res_vga = new UINT[2] { 640, 480 };
    a_keybind[1].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[1].event.parameter = p_res_vga;
    a_keybind[1].key = SDLK_F2;
    UINT* p_res_svga = new UINT[2] { 800, 600 };
    a_keybind[2].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[2].event.parameter = p_res_svga;
    a_keybind[2].key = SDLK_F3;
    UINT* p_res_wxga = new UINT[2] { 1280, 720 };
    a_keybind[3].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[3].event.parameter = p_res_wxga;
    a_keybind[3].key = SDLK_F4;
    UINT* p_res_hd = new UINT[2] { 1920, 1080 };
    a_keybind[4].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[4].event.parameter = p_res_hd;
    a_keybind[4].key = SDLK_F5;

    EventSystem* event_system = new EventSystem;
    event_system->a_keybind = a_keybind;
    Manager::setEventSystem((System*)event_system);

    PositionSystem* position_system = new PositionSystem;
    Manager::addSystem((System*)position_system);
}

void load_all_game_entity()
{
    CFontData font_small;
    CFontData font_medium;
    CFontData font_big;

    FLOAT3D world_start_position = FLOAT3D(9, 6, 32);
    ANGLE3D world_start_rotation = ANGLE3D(0, 20, 0);

    font_small.Load_t(CTFILENAME("Fonts\\Display3-narrow.fnt"));
    font_medium.Load_t(CTFILENAME("Fonts\\Display3-normal.fnt"));
    font_big.Load_t(CTFILENAME("Fonts\\Display3-caps.fnt"));

    main_window* e_window = new main_window;
    e_window->win_title = "Serious Engine Sandbox Game";
    e_window->win_api = GfxAPIType::GAT_OGL;
    e_window->win_adapter = 0;
    e_window->win_depth = DisplayDepth::DD_32BIT;
    e_window->win_flags = SE_MAINWINDOW_FLAGS_NULL;
    SER_ADD_ENTITY(e_window, main_window);

    /*
    game_control->kb_keybind[SE_KEYBIND_EXIT] = SDLK_ESCAPE;
    game_control->kb_keybind[SE_KEYBIND_FULLSCREEN] = SDLK_F1;

    game_control->kb_keybind[SE_KEYBIND_DEBUG_BORDER] = SDLK_F6;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_ENTITYID] = SDLK_F7;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_POSITION] = SDLK_F8;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_FPS] = SDLK_F9;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_CURSOR] = SDLK_F10;
    */

    camera* e_camera = new camera();
    e_camera->cam_fov = 90.0f;
    e_camera->cam_pos = world_start_position;
    e_camera->cam_rot = world_start_rotation;
    e_camera->cam_speed = 1.0f; /*
    memset(camera->kb_keybind, 0, sizeof(ULONG) * SE_ECS_KEYBIND_MAX);
    camera->kb_keybind[SE_KEYBIND_CAMERA_RESET] = SDLK_F12;
    camera->kb_keybind[SE_KEYBIND_CAMERA_RIGHT] = SDLK_RIGHT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_LEFT] = SDLK_LEFT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_FORWARD] = SDLK_UP;
    camera->kb_keybind[SE_KEYBIND_CAMERA_BACK] = SDLK_DOWN;
    camera->kb_keybind[SE_KEYBIND_CAMERA_UP] = SDLK_SPACE;
    camera->kb_keybind[SE_KEYBIND_CAMERA_DOWN] = SDLK_c;*/
    SER_ADD_ENTITY(e_camera, camera);

    cursor* e_cursor = new cursor;
    e_cursor->texture.SetData_t(CTFILENAME("TexturesMP\\General\\Pointer.tex"));
    e_cursor->w = 32;
    e_cursor->h = 32;
    SER_ADD_ENTITY(e_cursor, cursor);

    menu_image* logosam = new menu_image();
    logosam->pos_x = 480;
    logosam->pos_y = 10;
    logosam->pos_w = 1024;
    logosam->pos_h = 256;
    logosam->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\logo.tex"));
    SER_ADD_ENTITY(logosam, menu_image);

    menu_image* logoct = new menu_image();
    logoct->pos_x = 16;
    logoct->pos_y = 864;
    logoct->pos_w = 200;
    logoct->pos_h = 200;
    logoct->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\LogoCT.tex"));
    SER_ADD_ENTITY(logoct, menu_image);

    menu_image* logose = new menu_image();
    logose->pos_x = 1704;
    logose->pos_y = 864;
    logose->pos_w = 200;
    logose->pos_h = 200;
    logose->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
    SER_ADD_ENTITY(logose, menu_image);

    menu_button* menu_button_sp = new menu_button;
    menu_button_sp->pos_y = 300;
    menu_button_sp->pos_w = 300;
    menu_button_sp->pos_h = 50;
    menu_button_sp->align = 0;
    menu_button_sp->align_x = 0;
    menu_button_sp->fontdata = font_big;
    menu_button_sp->fontsize = 2;
    menu_button_sp->text = TRANS("SINGLE PLAYER");
    menu_button_sp->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_sp->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_sp, menu_button);

    menu_button* menu_button_net = new menu_button;
    menu_button_net->pos_y = 375;
    menu_button_net->pos_w = 300;
    menu_button_net->pos_h = 50;
    menu_button_net->align = 0;
    menu_button_net->align_x = 0;
    menu_button_net->fontdata = font_big;
    menu_button_net->fontsize = 2;
    menu_button_net->text = TRANS("NETWORK");
    menu_button_net->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_net->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_net, menu_button);

    menu_button* menu_button_split = new menu_button;
    menu_button_split->pos_y = 450;
    menu_button_split->pos_w = 300;
    menu_button_split->pos_h = 50;
    menu_button_split->align = 0;
    menu_button_split->align_x = 0;
    menu_button_split->fontdata = font_big;
    menu_button_split->fontsize = 2;
    menu_button_split->text = TRANS("SPLIT SCREEN");
    menu_button_split->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_split->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_split, menu_button);

    menu_button* menu_button_demo = new menu_button;
    menu_button_demo->pos_y = 525;
    menu_button_demo->pos_w = 300;
    menu_button_demo->pos_h = 50;
    menu_button_demo->align = 0;
    menu_button_demo->align_x = 0;
    menu_button_demo->fontdata = font_big;
    menu_button_demo->fontsize = 2;
    menu_button_demo->text = TRANS("DEMO");
    menu_button_demo->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_demo->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_demo, menu_button);

    menu_button* menu_button_mod = new menu_button;
    menu_button_mod->pos_y = 600;
    menu_button_mod->pos_w = 300;
    menu_button_mod->pos_h = 50;
    menu_button_mod->align = 0;
    menu_button_mod->align_x = 0;
    menu_button_mod->fontdata = font_big;
    menu_button_mod->fontsize = 2;
    menu_button_mod->text = TRANS("MODS");
    menu_button_mod->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_mod->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_mod, menu_button);

    menu_button* menu_button_hs = new menu_button;
    menu_button_hs->pos_y = 675;
    menu_button_hs->pos_w = 300;
    menu_button_hs->pos_h = 50;
    menu_button_hs->align = 0;
    menu_button_hs->align_x = 0;
    menu_button_hs->fontdata = font_big;
    menu_button_hs->fontsize = 2;
    menu_button_hs->text = TRANS("HIGH SCORES");
    menu_button_hs->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_hs->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_hs, menu_button);

    menu_button* menu_button_opt = new menu_button;
    menu_button_opt->pos_y = 750;
    menu_button_opt->pos_w = 300;
    menu_button_opt->pos_h = 50;
    menu_button_opt->align = 0;
    menu_button_opt->align_x = 0;
    menu_button_opt->fontdata = font_big;
    menu_button_opt->fontsize = 2;
    menu_button_opt->text = TRANS("OPTIONS");
    menu_button_opt->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_opt->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_opt, menu_button);

    menu_button* menu_button_quit = new menu_button;
    menu_button_quit->pos_y = 825;
    menu_button_quit->pos_w = 300;
    menu_button_quit->pos_h = 50;
    menu_button_quit->align = 0;
    menu_button_quit->align_x = 0;
    menu_button_quit->fontdata = font_big;
    menu_button_quit->fontsize = 2;
    menu_button_quit->text = TRANS("QUIT");
    //menu_button_quit->sea_action = quitgame;
    menu_button_quit->color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_quit->color_focus = SE_COL_ORANGE_DARK | 255;
    SER_ADD_ENTITY(menu_button_quit, menu_button);
}