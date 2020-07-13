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

    Static Variable:                (?)                     (?)
    Static Method:                  (?)                     (?)
    Static Member:                  (?)                     (?)
    Namespace:                      (?)                     (?)


*/