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
#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Base/Stream.h>
#include <Engine/Engine.h>
#include <Engine/Sound/SoundLibrary.h>

#include "MainWindow.h"
#include "SplashScreen.h"
#include <SER/ECS/Manager.h>

#include "ECSGame/Entity.h"
#include "ECSGame/RenderSystem.h"

#include <thread>

using namespace SER;

// FIXME: Cant compile without this global variable
HWND _hwndMain = NULL;

static char* argv0 = NULL;
extern void load_all_game_setting();
extern void load_all_game_system();
extern void load_all_game_entity();

CTFileName g_world_file = CTFILENAME("Levels\\TestGame.wld");
CWorld* g_world_data = NULL;
CTString g_gamename = "ser_sandbox";

CTFileName g_logfile = CTFILENAME("fast.log");
CTFileStream g_logstream;

COLOR g_fb_color = C_BLACK | 0xff;

UINT g_list_resolution[][2] = {
    { 640, 480 },
    { 800, 600 },
    { 1280, 720 },
    { 1920, 1080 }
};

int submain(char* _cmdline)
{
    SESplashScreen splashscreen;
    MainWindow mainwindow;

    splashscreen.setBitmap("Splash.bmp");
    splashscreen.show();

    // initialize engine
    SE_InitEngine(argv0, g_gamename);

    SE_LoadDefaultFonts();
    CFontData font_small;
    CFontData font_medium;
    CFontData font_big;

    font_small.Load_t(CTFILENAME("Fonts\\Display3-narrow.fnt"));
    font_medium.Load_t(CTFILENAME("Fonts\\Display3-normal.fnt"));
    font_big.Load_t(CTFILENAME("Fonts\\Display3-caps.fnt"));

    int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    // Add space for 1 MB + 256 Event pointers (32/64 bit per pointer) + 256 Setting pointers (32/64 bit per pointer)
    Manager::init((1UL << 20), 256, 256);

    load_all_game_setting();
    load_all_game_system();
    if (!Manager::getEntityManager()->loadDisk("Levels\\TestGame.bin")) {
        load_all_game_entity();
        Manager::getEntityManager()->saveDisk("Levels\\TestGame.bin");
    }

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    ULONG number_thread = 1;
    Manager::setThreadNumber(number_thread);
    Manager::splitThreadMemory();

    g_world_data = new CWorld;
    g_world_data->Load_t(g_world_file);

    int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    printf("Loading time t1: %ld\n", t1 - t0);
    printf("Loading time t2: %ld\n", t2 - t1);

    mainwindow.setTitle("Serious Engine Reloaded Sandbox Game");
    mainwindow.setAPI(GfxAPIType::GAT_OGL);
    mainwindow.setDepth(DisplayDepth::DD_32BIT);
    splashscreen.hide();

    while (Manager::isGameStarted()) {
        mainwindow.init();
        {
            RenderSystem* render_ptr = (RenderSystem*)Manager::getRenderSystem();
            render_ptr->setDrawPort(mainwindow.getDrawPort());
            render_ptr->setViewPort(mainwindow.getViewPort());
        }
        Manager::run();
        mainwindow.destroy();
    }
    SE_EndEngine();
    return TRUE;
}

int main(int _argc, char** _argv)
{
    argv0 = _argv[0];

    CTString cmdline;
    for (int i = 1; i < _argc; i++) {
        cmdline += " \"";
        cmdline += _argv[i];
        cmdline += "\"";
    }

    int result;
    CTSTREAM_BEGIN
    {
        result = submain((char*)((const char*)cmdline));
    }
    CTSTREAM_END;

    return result;
}