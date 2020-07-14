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

#include "MainWindow.h"
#include <QApplication>

#include <Engine/Engine.h>

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
char* argv0 = NULL;

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    argv0 = argv[0];

    CTString cmdline;
    for (int i = 1; i < argc; i++) {
        cmdline += " \"";
        cmdline += argv[i];
        cmdline += "\"";
    }
    MainWindow w;

    w.show();
    return a.exec();
}
