#include "audiosel.h"	  

//声音初始化
void Audiosel_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB端口时钟
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//PB.7 推挽输出 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure); //PD.7 推挽输出
}
//设置4052的选择通道
//声音通道选择
//0//MP3通道
//1//收音机通道
//2//PWM音频通道
//3//无声
void Audiosel_Set(u8 ch)
{
	AUDIO_SELA=ch&0X01;
 	AUDIO_SELB=(ch>>1)&0X01;	 
}
















