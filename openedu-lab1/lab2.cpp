//
// Created by Женя on 02.06.2023.
//

#include "hal.h"

int lamps[8] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};
int animation[8][8] = {{0, 0, 0, 1, 1, 0, 0, 0},
                       {0, 0, 1, 1, 1, 1, 0, 0},
                       {0, 1, 1, 1, 1, 1, 1, 0},
                       {1, 1, 1, 1, 1, 1, 1, 1},
                       {0, 1, 1, 1, 1, 1, 1, 0},
                       {0, 0, 1, 1, 1, 1, 0, 0},
                       {0, 0, 0, 1, 1, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};
const int TIME_INIT = 500;
const int TIME_DELTA = 50;
unsigned int switches[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

int sw_cur = 0;
int state = 0;

int match_sw_to_number() {
    int res = 0;
    int mul = 8;
    for (int i = 0; i < 4; i++) {
        if (HAL_GPIO_ReadPin(GPIOE, switches[i]) == GPIO_PIN_SET) {
            res += mul;
        }
        mul /= 2;
    }
    return res;
}

void set_animation_state(int state) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], animation[state][i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void TIM6_IRQ_Handler()
{
    set_animation_state(state);
    state = (state + 1) % 8;
}

void TIM7_IRQ_Handler()
{
    sw_cur = match_sw_to_number();
    WRITE_REG(TIM6_ARR, TIME_INIT + TIME_DELTA * sw_cur);
}


int umain() {

    registerTIM6_IRQHandler(TIM6_IRQ_Handler);
    registerTIM7_IRQHandler(TIM7_IRQ_Handler);

    __enable_irq();

    WRITE_REG(TIM6_ARR, TIME_INIT);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);
    WRITE_REG(TIM7_ARR, 100);
    WRITE_REG(TIM7_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM7_PSC, 0);

    // turn on timers
    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
    WRITE_REG(TIM7_CR1, TIM_CR1_CEN);

    return 0;
}