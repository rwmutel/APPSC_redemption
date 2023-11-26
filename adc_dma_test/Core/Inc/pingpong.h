//
// Created by vepbxer on 11/26/23.
//
#include "lcd5110.h"

#ifndef ADC_DMA_TEST_PINGPONG_H
#define ADC_DMA_TEST_PINGPONG_H
typedef enum state_t {
    START, R_SERVE, L_SERVE, R_WAIT, L_WAIT, R_TURN, L_TURN, UNSURE, END
} state_t;

extern volatile state_t state;
extern volatile state_t server;
extern volatile uint8_t l_score;
extern volatile uint8_t r_score;
extern volatile bool deuce;

void print_score(LCD5110_display *lcd_conf) {
    LCD5110_clear_scr(lcd_conf);
    LCD5110_set_cursor(1, 22, lcd_conf);
    LCD5110_printf(lcd_conf, BLACK, "L:%u", l_score);
    LCD5110_set_cursor(60, 22, lcd_conf);
    LCD5110_printf(lcd_conf, BLACK, "R:%u", r_score);
    if (deuce) {
        LCD5110_set_cursor(1, 40, lcd_conf);
        LCD5110_print("DEUCE", BLACK, lcd_conf);
    }
    LCD5110_refresh(lcd_conf);
}
void print_score_text(LCD5110_display *lcd_conf, const char *str) {
    print_score(lcd_conf);
    LCD5110_set_cursor(1, 40, lcd_conf);
    LCD5110_print(str, BLACK, lcd_conf);
    LCD5110_refresh(lcd_conf);
}

void update_score(state_t winner, LCD5110_display* lcd_conf) {
    if (winner == L_SERVE) {
        l_score++;
        if (r_score == 10 && l_score == 10) {
            deuce = true;
        } else if ((l_score == 11 && !deuce)
                   || (l_score - r_score > 1 && deuce)) {
            print_score_text(lcd_conf, "LEFT WON!");
            state = END;
        } else {
            print_score(lcd_conf);
            if (((l_score + r_score) % 2 == 0 && !deuce) || deuce) {
                state = (server == R_SERVE) ? L_SERVE : R_SERVE;
            } else {
                state = server;
            }
            server = state;
        }
    } else if (winner == R_SERVE) {
        r_score++;
        if (r_score == 10 && l_score == 10) {
            deuce = true;
        } else if ((r_score == 11 && !deuce)
                   || (r_score - l_score > 1 && deuce)) {
            print_score_text(lcd_conf, "RIGHT WON!");
            state = END;
        } else {
            print_score(lcd_conf);
            if (((l_score + r_score) % 2 == 0 && !deuce) || deuce) {
                state = (server == R_SERVE) ? L_SERVE : R_SERVE;
            } else {
                state = server;
            }
            server = state;
        }
    }
}

void print_current_state(LCD5110_display* lcd_conf) {
    LCD5110_set_cursor(0, 0, lcd_conf);
    switch (state) {
        case START:
            LCD5110_print("START\n", BLACK, lcd_conf);
            break;
        case L_SERVE:
            LCD5110_print("L_SERVE\n", BLACK, lcd_conf);
            break;
        case R_SERVE:
            LCD5110_print("R_SERVE\n", BLACK, lcd_conf);
            break;
        case L_WAIT:
            LCD5110_print("L_WAIT\n", BLACK, lcd_conf);
            break;
        case R_WAIT:
            LCD5110_print("R_WAIT\n", BLACK, lcd_conf);
            break;
        case L_TURN:
            LCD5110_print("L_TURN\n", BLACK, lcd_conf);
            break;
        case R_TURN:
            LCD5110_print("R_TURN\n", BLACK, lcd_conf);
            break;
    }
}

void switch_pp_state(LCD5110_display* lcd_conf) {
    // tablehit (left side)
    if (!HAL_GPIO_ReadPin(TABLEHIT_BTN_GPIO_Port, TABLEHIT_BTN_Pin)) {
        if (state == START) {
            state = L_SERVE;
            server = L_SERVE;
        } else if (state == L_SERVE) {
            state = L_WAIT;
        } else if (state == R_SERVE) {
            update_score(L_SERVE, lcd_conf);
        } else if (state == L_WAIT) {
            update_score(R_SERVE, lcd_conf);
        } else if (state == R_WAIT) {
            state = L_TURN;
        } else if (state == L_TURN) {
            update_score(R_SERVE, lcd_conf);
        } else if (state == R_TURN) {
            state = L_TURN;
        };
        print_current_state(lcd_conf);
    }
        // oppositehit (right side)
    else if (!HAL_GPIO_ReadPin(OPPOSITEHIT_BTN_GPIO_Port,
                               OPPOSITEHIT_BTN_Pin)) {
        if (state == START) {
            state = R_SERVE;
            server = state;
        } else if (state == R_SERVE) {
            state = R_WAIT;
        } else if (state == L_SERVE) {
            update_score(R_SERVE, lcd_conf);
        } else if (state == R_WAIT) {
            update_score(L_SERVE, lcd_conf);
        } else if (state == L_WAIT) {
            state = R_TURN;
        } else if (state == R_TURN) {
            update_score(L_SERVE, lcd_conf);
        } else if (state == L_TURN) {
            state = R_TURN;
        }
        print_current_state(lcd_conf);
    }
}
#endif //ADC_DMA_TEST_PINGPONG_H
