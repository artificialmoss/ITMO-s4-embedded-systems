//
// Created by Женя on 02.06.2023.
//

#include "hal.h"

int lamps[8] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};
int animation[8][8] = {{1, 1, 0, 0, 0, 0, 0, 1},
                       {0, 1, 1, 0, 0, 0, 1, 0},
                       {0, 0, 1, 1, 0, 1, 0, 0},
                       {0, 0, 0, 1, 1, 0, 0, 0},
                       {0, 0, 0, 1, 1, 1, 0, 0},
                       {0, 0, 1, 0, 0, 1, 1, 0},
                       {0, 1, 0, 0, 0, 0, 1, 1},
                       {1, 0, 0, 0, 0, 0, 0, 1}};
const int ANIMATION_DELAY = 500;
unsigned int switches[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

bool check_code() {
    return (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET &&
           HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET);
}

void match_lamps_to_sw() {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], HAL_GPIO_ReadPin(GPIOE, switches[i]));
    }
    for (int i = 4; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], GPIO_PIN_RESET);
    }
}

void set_animation_state(int state) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], animation[state][i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void green() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void yellow() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void red() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

bool check_btn() {
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET;
}

int umain() {
    int state = 0;
    bool button = false;
    while (true) {
        if (check_code()) {
            button = check_btn() ^ button;
            if (!button) {
                green();
                set_animation_state(state);
                state = (state + 1) % 8;
                HAL_Delay(ANIMATION_DELAY);
            } else {
                red();
                HAL_Delay(500);
            }
        } else {
            match_lamps_to_sw();
            yellow();
            state = 0;
            button = false;
        }
    }

    return 0;
}