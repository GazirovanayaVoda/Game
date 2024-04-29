#ifndef MACRO_CONSTS_H_INCLUDED
#define MACRO_CONSTS_H_INCLUDED

    #define UINT8MAX            255
    #define UINT8MIN            0
    #define INT8MAX             127
    #define INT8MIX             -128

    #define UINT16MAX           65535
    #define UINT16MIX           0
    #define INT16MAX            32767
    #define INT16MIN            –32768

    #define UINT32MAX           4294967295
    #define UINT32MIN           0
    #define INT32MAX            2147483647
    #define INT32MIN            –2147483648

    #define START_DEBUG         false

    #define WINDOW_WIDTH        1280
    #define WINDOW_HEIGHT       720
    #define WINDOW_NAME         "Game"
    #define GAME_CLASS_NAME     "Project_game"
    #define TARGET_FPS          75


    #define MAXCOINCOUNT        8
    #define COINQUALITY         16

    #define MAXLEVELS           3
    #define MAXOBJECTS          16
    #define MAXSPEEDVARS        16
    #define MAXLEVELNAME        48

    #define MAXENEMYCOUNT       4

    #define DEBUG_TEXT_SIZE     0.005f

    #define DEATHSCREEN         0.01f
    #define DEATHCONST          2.0f

    #define PLAYER_SPEED_X      0.025f
    #define JUMP_FORCE          0.035f

    #define TARGET_GRAVITY      0.001f                  //0,001f

    #define NEXT_LEVEL_CONST    7.5f

    #define LEVEL_SKY          0.5f, 0.75f, 1.0f, 1.0f

    #define NULL_COLOR         _color4f_(-1,-1,-1,-1)

    #define DEBUGLINESIZE      5
    #define DEBUGPOINTSIZE     10

    #define PLAYERPOSX      player->bobj.pos.x
    #define PLAYERPOSY      player->bobj.pos.y

    #define PLAYERSIZEX     player->bobj.sizex
    #define PLAYERSIZEY     player->bobj.sizey

    #define OBJPOSX         obj->pos.x
    #define OBJPOSY         obj->pos.y

    #define OBJSIZEX        obj->sizex
    #define OBJSIZEY        obj->sizey


#endif // MACRO_CONSTS_H_INCLUDED