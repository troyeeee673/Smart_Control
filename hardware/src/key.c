//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "key.h"
#include "delay.h"
#include "led.h"



void Key_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//打开GPIOB的时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;				//设置为输出
	gpio_initstruct.GPIO_Pin = GPIO_Pin_1;						//将初始化的Pin脚
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	
	GPIO_Init(GPIOA, &gpio_initstruct);							//初始化GPIO
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);//设置外部中断
	
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructrue;
	
	NVIC_InitStructrue.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructrue.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructrue.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructrue.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStructrue);
}

void EXTI1_IRQHandler(void)
{
	DelayMs(10);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == (BitAction) 0)
	{
		if(led_info.Led_Status== LED_ON) Led_Set(LED_OFF);
		else
			Led_Set(LED_ON);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
	
}
