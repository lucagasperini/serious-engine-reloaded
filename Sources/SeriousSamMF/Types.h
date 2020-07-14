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

#ifndef SER_TYPES_H
#define SER_TYPES_H

#include <stdint.h>
/*
    Just a cool type definition standard
*/
typedef bool BOOL;

typedef int8_t BYTE;
typedef uint8_t UBYTE;
typedef uint_fast8_t UFBYTE;
typedef int_fast8_t FBYTE;
typedef int_least8_t LBYTE;
typedef uint_least8_t ULBYTE;

typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef uint_fast16_t UFSHORT;
typedef int_fast16_t FSHORT;
typedef int_least16_t LSHORT;
typedef uint_least16_t ULSHORT;

typedef int32_t INT;
typedef uint32_t UINT;
typedef uint_fast32_t UFINT;
typedef int_fast32_t FINT;
typedef int_least32_t LINT;
typedef uint_least32_t ULINT;

typedef int64_t LONG;
typedef uint64_t ULONG;
typedef uint_fast64_t UFLONG;
typedef int_fast64_t IFLONG;
typedef int_least64_t LLONG;
typedef uint_least64_t ULLONG;

typedef intptr_t PTR;
typedef uintptr_t UPTR;

typedef unsigned char UTF8;
typedef char16_t UTF16;
typedef char32_t UTF32;

typedef void VOID;
typedef char CHAR;
typedef float FLOAT;
typedef double DOUBLE;
typedef long double LDOUBLE;

typedef uint_fast32_t INDEX;
typedef uint_fast16_t PIX;
typedef uint32_t COLOR;

#endif

/*
    Maybe define a standard for NAMES:

    Fundamental types:              Upper Case              (INT, CHAR, UBYTE, etc...)
    Fundamental types with context: Upper Case              (INDEX, PIX, COLOR, etc...)
    Class Names:                    Pascal Case             (SEMainWindow, RenderSystem, ECSManager, etc...)
    Struct Names:                   Pascal Case             (SEPositionComponent, SEKeyboardComponent, SECameraComponent, etc...)
    Function Names:                 Lower Case              (main, init, resolution_change, etc...)
    Method Names:                   Camel Case              (getX, addEntity, setDrawPort, etc...)
    Variables Names:                Lower Case              (result, manager, world_start_position, etc...)
    Member Names:                   Lower Case              (title, texture, render_system, etc...)
    Arguments Name:                 Underscore + Lower Case (_w, _position, _camera, etc...)
    Constant Names:                 Upper Case              (SE_COL_ORANGE_NEUTRAL, FOO_BAR, etc...)
    Define Names:                   Upper Case              (SE_MAINWINDOW_FLAGS_NULL, SSMF_WINDOW_RECOVERY_H, SSMF_INPUTSYSTEM_H, ecc...)
    File Name CPP:                  Pascal Case             (PositionSystem.cpp, MainWindow.cpp, Entity.cpp, etc...)
    File Name H:                    Pascal Case             (PositionSystem.h, Types.h, Manager.h)
    File Name UI:                   Pascal Case             (MainWindow.ui, etc...)
    Folder Names:                   Pascal Case             (SeriousSam, SeriousEditor, Bin)
    Comment Fixme:                  FIXME(<EMAIL>, BugID):  FIXME(<luca.gasperini@xsoftware.it>, 111): Please fix this stuff!
    Comment Todo:                   TODO(<EMAIL>, BugID):   TODO(<luca.gasperini@xsoftware.it>, 111): Please do this stuff!
    Static Variable:                (?)                     (?)
    Static Method:                  (?)                     (?)
    Static Member:                  (?)                     (?)
    Namespace:                      (?)                     (?)

    Other cool RULES:

    When define a pointer of a variable, place the "*" char on type name, not the variable name (example : VOID* myptr).
    Float numbers end with "f" char and a number after point (example : 1.0f)
    Try to stay on 80 char row length (Except for comment).
    Use Inline comment to describe code.
    Use Block comment to describe functions, class, struct, enum and method declaration.
    Use Preprocessor Directives at the start of the line.
    Namespaces have not extra level of indentation
*/