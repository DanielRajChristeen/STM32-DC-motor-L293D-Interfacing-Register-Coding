#include "stm32f446xx.h"

void delay_ms(uint32_t ms);

/* ===== MOTOR CONTROL ===== */
void motor_forward(void)
{
    GPIOA->ODR |=  (1 << 6);   // IN1 = 1
    GPIOA->ODR &= ~(1 << 7);   // IN2 = 0
}

void motor_reverse(void)
{
    GPIOA->ODR &= ~(1 << 6);   // IN1 = 0
    GPIOA->ODR |=  (1 << 7);   // IN2 = 1
}

void motor_stop(void)
{
    TIM2->CCR1 = 0;            // PWM duty = 0
    GPIOA->ODR &= ~(1 << 6);
    GPIOA->ODR &= ~(1 << 7);
}

/* ===== SPEED CONTROL ===== */
void speed_up(void)
{
    for (uint16_t duty = 0; duty <= 1000; duty += 50)
    {
        TIM2->CCR1 = duty;
        delay_ms(50);
    }
}

void speed_down(void)
{
    for (int duty = 1000; duty >= 0; duty -= 50)
    {
        TIM2->CCR1 = duty;
        delay_ms(50);
    }
}

int main(void)
{
    /* ===== CLOCK ENABLE ===== */
    RCC->AHB1ENR |= (1 << 0);    // GPIOA EN
    RCC->APB1ENR |= (1 << 0);    // TIM2 EN

    /* ===== GPIO CONFIG ===== */
    // PA5 → AF (TIM2_CH1)
    GPIOA->MODER &= ~(3 << (5 * 2));
    GPIOA->MODER |=  (2 << (5 * 2));

    // PA6, PA7 → Output
    GPIOA->MODER &= ~(3 << (6 * 2));
    GPIOA->MODER |=  (1 << (6 * 2));

    GPIOA->MODER &= ~(3 << (7 * 2));
    GPIOA->MODER |=  (1 << (7 * 2));

    // PA5 AF1 (TIM2)
    GPIOA->AFR[0] |= (1 << (5 * 4));

    /* ===== TIMER CONFIG ===== */
    TIM2->PSC = 84 - 1;         // 84 MHz → 1 MHz
    TIM2->ARR = 1000 - 1;       // 1 kHz PWM

    // PWM Mode 1 on CH1
    TIM2->CCMR1 |= (6 << 4);    // OC1M = 110
    TIM2->CCMR1 |= (1 << 3);    // OC1PE enable

    TIM2->CCER |= (1 << 0);     // CH1 enable
    TIM2->CR1  |= (1 << 0);     // TIM2 enable

    while (1)
    {
        motor_forward();
        speed_up();
        speed_down();
        motor_stop();
        delay_ms(1000);

        motor_reverse();
        speed_up();
        speed_down();
        motor_stop();
        delay_ms(1000);
    }
}

/* ===== SIMPLE DELAY ===== */
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < (ms * 4000); i++)
        __NOP();
}
