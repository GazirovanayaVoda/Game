#include <windows.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "libs/vec2.h"
#include "libs/color4f.h"

#include "libs/glad/include/glad/glad.h"

#define MINIAUDIO_IMPLEMENTATION
#include "libs/miniaudio/miniaudio.h"

#include "libs/stb-font/stb_easy_font.h"
#include "version.h"
#include "libs/func.h"
#include "libs/error_codes.h"
#include "libs/macro_functions.h"

#include "libs/coins.h"
#include "libs/enemy.h"

#include "libs/macro_consts.h"

/*constants*/

const float _ratio_ = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

float gravity = TARGET_GRAVITY;

/*end*/

/*global variables*/

bool debug = START_DEBUG;
bool end_screen = false;
bool vsync = false;

ma_result sound_eng_result;
ma_engine sound_engine;


/*function to play sounds*/
void playSound(ma_engine *engine, const char *path)
{
    /*trying to play sound*/
    if (ma_engine_play_sound(engine, path, NULL) != MA_SUCCESS) {
        /*print error*/
        printf("Can't play sound: %s\n", path);
        print_err(CANT_PLAY_SOUND_ERR);
    }
}

struct timeval stop, start;
float delta_time = 1;
float fps = 1;
float fps_time;

float death_screen;
uint8_t Level = 0;

vec2 mouse_pos;

const char Level_list[MAXLEVELS][MAXLEVELNAME] = {
"res/levels/level0.lvl",
"res/levels/level1.lvl",
"res/levels/level2.lvl"
};

const char coin_map_list[MAXCOINCOUNT][MAXLEVELNAME] = {
"res/coin_maps/coin_map0.lvl",
"res/coin_maps/coin_map1.lvl",
"res/coin_maps/coin_map2.lvl",
};

const char enemy_map_arr[MAXENEMYCOUNT][MAXLEVELNAME] = {
"res/enemy_maps/enemy_map0.lvl",
"res/enemy_maps/enemy_map1.lvl",
"res/enemy_maps/enemy_map2.lvl",
};

/*end*/

/*
TO DO LIST
1. Player, Physics DONE
2. Levels System   DONE
3. Sounds          DONE

TO DO LIST 28/12/2023
1. Upgrade collision system DONE
1.1 Fix fps
2. Make CoInS!!!
3. Make cool levels (1/3)
4. Done project
*/



/*some sht*/
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

/* function for drawing rectangles */
void draw_rect(float x, float y, float w, float h, color4f clr)
{
    float vertex_arr[4][2] = {
        {0.0f,0.0f},
        {1.0f,0.0f},
        {0.0f,1.0f},
        {1.0f,1.0f},
    };

    float color_arr[4][4] = {
        {clr.r,clr.g,clr.b,clr.a},
        {clr.r,clr.g,clr.b,clr.a},
        {clr.r,clr.g,clr.b,clr.a},
        {clr.r,clr.g,clr.b,clr.a}
    };

    glPushMatrix();
    glTranslatef(x,y,.0f);
    glScalef(w/_ratio_,h,.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2,GL_FLOAT,NULL,vertex_arr); /*2 - for 2d; 3 - for 3d*/
    glColorPointer(4,GL_FLOAT,NULL,color_arr);  /*3 - for RGB; 4 - for RGBA*/

    glDrawArrays(GL_TRIANGLE_STRIP, NULL, 4);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}



/* object type */
typedef struct _obj {
    vec2 bpos;
    vec2 pos;
    vec2 center;
    float sizex, sizey;
    color4f clr;
    bool plr;
} object;

/* all walls in game */
object object_arr[MAXOBJECTS];
coin coins_arr[MAXCOINCOUNT+1];
enemy enemy_arr[MAXENEMYCOUNT];


    /* object initializer */
    object object_init(float x, float y, float sx, float sy, float r, float g, float b, bool plr)
    {
        object self;
        self.bpos = vec2_init(x,y);
        self.pos = self.bpos;
        self.clr = _color4f_(r,g,b,1.0f);
        self.sizex = sx;
        self.sizey = sy;

        self.center = vec2_init((x + (sx / 2.0f)) / _ratio_, y + (sy / 2.0f));

        self.plr = plr;

        return self;
    }

    /* functuion for printing object data */
    void object_prdata(object* obj)
    {
        printf("pos: (%f ; %f) size: (%f ; %f) colorru/?at=12111638453339728127284: (%f ; %f ; %f ; %f)\n",
               obj->pos.x,
               obj->pos.y,
               obj->pos.x,
               obj->pos.y,
               obj->clr.r,
               obj->clr.g,
               obj->clr.b,
               obj->clr.a);
    }


    /* function for drawing objects */
    void object_draw(object* obj)
    {
        draw_rect(obj->pos.x,
                  obj->pos.y,
                  obj->sizex,
                  obj->sizey,
                  obj->clr);
    }

    /* function for moving object to some pos */
    void object_moveto(object* obj, vec2* pos) {
        obj->pos = *pos;
    }

    /* function for moving object on some vec */
    void object_moveon(object* obj, vec2* vec) {
        obj->pos = vec2_plus(obj->pos, (vec2){(vec->x) * FPS_MULT, (vec->y) * FPS_MULT});
    }


/*player type*/
typedef struct _player {
    object bobj;
    vec2 speed;
    bool collide;
    uint8_t coin_count, coins;
    int8_t hp, damage;
} player;

    /* player initializer */
    player player_init(float x, float y, float szx, float szy, float r, float g, float b, float dmg)
    {
        player self;
        self.bobj = object_init(x,y,szx,szy,1,1,1,true);
        self.speed = vec2_init(.0f,.0f);
        self.hp = INT8MAX;
        self.damage = dmg;
        return self;
    }

void player_getdamage(player *player, enemy* enem)
{
    #define ENEMY_POSX  enem->pos.x
    #define ENEMY_POSY  enem->pos.y
    #define ENEMY_SIZEX enem->size.x
    #define ENEMY_SIZEY enem->size.y

    bool Collide = (PLAYERPOSX + PLAYERSIZEX > ENEMY_POSX) && 
    (PLAYERPOSX < ENEMY_POSX + ENEMY_SIZEX) &&
    (PLAYERPOSY + PLAYERSIZEY > ENEMY_POSY) &&
    (PLAYERPOSY < ENEMY_POSY + ENEMY_SIZEY);

    if (Collide && enem->alive) {
        player->hp -= enem->damage;
        enem->pos.y += 2.0f; 
    }
}

void movcam(vec2 x) {
    for (uint8_t i = 0; i < MAXOBJECTS; ++i) {
        object_moveon(&object_arr[i],&x);
    }
    for (uint8_t i = 0; i < MAXCOINCOUNT; ++i) {
        coin_moveon(&coins_arr[i], &((vec2){x.x * FPS_MULT, x.y * FPS_MULT}));
    }
    for (uint8_t i = 0; i < MAXENEMYCOUNT; ++i) {
        enemy_moveon(&enemy_arr[i], &((vec2){x.x * FPS_MULT, x.y * FPS_MULT}));
    }
}

void reset_pos() {
    for (uint8_t i = 0; i < MAXOBJECTS; ++i) {
        object_moveto(&object_arr[i],&object_arr[i].bpos);
    }

    for (uint8_t i = 0; i < MAXCOINCOUNT; ++i) {
        coin_moveto(&coins_arr[i], &coins_arr[i].bpos);      
    }

    for (uint8_t i = 0; i < MAXENEMYCOUNT; ++i) {
        enemy_reset_pos(&enemy_arr[i]);        
    }
}

/*drawing text*/
void draw_text(const float x, const float y, char *text, const float size)
{
    glPushMatrix();
    glTranslatef(x,y,0.0f);
    glScalef(size / _ratio_,-size,1);

    BASIC_PRINT;

    glPopMatrix();
}

void draw_num(const float x, const float y, uint8_t num, const float size)
{
    char Arr[4];
    /*parsing nums*/
    
    for (uint8_t u=0; u<3; ++u) {
        Arr[2-u] = num%10 + 48;
        num /= 10;
    }
    draw_text(x,y,&Arr,size);
}

color4f flashscreen;

void flash(float r, float g, float b, float time) {
    death_screen = time;
    flashscreen = _color4f_(r,g,b,death_screen);
}

void flashscr()
{
    draw_rect(-1.0f,-1.0f,4.0f,2.0f,_color4f_(flashscreen.r,
                                             flashscreen.g,
                                             flashscreen.b,
                                             fclamp(death_screen,0.0f,1.0f)));
    death_screen -= DEATHSCREEN * FPS_MULT;
}


player main_player;

void init_enemies(const uint8_t num)
{
    FILE* enemy_map_file;
    int8_t err_code = 0;
    uint8_t i = 0;

    printf("Attempting to open enemy map: %s\n", enemy_map_arr[num]);

    enemy_map_file = fopen(enemy_map_arr[num], "r");

    if (enemy_map_file == NULL) {
        err_code = CANT_READ_ENEMY_MAP_FILE;
        print_err(err_code);
    } else if (err_code == SUCCESS) {
        while (fscanf(enemy_map_file, "%f%f%f%f%f%f%f%f%f%f%d",
                    &(enemy_arr[i].bpos.x),
                    &(enemy_arr[i].bpos.y),
                    &(enemy_arr[i].size.x),
                    &(enemy_arr[i].size.y),
                    &(enemy_arr[i].speed.x),
                    &(enemy_arr[i].speed.y),
                    &(enemy_arr[i].color.r),
                    &(enemy_arr[i].color.g),
                    &(enemy_arr[i].color.b),
                    &(enemy_arr[i].color.a),
                    &(enemy_arr[i].damage)) != EOF) {
                        
                        enemy_arr[i].pos = enemy_arr[i].bpos;
                        enemy_arr[i].alive = true;
                        enemy_arr[i].hp = INT8MAX;

                        ++i;
                    }
                    fclose(enemy_map_file);
    } else {
        print_err(ERROR_INIT_ENEMIES);
    }
}

void init_coins(const uint8_t num)
{
  FILE* coin_map_file;
  int8_t err_code = 0;
  uint8_t i = 0;

  printf("Attempting to open coin map: %s\n", coin_map_list[num]);
  coin_map_file = fopen(coin_map_list[num], "r");

  if (coin_map_file == NULL) {
      err_code = CANT_READ_COIN_MAP_FILE;
      print_err(err_code);
  } else if (err_code == SUCCESS) {
      while (fscanf(coin_map_file,"%f%f%f",
                      &(coins_arr[i].pos.x),
                      &(coins_arr[i].pos.y),
                      &(coins_arr[i].rad)) != EOF) {

          coins_arr[i].bpos = coins_arr[i].pos;
          coins_arr[i].color = _color4f_(1, 0.835, 0.157, 1);
          coins_arr[i].exist = true;
          coins_arr[i].quality = COINQUALITY;
          ++i;
      }
      fclose(coin_map_file);
  } else {
      print_err(ERROR_INIT_COINS);
  }
}

void init_level(const uint8_t num)
{
    FILE* Level_file;
    int8_t err_code = 0;
    uint8_t i = 0;

    reset_pos();

    printf("Attempt to read file for level num %d\n",num);
    printf("%s\n", Level_list[num]);

    Level_file = fopen(Level_list[num],"r");

    if (Level_file == NULL) {
        err_code = CANT_READ_LEVEL_FILE;
        print_err(err_code);
    } else if (err_code == SUCCESS) {
        while (fscanf(Level_file,"%f%f%f%f%f%f%f",
                       &(object_arr[i].pos.x),
                       &(object_arr[i].pos.y),
                       &(object_arr[i].sizex),
                       &(object_arr[i].sizey),
                       &(object_arr[i].clr.r),
                       &(object_arr[i].clr.g),
                       &(object_arr[i].clr.b)) != EOF) {
            object_arr[i].bpos = object_arr[i].pos;
            object_arr[i].clr.a = 1.0f;
            ++i;
        };
        fclose(Level_file);
        init_enemies(num);
        init_coins(num);
    }
    else {
        print_err(ERROR_INIT_LEVEL);
    }
}

void initVar()
{
    /*init sound engine*/
    sound_eng_result = ma_engine_init(NULL, &sound_engine);
    if (sound_eng_result != MA_SUCCESS)
        print_err(SOUND_ENGINE_INIT_ERR);
    
    enemy_setratio(_ratio_);
    setRatio(_ratio_);

    /* init player */
    main_player = player_init(0,0,0.25,0.25,1,1,1,100);

    /* init level */
    init_level(Level);
    //init_level(Level); /*stupid shit but fix the bug*/

    /*printing objects information*/
    for (uint8_t i = 0; i < MAXOBJECTS; ++i)
        object_prdata(&object_arr[i]);
}

bool collide(player* player, object* obj)
{
    /*KILL IT WITH FIRE*/

    #define RIGHT           ((PLAYERPOSX + PLAYERSIZEX) > OBJPOSX*_ratio_ - player->speed.x)
    #define DOWN            ((PLAYERPOSY + PLAYERSIZEY) > OBJPOSY + player->speed.y)
    #define LEFT            (PLAYERPOSX < (OBJPOSX + OBJSIZEX / _ratio_ + player->speed.x))
    #define TOP             (PLAYERPOSY < (OBJPOSY + OBJSIZEY + player->speed.y))

    float obj_top_s, obj_down_s, obj_left_s, obj_right_s;
    bool result;

    result = (LEFT&
               TOP&
               RIGHT&
               DOWN
               );

    if (result) {

        obj_top_s = OBJPOSY + OBJSIZEY - PLAYERPOSY;
        obj_down_s = abs(OBJPOSY + OBJSIZEY - PLAYERSIZEY);

        obj_left_s = PLAYERPOSX + PLAYERSIZEX - OBJPOSX;
        obj_right_s = OBJPOSX + OBJSIZEX - PLAYERPOSX;

        if (obj_top_s < obj_down_s) {
            player->speed.y = fclamp(player->speed.y, player->speed.y, 0);
        } else {
            player->speed.y = fclamp(player->speed.y, 0, player->speed.y);
        }

        #define OBJSIZE_CONST 0.01f

        if ((PLAYERPOSY < (OBJPOSY + OBJSIZEY - OBJSIZE_CONST)) && ((PLAYERPOSY + PLAYERSIZEY) > OBJPOSY + OBJSIZE_CONST)) {
            if (obj_left_s < obj_right_s) {
                player->speed.x = fclamp(player->speed.x, 0, player->speed.x);
            } else {
                player->speed.x = fclamp(player->speed.x, player->speed.x, 0);
            }
        }
    }
    return result;
}

bool coin_collide(player* pl, coin* cn)
{
    float sqr_dist;
    sqr_dist = powf((pl->bobj.center.x - cn->pos.x) * _ratio_, 2) + powf(pl->bobj.center.y - cn->pos.y, 2);

    if (sqr_dist < powf(cn->rad, 2)) {
        if (cn->exist) {
            pl->coins += 1;
            playSound(&sound_engine,"res/sounds/coin.wav");
        }
        cn->exist = false;
        return true;
    }
    return false;
}

void collide_all_coins(player* pl)
{
    for (uint8_t i=0;i<MAXCOINCOUNT;++i) {
        if (coin_collide(pl, &coins_arr[i])) {
            break;
        }
    }
}

void collide_all(player *player) 
{
    for (uint8_t col=0;col<MAXOBJECTS;++col)
        if (collide(player, &object_arr[col]))
            break;
}

void changeLevel()
{
    playSound(&sound_engine,"res/sounds/next_level.wav");
    if ((Level + 1) >= MAXLEVELS) {
        end_screen = true;
    } else {
        Level = fclamp(Level + 1, 0, MAXLEVELS - 1);
        printf("Changing level, level: %d\n", Level);
        init_level(Level);
        flash(1.0f,1.0f,1.0f,0.75f);
        reset_pos();
    };
};

void gameLogicUpdate()
{
    if (!end_screen) {
        /*do some things with gravity*/
        GRAVITYF;

        /*Check collisions for all objects with player*/
        collide_all(&main_player);

        collide_all_coins(&main_player);

        for (uint8_t i = 0; i < MAXENEMYCOUNT; ++i) {
            enemy_update(&main_player.bobj.pos, &enemy_arr[i], FPS_MULT);
            player_getdamage(&main_player, &enemy_arr[i]);
        }
        
        /*Move all objects*/
        movcam(main_player.speed);

        if (main_player.hp <= 0) {
            GAME_OVER;
        }

        if (object_arr[0].pos.y > DEATHCONST)
            main_player.hp = 0;

        if (object_arr[0].pos.x < -NEXT_LEVEL_CONST)
            changeLevel();
    }
}

void debug_draw()
{
    float vertex_arr[MAXCOINCOUNT][2];
    float line_arr[MAXCOINCOUNT * 2][2];
    float color_arr[MAXCOINCOUNT * 2][4];

    for (uint8_t i=0; i < MAXCOINCOUNT; ++i) {
        vertex_arr[i][0] = coins_arr[i].pos.x;
        vertex_arr[i][1] = coins_arr[i].pos.y;
    };

    for (uint8_t i=0; i < MAXCOINCOUNT * 2; ++i) {
        color_arr[i][0] = 1.0f;
        color_arr[i][1] = .0f;
        color_arr[i][2] = .0f;
        color_arr[i][3] = .6f;
    };

    for (uint8_t i=0; i < MAXCOINCOUNT * 2; i += 2) {

        line_arr[i][0] = coins_arr[i / 2].pos.x;
        line_arr[i][1] = coins_arr[i / 2].pos.y;
        
        line_arr[i + 1][0] = main_player.bobj.center.x;
        line_arr[i + 1][1] = main_player.bobj.center.y;

    };

    glPushMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2,GL_FLOAT,NULL,vertex_arr); /*2 - for 2d; 3 - for 3d*/
    glColorPointer(4,GL_FLOAT,NULL,color_arr);  /*3 - for RGB; 4 - for RGBA*/

    glDrawArrays(GL_POINTS, NULL, MAXCOINCOUNT);

    glVertexPointer(2,GL_FLOAT,NULL,line_arr);
    glDrawArrays(GL_LINES, NULL, MAXCOINCOUNT * 2);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();

    /*Draw debug text*/
    DEBUG_TEXT_DRAWING;
}

void drawUpdate()
{
    /*clear screen*/
    glClearColor(LEVEL_SKY);
    /*idk*/
    glClear(GL_COLOR_BUFFER_BIT);
    /*Save matrix somewhere in memory*/
    glPushMatrix();

    /*!!! Drawing functions starts here !!!*/

    if (!end_screen) {
        /* Drawing all objects */
        for (uint8_t i = 0; i < MAXOBJECTS; ++i) {                        
            object_draw(&object_arr[i]);                                
        }
        for (uint8_t i = 0; i < MAXCOINCOUNT; ++i) {
            coin_draw(&coins_arr[i]);
        }
        for (uint8_t i = 0; i < MAXENEMYCOUNT; ++i) {
            enemy_draw(&enemy_arr[i]);
        }

        float mouse_vertex[2][2] = {
        {main_player.bobj.center.x, main_player.bobj.center.y},
        {mouse_pos.x, mouse_pos.y}
        };
        float mouse_point_clr[2][4] = {
            {.0f,.0f,.0f,.0f},
            {.0f,.0f,.0f,1.0f}
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2,GL_FLOAT,NULL,mouse_vertex); /*2 - for 2d; 3 - for 3d*/
        glColorPointer(4,GL_FLOAT,NULL,mouse_point_clr);  /*3 - for RGB; 4 - for RGBA*/

        glDrawArrays(GL_LINES, NULL, 2);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        /* Drawing player */
        object_draw(&main_player.bobj);

        draw_text(-0.95,-0.9,"Coins:",DEBUG_TEXT_SIZE);
        draw_num(-0.8,-0.9,main_player.coins,DEBUG_TEXT_SIZE);

        draw_text(0.8, -0.9, "HP:",DEBUG_TEXT_SIZE);             
        draw_num(0.9, -0.9,main_player.hp,DEBUG_TEXT_SIZE);

    } else {
        draw_text(-0.25,0,"Thanks for Playing",0.01f);
        draw_text(-0.25,-0.25,"Collected coins:",0.01f);
        draw_num(0.25,-0.25,main_player.coins,0.01f);
        draw_text(-0.95,-0.9,"Press Esc for exit",0.005f);
    };

    /* Do stuff only when debug == 1 */
    if (debug)
        debug_draw();

    /*Fill screen with some sht*/
    flashscr();

    /*!!! END !!!*/

    /*Load matrix from memory*/
    glPopMatrix();
};

void MainUpdate()
{
    /*Update logic for game*/
    gameLogicUpdate();

    /*Draw frames*/
    drawUpdate();
}

/* FUCK IT */
void SetVSync(bool sync)
{
    typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

    const char *extensions = (char*)glGetString(GL_EXTENSIONS);

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (wglSwapIntervalEXT)
        wglSwapIntervalEXT(sync);
}

/* IDFK what happening here, idk winAPI :skull: */

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = GAME_CLASS_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    hwnd = CreateWindowEx(0,
                          GAME_CLASS_NAME,
                          WINDOW_NAME,
                          WS_OVERLAPPEDWINDOW,
                          0,
                          0,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    EnableOpenGL(hwnd, &hDC, &hRC);

    /*Load glad lib*/
    gladLoadGL();

    /*Init global variables*/
    initVar();

    /*idk*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*Enable transparency*/
    glEnable(GL_BLEND);

    glLineWidth(DEBUGLINESIZE);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(DEBUGPOINTSIZE);

    /* Enable VSYNC */
    SetVSync(vsync);

    /*Disable win cursor*/
    ShowCursor(false);

    /*Main loop*/
    while (!bQuit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /*idk*/
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {

            /*get time for timer*/
            mingw_gettimeofday(&start, NULL);

            /*Update game*/
            POINT raw_mouse_pos;
            GetCursorPos(&raw_mouse_pos);
            ScreenToClient(hwnd, &raw_mouse_pos);
            mouse_pos = vec2_init((raw_mouse_pos.x - (WINDOW_WIDTH / 2.0f)) / (WINDOW_WIDTH / 2.0f),
            -(raw_mouse_pos.y - (WINDOW_HEIGHT / 2.0f)) / (WINDOW_HEIGHT / 2.0f));

            MainUpdate();

            /*idk*/
            SwapBuffers(hDC);

            /*get time for timer*/
            mingw_gettimeofday(&stop, NULL);

            /*F it*/
            delta_time = ((stop.tv_sec - start.tv_sec) * 1000000.0f + stop.tv_usec - start.tv_usec)/1000.0f;
            fps_time += delta_time;
            if (fps_time > 2000) {
                printf("Fps: %f\n",fps);
                fps_time = 0;
            }

            fps = 1000 / fclamp(delta_time,0.0001,delta_time);
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY: {
            /*Disable sound engine*/
            ma_engine_uninit(&sound_engine);
            puts("Window Closed\nConsole will be killed in 5 seconds\n...\n");
            print_err(SUCCESS);
            Sleep(5);
            exit(SUCCESS);
        }
        /* Shitty code */
        case WM_SIZE: {
            
            printf("Windows size changed\n");
        } break;
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_UP: {
                    if (fclampb(main_player.speed.y, -0.01f * FPS_MULT, 0.01f * FPS_MULT)) {
                        if (debug) {
                            main_player.speed.y = -PLAYER_SPEED_X;
                        } else {
                            playSound(&sound_engine, "res/sounds/jump.wav");
                            main_player.speed.y = -JUMP_FORCE;
                        }
                    }
                } break;
                case VK_DOWN: {
                    if ((TARGET_GRAVITY==0.0f) || debug) {
                        main_player.speed.y = PLAYER_SPEED_X;
                    }
                } break;
                case VK_RIGHT: {
                    main_player.speed.x = -PLAYER_SPEED_X  / _ratio_;
                } break;
                case VK_LEFT: {
                    main_player.speed.x = PLAYER_SPEED_X / _ratio_;
                } break;
                case VK_ESCAPE: {
                    PostQuitMessage(0); 
                } break;
                case VK_F3: {
                    debug = !debug;
                } break;
                case VK_F1: {
                    GAME_OVER;
                } break;
                case VK_F5: {
                    init_level(Level);
                    end_screen = false;
                } break;
                case VK_F6: {
                    Level = (uint8_t)fclamp(Level-1, 0, MAXLEVELS-1);
                } break;
                case VK_F8: {
                    Level = (uint8_t)fclamp(Level+1, 0, MAXLEVELS-1);
                } break;
                case VK_F4: {
                    vsync = !vsync;
                    SetVSync(vsync);
                } break;
            }
        } break;
        case WM_KEYUP:
            switch (wParam) {
                case VK_RIGHT: {
                    main_player.speed.x = 0;
                } break;
                case VK_LEFT: {
                    main_player.speed.x = 0;
                } break;
                case VK_DOWN: {
                    main_player.speed.y = 0;
                } break;
                case VK_UP: {
                    if ((TARGET_GRAVITY == 0.0f) || debug) {
                        main_player.speed.y = 0;
                    }
                } break;
            } break;
        case WM_LBUTTONDOWN: {
            playSound(&sound_engine, "res/sounds/shoot.wav");
            for (uint8_t i = 0; i < MAXENEMYCOUNT; ++i) {
                if (enemy_giveDamage(&mouse_pos, &enemy_arr[i], &main_player.damage)) {
                    playSound(&sound_engine, "res/sounds/enemy_died.wav");
                    break;
                }
            }
        } break;
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }
    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    *hDC = GetDC(hwnd);

    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
