#ifndef SSMF_CONTROLS_H
#define SSMF_CONTROLS_H

#include "InterfaceSDL.h"
#include "MainWindow.h"
#include "ECS/RenderSystem.h"
#include "ECS/Manager.h"

extern SEMainWindow* main_win;
extern CDrawPort* main_dp;
extern CViewPort* main_vp;
extern BOOL main_menu_started;
extern BOOL main_game_started;
extern RenderSystem* render_system;
extern CPlayerEntity* pen;
extern ECSManager* manager;

void quitgame()
{
    main_game_started = FALSE;
}

void resolution_fullscreen() 
{
    if(main_win->getMode() == SE_WINDOW_MODE_FULLSCREEN)
        main_win->setMode(SE_WINDOW_MODE_WINDOWED);
    else
        main_win->setMode(SE_WINDOW_MODE_FULLSCREEN);
        
    main_dp->Unlock();
    main_win->create();
    main_dp = main_win->getDrawPort();
    main_vp = main_win->getViewPort();
    main_dp->Lock();
}

void resolution_vga()
{
    ULONG w = 640;
    ULONG h = 480;
    
    if(main_win->getW() != w || main_win->getH() != h) {
        main_dp->Unlock();
        main_win->setW(w);
        main_win->setH(h);
        main_win->create();
        main_dp = main_win->getDrawPort();
        main_vp = main_win->getViewPort();
        main_dp->Lock();
    }
}

void resolution_svga()
{
    ULONG w = 800;
    ULONG h = 600;
    
    if(main_win->getW() != w || main_win->getH() != h) {
        main_dp->Unlock();
        main_win->setW(w);
        main_win->setH(h);
        main_win->create();
        main_dp = main_win->getDrawPort();
        main_vp = main_win->getViewPort();
        main_dp->Lock();
    }
}

void resolution_wxga()
{
    ULONG w = 1280;
    ULONG h = 720;
    
    if(main_win->getW() != w || main_win->getH() != h) {
        main_dp->Unlock();
        main_win->setW(w);
        main_win->setH(h);
        main_win->create();
        main_dp = main_win->getDrawPort();
        main_vp = main_win->getViewPort();
        main_dp->Lock();
    }
}

void resolution_hd()
{
    ULONG w = 1920;
    ULONG h = 1080;
    
    if(main_win->getW() != w || main_win->getH() != h) {
        main_dp->Unlock();
        main_win->setW(w);
        main_win->setH(h);
        main_win->create();
        main_dp = main_win->getDrawPort();
        main_vp = main_win->getViewPort();
        main_dp->Lock();
    }
}

void debug_draw_border()
{
    render_system->dbg_draw_border=!render_system->dbg_draw_border;
}

void debug_draw_id()
{
    render_system->dbg_draw_id=!render_system->dbg_draw_id;
}

void debug_draw_position()
{
    render_system->dbg_draw_position=!render_system->dbg_draw_position;
}

void debug_reset_position()
{
    pen->en_plPlacement = CPlacement3D(FLOAT3D(10,40,0),ANGLE3D(90,0,10));
}

void player_move_right()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(2.0f,0.0f,0.0f));
}

void player_move_left()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(-2.0f,0.0f,0.0f));
}

void player_move_forward()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,2.0f,0.0f));
}

void player_move_back()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,-2.0f,0.0f));
}

void player_move_up()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,0.0f,2.0f));
}

void player_move_down()
{
    pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,0.0f,-2.0f));
}

void load_game_controls()
{
    GameControl* game_f1 = new GameControl;
    game_f1->sekc_key = SDLK_F1;
    game_f1->sea_action = resolution_fullscreen;
    manager->addEntity((SEEntity*)game_f1);

    GameControl* game_f2 = new GameControl;
    game_f2->sekc_key = SDLK_F2;
    game_f2->sea_action = resolution_vga;
    manager->addEntity((SEEntity*)game_f2);

    GameControl* game_f3 = new GameControl;
    game_f3->sekc_key = SDLK_F3;
    game_f3->sea_action = resolution_svga;
    manager->addEntity((SEEntity*)game_f3);

    GameControl* game_f4 = new GameControl;
    game_f4->sekc_key = SDLK_F4;
    game_f4->sea_action = resolution_wxga;
    manager->addEntity((SEEntity*)game_f4);

    GameControl* game_f5 = new GameControl;
    game_f5->sekc_key = SDLK_F5;
    game_f5->sea_action = resolution_hd;
    manager->addEntity((SEEntity*)game_f5);

    GameControl* game_f6 = new GameControl;
    game_f6->sekc_key = SDLK_F6;
    game_f6->sea_action = debug_draw_border;
    manager->addEntity((SEEntity*)game_f6);

    GameControl* game_f7 = new GameControl;
    game_f7->sekc_key = SDLK_F7;
    game_f7->sea_action = debug_draw_id;
    manager->addEntity((SEEntity*)game_f7);

    GameControl* game_f8 = new GameControl;
    game_f8->sekc_key = SDLK_F8;
    game_f8->sea_action = debug_draw_position;
    manager->addEntity((SEEntity*)game_f8);

    GameControl* game_player_right = new GameControl;
    game_player_right->sekc_key = SDLK_RIGHT;
    game_player_right->sea_action = player_move_right;
    manager->addEntity((SEEntity*)game_player_right);

    GameControl* game_player_left = new GameControl;
    game_player_left->sekc_key = SDLK_LEFT;
    game_player_left->sea_action = player_move_left;
    manager->addEntity((SEEntity*)game_player_left);

    GameControl* game_player_forward = new GameControl;
    game_player_forward->sekc_key = SDLK_UP;
    game_player_forward->sea_action = player_move_left;
    manager->addEntity((SEEntity*)game_player_forward);

    GameControl* game_player_back = new GameControl;
    game_player_back->sekc_key = SDLK_DOWN;
    game_player_back->sea_action = player_move_left;
    manager->addEntity((SEEntity*)game_player_back);

    GameControl* game_player_up = new GameControl;
    game_player_up->sekc_key = SDLK_SPACE;
    game_player_up->sea_action = player_move_up;
    manager->addEntity((SEEntity*)game_player_up);

    GameControl* game_player_down = new GameControl;
    game_player_down->sekc_key = SDLK_c;
    game_player_down->sea_action = player_move_down;
    manager->addEntity((SEEntity*)game_player_down);
}

#endif