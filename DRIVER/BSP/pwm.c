#include "stm32f10x.h"                  // Device header
#include <stdint.h>

void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // TIM2_CH1
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        TIM_InternalClockConfig(TIM2);

        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_Period = 499;     //ARR
        TIM_TimeBaseInitStructure.TIM_Prescaler = 71;   //PSC
        TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

        TIM_OCInitTypeDef TIM_OCInitStructure;
        TIM_OCStructInit(&TIM_OCInitStructure);
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse = 0;  //CCR
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);

        TIM_Cmd(TIM2, ENABLE);
    }

    // TIM3_CH1
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        TIM_InternalClockConfig(TIM3);

        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_Period = 499;     //ARR
        TIM_TimeBaseInitStructure.TIM_Prescaler = 71;   //PSC
        TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

        TIM_OCInitTypeDef TIM_OCInitStructure;
        TIM_OCStructInit(&TIM_OCInitStructure);
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse = 0;  //CCR
        TIM_OC1Init(TIM3, &TIM_OCInitStructure);

        TIM_Cmd(TIM3, ENABLE);
    }
}

void PWM_SetCompare1(uint8_t No, uint16_t Compare)
{
    if (No == 0) {
	    TIM_SetCompare1(TIM2, Compare);
    } else {
	    TIM_SetCompare1(TIM3, Compare);
    }
}

void PWM_SetAutoreload(uint8_t No, uint16_t Autoreload)
{
    if (No == 0) {
        TIM_SetAutoreload(TIM2, Autoreload);
    } else {
        TIM_SetAutoreload(TIM3, Autoreload);
    }
}