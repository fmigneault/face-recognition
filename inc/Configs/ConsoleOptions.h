#ifndef FACE_RECOG_CONSOLE_OPTIONS_H
#define FACE_RECOG_CONSOLE_OPTIONS_H

/*
Usage example:

    HANDLE hConsole;
    setTextColor(hConsole,YELLOW_TEXT|PINK_BACKGROUND);
    cout << "YELLOW_TEXT|PINK_BACKGROUND";
    resetTextColor(hConsole);

    OR

    // with [background] and [text] as unique hexadecimaal character
    system("COLOR [background][text]");
    // ex:
    system("COLOR 1A");
    cout << "BLUE_FADE_BACKGROUND|GREEN_TEXT";

Source:
http://codereview.stackexchange.com/questions/42467/user-defined-color-implementation-in-windows-console
*/

#include "Configs/Platform.h"

#ifdef FACE_RECOG_WINDOWS
#define CONSOLE_COLOR_ENABLED
#endif

////////////////////////////////////////////////// LINUX (TODO)
// http://bluesock.org/~willg/dev/ansi.html
// http://stackoverflow.com/questions/3506504/c-code-changes-terminal-text-color-how-to-restore-defaults-linux
//////////////////

#ifdef CONSOLE_COLOR_ENABLED
    /* Set according to usage */
    //#define SET_COLOR_WITH_SYSTEM
    #ifndef SET_COLOR_WITH_SYSTEM
        #ifndef NOMINMAX
        #define NOMINMAX
        #endif
        #include <windows.h>
    #endif

    enum COLOR
    {
        /* Text foreground colors */
        // Faded text colors
        BLACK_TEXT              = 0x00,
        BLUE_FADE_TEXT          = 0x01,
        GREEN_FADE_TEXT         = 0x02,
        TEAL_FADE_TEXT          = 0x03,
        RED_FADE_TEXT           = 0x04,
        PINK_FADE_TEXT          = 0x05,
        YELLOW_FADE_TEXT        = 0x06,
        WHITE_FADE_TEXT         = 0x07,
        // Standard text colors
        GRAY_TEXT               = 0x08,
        BLUE_TEXT               = 0x09,
        GREEN_TEXT              = 0x0A,
        TEAL_TEXT               = 0x0B,
        RED_TEXT                = 0x0C,
        PINK_TEXT               = 0x0D,
        YELLOW_TEXT             = 0x0E,
        WHITE_TEXT              = 0x0F,
        /* Background colors (<COLOR>_TEXT << 4) */
        // Faded text background color
        BLACK_BACKGROUND        = 0x00,
        BLUE_FADE_BACKGROUND    = 0x10,
        GREEN_FADE_BACKGROUND   = 0x20,
        TEAL_FADE_BACKGROUND    = 0x30,
        RED_FADE_BACKGROUND     = 0x40,
        PINK_FADE_BACKGROUND    = 0x50,
        YELLOW_FADE_BACKGROUND  = 0x60,
        WHITE_FADE_BACKGROUND   = 0x70,
        // Standard text background color
        GRAY_BACKGROUND         = 0x80,
        BLUE_BACKGROUND         = 0x90,
        GREEN_BACKGROUND        = 0xA0,
        TEAL_BACKGROUND         = 0xB0,
        RED_BACKGROUND          = 0xC0,
        PINK_BACKGROUND         = 0xD0,
        YELLOW_BACKGROUND       = 0xE0,
        WHITE_BACKGROUND        = 0xF0,
    };

    #ifndef SET_COLOR_WITH_SYSTEM
    inline BOOL resetTextColor(HANDLE h)
    {
        return SetConsoleTextAttribute(h, WHITE_FADE_TEXT);
    }

    inline BOOL setTextColor(HANDLE h, WORD c)
    {
        return SetConsoleTextAttribute(h, c);
    }
    #endif
#endif

#endif /*FACE_RECOG_CONSOLE_OPTIONS_H*/
