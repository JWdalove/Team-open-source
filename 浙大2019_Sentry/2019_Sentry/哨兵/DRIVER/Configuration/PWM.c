#include "PWM.h"
void PWM_Configuration(void)
{
    GPIO_InitTypeDef          gpio;
    TIM_TimeBaseInitTypeDef   tim;
    TIM_OCInitTypeDef         oc;


    /***************  TIM2 Configuration  ***************

    	  Òý½Å£ºPA0,PA1   Í¨µÀ£ºTIM2_CH1|2  -->Ä¦²ÁÂÖ

    *****************************************************/

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&gpio);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0, GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1, GPIO_AF_TIM2);


    tim.TIM_Prescaler = 42-1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 10000;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&tim);

    oc.TIM_OCMode = TIM_OCMode_PWM2;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OutputNState = TIM_OutputState_Disable;
    oc.TIM_Pulse = 0;
    oc.TIM_OCPolarity = TIM_OCPolarity_Low;
    oc.TIM_OCNPolarity = TIM_OCPolarity_High;
    oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
    oc.TIM_OCNIdleState = TIM_OCIdleState_Set;

    TIM_OC1Init(TIM2,&oc);
    TIM_OC2Init(TIM2,&oc);
    TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2,ENABLE);
    TIM_CtrlPWMOutputs(TIM2,ENABLE);
    TIM_Cmd(TIM2,ENABLE);
}


void ECS_Init()
{
    TIM2->CCR1 = 2000;
    TIM2->CCR2 = 2000;
}
