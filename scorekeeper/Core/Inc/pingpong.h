//
// Created by vepbxer on 11/26/23.
//
#include "stdbool.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"

#ifndef ADC_DMA_TEST_PINGPONG_H
#define ADC_DMA_TEST_PINGPONG_H

#define PP_TIMEOUT_TICKS 1000

typedef enum state_t {
    START, R_SERVE, L_SERVE, R_WAIT, L_WAIT, R_TURN, L_TURN, UNSURE, END
} state_t;

extern volatile state_t state;
extern volatile state_t server;
extern volatile uint8_t l_score;
extern volatile uint8_t r_score;
extern volatile bool deuce;

void print_score(char* text) {
    char buf[50];
    sprintf(buf, "%d, %d, %s\r\n", l_score, r_score, text);
    HAL_UART_Transmit(&huart1, (uint8_t *) buf, strlen(buf), 100);
}

void update_score(state_t winner) {
    if (winner == L_SERVE) {
        l_score++;
        if (r_score == 10 && l_score == 10) {
            deuce = true;
        } else if ((l_score == 11 && !deuce)
                   || (l_score - r_score > 1 && deuce)) {
            state = END;
            print_score("Left player wins!");
        } else {
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
            state = END;
            print_score("Right player wins!");
        } else {
            if (((l_score + r_score) % 2 == 0 && !deuce) || deuce) {
                state = (server == R_SERVE) ? L_SERVE : R_SERVE;
            } else {
                state = server;
            }
            server = state;
        }
    }
}

void print_current_state() {
    switch (state) {
        case START:
            print_score("Game started");
            break;
        case L_SERVE:
            print_score("Left player serves");
            break;
        case R_SERVE:
            print_score("Right player serves");
            break;
        case L_WAIT:
            print_score("Waiting on right court");
            break;
        case R_WAIT:
            print_score("Waiting on left court");
            break;
        case L_TURN:
            print_score("Left returns");
            break;
        case R_TURN:
            print_score("Right returns");
            break;
    }
}

void switch_pp_state(bool tablehit, bool oppositehit) {
    // tablehit (left side)
    if (tablehit) {
        if (state == START) {
            state = L_SERVE;
            server = L_SERVE;
        } else if (state == L_SERVE) {
            state = L_WAIT;
        } else if (state == R_SERVE) {
            update_score(L_SERVE);
        } else if (state == L_WAIT) {
            update_score(R_SERVE);
        } else if (state == R_WAIT) {
            state = L_TURN;
        } else if (state == L_TURN) {
            update_score( R_SERVE);
        } else if (state == R_TURN) {
            state = L_TURN;
        };
    }
    // oppositehit (right side)
    else if (oppositehit) {
        if (state == START) {
            state = R_SERVE;
            server = state;
        } else if (state == R_SERVE) {
            state = R_WAIT;
        } else if (state == L_SERVE) {
            update_score( R_SERVE);
        } else if (state == R_WAIT) {
            update_score( L_SERVE);
        } else if (state == L_WAIT) {
            state = R_TURN;
        } else if (state == R_TURN) {
            update_score(L_SERVE);
        } else if (state == L_TURN) {
            state = R_TURN;
        }
    }
    print_current_state();
}

void check_timeout(state_t init_state) {
    if (init_state == R_WAIT && init_state == state) {
        update_score( L_SERVE);
    } else if (init_state == L_WAIT && init_state == state) {
        update_score(R_SERVE);
    } else if (init_state == R_TURN && init_state == state) {
        update_score(L_SERVE);
    } else if (init_state == L_TURN && init_state == state) {
        update_score(R_SERVE);
    }
    print_current_state();
}
#endif //ADC_DMA_TEST_PINGPONG_H
