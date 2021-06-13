#include "GPIO.h"

void Switch_Init()
{
    GPIO_InitTypeDef  gpio;
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE);
//    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_14;		//∫ÏÕ‚
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &gpio);

}

void Laser_Init()
{
	GPIO_InitTypeDef gpio1;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	gpio1.GPIO_Pin = GPIO_Pin_1 ;
	gpio1.GPIO_Mode = GPIO_Mode_OUT;
	gpio1.GPIO_OType = GPIO_OType_PP;
	gpio1.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&gpio1);
	GPIO_SetBits(GPIOE, GPIO_Pin_1);
}

