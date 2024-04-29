#ifndef MACRO_FUNCTIONS_H_INCLUDED
#define MACRO_FUNCTIONS_H_INCLUDED
    #define FPS_MULT (TARGET_FPS / fps)

    #define GAME_OVER ({                                \
        main_player.hp = 127;                           \
        flash(1.0f,0.0f,0.0f,1.0f);                     \
        playSound(&sound_engine,"res/sounds/death.wav");    \
        reset_pos();                                    \
        })

    #define GRAVITYF ({                                 \
        gravity = TARGET_GRAVITY * FPS_MULT;  \
        main_player.speed.y += gravity * !debug;        \
        })

    #define DEBUG_TEXT_DRAWING ({                               \
        draw_text(-0.99, 1,"FPS:",DEBUG_TEXT_SIZE);             \
        if (fps>255) {                                          \
            draw_text(-0.9, 1, ">255", DEBUG_TEXT_SIZE);        \
        } else {                                                \
            draw_num(-0.9, 1,(unsigned char)fps,DEBUG_TEXT_SIZE);   \
        }                                                       \
        draw_text(-0.99, 0.9,"Frame_Time:",DEBUG_TEXT_SIZE);    \
        draw_num(-0.8, 0.9,delta_time,DEBUG_TEXT_SIZE);         \
                                                                \
        draw_text(0.6,1,"Build Date:",DEBUG_TEXT_SIZE);         \
        draw_text(0.8,1,BUILD_DATE,DEBUG_TEXT_SIZE);            \
        draw_text(0.8,0.9,VERSION,DEBUG_TEXT_SIZE);  \
        draw_text(0.9,0.9,VERSION_STATUS,DEBUG_TEXT_SIZE);        \
        draw_text(0.8,0.8,"Level:",DEBUG_TEXT_SIZE);            \
        draw_num(0.9,0.8,Level,DEBUG_TEXT_SIZE);                \
        draw_text(0.8, -0.8,"X:",DEBUG_TEXT_SIZE);              \
        draw_num(0.9, -0.8,(unsigned char)object_arr[0].pos.x,DEBUG_TEXT_SIZE);\
        draw_text(0.8, -0.9, "Y:",DEBUG_TEXT_SIZE);             \
        draw_num(0.9, -0.9,(unsigned char)object_arr[0].pos.y,DEBUG_TEXT_SIZE);\
        })

    #define BASIC_PRINT ({                                          \
            static char buffer[9999];                               \
            int num_quads;                                          \
                                                                    \
            num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));    \
                                                                    \
            glColor3f(0,0,0);                                       \
            glEnableClientState(GL_VERTEX_ARRAY);                   \
            glVertexPointer(2, GL_FLOAT, 16, buffer);               \
            glDrawArrays(GL_QUADS, 0, num_quads*4);                 \
            glDisableClientState(GL_VERTEX_ARRAY);                  \
        })                                                                                          
#endif // MACRO_FUNCTIONS_H_INCLUDED
