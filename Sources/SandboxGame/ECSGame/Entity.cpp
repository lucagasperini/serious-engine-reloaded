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
#include "InputSystem.h"
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

keybind* a_keybind = new keybind[SER_KEYBIND_MAX];

void quitgame()
{
    //g_game_started = FALSE;
}

void load_keybind()
{
    memset(a_keybind, 0, sizeof(keybind) * SER_KEYBIND_MAX);
    a_keybind[0].event.code = SER_EVENT_FULLSCREEN_CHANGE;
    a_keybind[0].key = SDLK_F1;
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
}

void load_all_game_system()
{
    RenderSystem* render_system = new RenderSystem;
    ECSManager::setRenderSystem((SESystem*)render_system);
    EventSystem* event_system = new EventSystem;
    event_system->a_keybind = a_keybind;
    ECSManager::addSystem((SESystem*)event_system);

    PositionSystem* position_system = new PositionSystem;
    ECSManager::addSystem((SESystem*)position_system);
    InputSystem* input_system = new InputSystem;
    ECSManager::addSystem((SESystem*)input_system);
}

void load_all_game_entity()
{

    // Add space for 1 MB
    ECSManager::grow(1048576);

    CFontData font_small;
    CFontData font_medium;
    CFontData font_big;

    FLOAT3D world_start_position = FLOAT3D(9, 6, 32);
    ANGLE3D world_start_rotation = ANGLE3D(0, 20, 0);

    font_small.Load_t(CTFILENAME("Fonts\\Display3-narrow.fnt"));
    font_medium.Load_t(CTFILENAME("Fonts\\Display3-normal.fnt"));
    font_big.Load_t(CTFILENAME("Fonts\\Display3-caps.fnt"));

    main_window* entity_window = new main_window;
    entity_window->win_title = "Serious Engine Sandbox Game";
    entity_window->win_api = GfxAPIType::GAT_OGL;
    entity_window->win_adapter = 0;
    entity_window->win_depth = DisplayDepth::DD_32BIT;
    entity_window->win_flags = SE_MAINWINDOW_FLAGS_NULL;
    ECSManager::addEntity((SEEntity*)entity_window, sizeof(main_window));

    /*
    game_control->kb_keybind[SE_KEYBIND_EXIT] = SDLK_ESCAPE;
    game_control->kb_keybind[SE_KEYBIND_FULLSCREEN] = SDLK_F1;

    game_control->kb_keybind[SE_KEYBIND_DEBUG_BORDER] = SDLK_F6;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_ENTITYID] = SDLK_F7;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_POSITION] = SDLK_F8;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_FPS] = SDLK_F9;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_CURSOR] = SDLK_F10;
    */

    Camera* camera = new Camera();
    camera->cam_fov = 90.0f;
    camera->cam_pos = world_start_position;
    camera->cam_rot = world_start_rotation;
    camera->cam_speed = 1.0f; /*
    memset(camera->kb_keybind, 0, sizeof(ULONG) * SE_ECS_KEYBIND_MAX);
    camera->kb_keybind[SE_KEYBIND_CAMERA_RESET] = SDLK_F12;
    camera->kb_keybind[SE_KEYBIND_CAMERA_RIGHT] = SDLK_RIGHT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_LEFT] = SDLK_LEFT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_FORWARD] = SDLK_UP;
    camera->kb_keybind[SE_KEYBIND_CAMERA_BACK] = SDLK_DOWN;
    camera->kb_keybind[SE_KEYBIND_CAMERA_UP] = SDLK_SPACE;
    camera->kb_keybind[SE_KEYBIND_CAMERA_DOWN] = SDLK_c;*/
    ECSManager::addEntity(camera, sizeof(Camera));

    mouse_texture* e_mouse_texture = new mouse_texture;
    e_mouse_texture->tex_data.SetData_t(CTFILENAME("TexturesMP\\General\\Pointer.tex"));
    ECSManager::addEntity((SEEntity*)e_mouse_texture, sizeof(mouse_texture));

    MenuImage* logosam = new MenuImage();
    logosam->pos_x = 480;
    logosam->pos_y = 10;
    logosam->pos_w = 1024;
    logosam->pos_h = 256;
    logosam->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\logo.tex"));
    ECSManager::addEntity((SEEntity*)logosam, sizeof(MenuImage));

    MenuImage* logoct = new MenuImage();
    logoct->pos_x = 16;
    logoct->pos_y = 864;
    logoct->pos_w = 200;
    logoct->pos_h = 200;
    logoct->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\LogoCT.tex"));
    ECSManager::addEntity((SEEntity*)logoct, sizeof(MenuImage));

    MenuImage* logose = new MenuImage();
    logose->pos_x = 1704;
    logose->pos_y = 864;
    logose->pos_w = 200;
    logose->pos_h = 200;
    logose->tex_data.SetData_t(CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
    ECSManager::addEntity((SEEntity*)logose, sizeof(MenuImage));

    MenuButton* menu_button_sp = new MenuButton;
    menu_button_sp->pos_y = 300;
    menu_button_sp->pos_w = 300;
    menu_button_sp->pos_h = 50;
    menu_button_sp->txt_align = 0;
    menu_button_sp->align_x = 0;
    menu_button_sp->txt_fontdata = font_big;
    menu_button_sp->txt_fontsize = 2;
    menu_button_sp->txt_mode = 0;
    menu_button_sp->txt_str = TRANS("SINGLE PLAYER");
    menu_button_sp->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_sp->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_sp, sizeof(MenuButton));

    MenuButton* menu_button_net = new MenuButton;
    menu_button_net->pos_y = 375;
    menu_button_net->pos_w = 300;
    menu_button_net->pos_h = 50;
    menu_button_net->txt_align = 0;
    menu_button_net->align_x = 0;
    menu_button_net->txt_fontdata = font_big;
    menu_button_net->txt_fontsize = 2;
    menu_button_net->txt_mode = 0;
    menu_button_net->txt_str = TRANS("NETWORK");
    menu_button_net->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_net->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_net, sizeof(MenuButton));

    MenuButton* menu_button_split = new MenuButton;
    menu_button_split->pos_y = 450;
    menu_button_split->pos_w = 300;
    menu_button_split->pos_h = 50;
    menu_button_split->txt_align = 0;
    menu_button_split->align_x = 0;
    menu_button_split->txt_fontdata = font_big;
    menu_button_split->txt_fontsize = 2;
    menu_button_split->txt_mode = 0;
    menu_button_split->txt_str = TRANS("SPLIT SCREEN");
    menu_button_split->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_split->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_split, sizeof(MenuButton));

    MenuButton* menu_button_demo = new MenuButton;
    menu_button_demo->pos_y = 525;
    menu_button_demo->pos_w = 300;
    menu_button_demo->pos_h = 50;
    menu_button_demo->txt_align = 0;
    menu_button_demo->align_x = 0;
    menu_button_demo->txt_fontdata = font_big;
    menu_button_demo->txt_fontsize = 2;
    menu_button_demo->txt_mode = 0;
    menu_button_demo->txt_str = TRANS("DEMO");
    menu_button_demo->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_demo->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_demo, sizeof(MenuButton));

    MenuButton* menu_button_mod = new MenuButton;
    menu_button_mod->pos_y = 600;
    menu_button_mod->pos_w = 300;
    menu_button_mod->pos_h = 50;
    menu_button_mod->txt_align = 0;
    menu_button_mod->align_x = 0;
    menu_button_mod->txt_fontdata = font_big;
    menu_button_mod->txt_fontsize = 2;
    menu_button_mod->txt_mode = 0;
    menu_button_mod->txt_str = TRANS("MODS");
    menu_button_mod->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_mod->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_mod, sizeof(MenuButton));

    MenuButton* menu_button_hs = new MenuButton;
    menu_button_hs->pos_y = 675;
    menu_button_hs->pos_w = 300;
    menu_button_hs->pos_h = 50;
    menu_button_hs->txt_align = 0;
    menu_button_hs->align_x = 0;
    menu_button_hs->txt_fontdata = font_big;
    menu_button_hs->txt_fontsize = 2;
    menu_button_hs->txt_mode = 0;
    menu_button_hs->txt_str = TRANS("HIGH SCORES");
    menu_button_hs->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_hs->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_hs, sizeof(MenuButton));

    MenuButton* menu_button_opt = new MenuButton;
    menu_button_opt->pos_y = 750;
    menu_button_opt->pos_w = 300;
    menu_button_opt->pos_h = 50;
    menu_button_opt->txt_align = 0;
    menu_button_opt->align_x = 0;
    menu_button_opt->txt_fontdata = font_big;
    menu_button_opt->txt_fontsize = 2;
    menu_button_opt->txt_mode = 0;
    menu_button_opt->txt_str = TRANS("OPTIONS");
    menu_button_opt->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_opt->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_opt, sizeof(MenuButton));

    MenuButton* menu_button_quit = new MenuButton;
    menu_button_quit->pos_y = 825;
    menu_button_quit->pos_w = 300;
    menu_button_quit->pos_h = 50;
    menu_button_quit->txt_align = 0;
    menu_button_quit->align_x = 0;
    menu_button_quit->txt_fontdata = font_big;
    menu_button_quit->txt_fontsize = 2;
    menu_button_quit->txt_mode = 0;
    menu_button_quit->txt_str = TRANS("QUIT");
    menu_button_quit->sea_action = quitgame;
    menu_button_quit->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_quit->btn_color2 = SE_COL_ORANGE_DARK | 255;
    ECSManager::addEntity((SEEntity*)menu_button_quit, sizeof(MenuButton));
}