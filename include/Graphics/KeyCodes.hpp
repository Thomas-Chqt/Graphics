/*
 * ---------------------------------------------------
 * KeyCodes.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/11 12:53:22
 * ---------------------------------------------------
 */

#ifndef KEYCODES_HPP
# define KEYCODES_HPP

#ifdef USING_APPKIT
    #define ESC_KEY   53

    #define ONE_KEY   18
    #define TWO_KEY   19

    #define W_KEY     13
    #define A_KEY     0
    #define S_KEY     1
    #define D_KEY     2

    #define SPACE_KEY 49

    #define UP_KEY    126
    #define LEFT_KEY  123
    #define DOWN_KEY  125
    #define RIGHT_KEY 124

    #define MOUSE_L   1
    #define MOUSE_R   2
#else
#ifdef USING_GLFW
    #define ESC_KEY   0

    #define ONE_KEY   49
    #define TWO_KEY   50

    #define W_KEY     87
    #define A_KEY     65
    #define S_KEY     83
    #define D_KEY     68

    #define SPACE_KEY 32

    #define UP_KEY    9
    #define LEFT_KEY  7
    #define DOWN_KEY  8
    #define RIGHT_KEY 6

    #define MOUSE_L   0
    #define MOUSE_R   1
#endif
#endif

#endif // KEYCODES_HPP