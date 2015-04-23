#include "led.h"
#include "delay.h"
#include "key.h"
#include "tpad.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "flash.h"
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "mmc_sd.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"
#include "string.h"
#include "usmart.h"
#include "fattester.h"
#include "piclib.h"
#include "vs10xx.h"
#include "mp3player.h"
#include "audiosel.h"
#include "recorder.h"
#include "timer.h"
//Edited by XMU-BMW-007	    2015-03-24
extern u8 Function_Select;
u8  len=0;
u16 t=0;


 int main(void)
 {	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(256000);	 	//串口波特率初始化为256000
 	LED_Init();			     //LED端口初始化
//	TPAD_Init();		//初始化触摸按键
	LCD_Init();				//LCD初始化
	KEY_Init();	 			//按键初始化
	Audiosel_Init();	//初始化音源选择
//	usmart_dev.init(72);//usmart初始化
 	mem_init(SRAMIN);	//初始化内部内存池	
 	VS_Init();	  
	TIM3_PWM_Init(93,0);	 //不分频。PWM频率=72000/94=765.957Khz
	TIM_SetCompare2(TIM3,70);  //初始化占空比 25%
 	exfuns_init();					//为fatfs相关变量申请内存  
  	f_mount(0,fs[0]); 		 		//挂载SD卡 
 	f_mount(1,fs[1]); 				//挂载FLASH.
	POINT_COLOR=BLUE;   
	   
 	while(font_init()){ 				//检查字库
		    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
	}

	Show_Str(10,50,200,16,"水下蓝光激光通信系统",16,0);				    	 
	Show_Str(80,70,200,16,"--调制发送端",16,0);				    	 
	// Show_Str(10,70,200,16,"WAV语音、传输实验",16,0);				    	 
	Show_Str(90,110,200,16,"--2015年1月16日",16,0);
	POINT_COLOR=RED;
	Show_Str(10,130,200,16,"KEY0:Wav_Exit",16,0);
	Show_Str(10,150,200,16,"KEY_UP:AGC+ KEY1:AGC-",16,0);
 	POINT_COLOR=BLUE;	
	Show_Str(10,170,200,16,"1-PWM_SET;  2-FUNC_TEXT;",16,0);	
	Show_Str(10,190,200,16,"3-FUNC_WAV; 4-FUNC_PIC;",16,0);
	Show_Str(10,210,200,16,"Function: 01",16,0); //90

	while(1)
	{
		Audiosel_Set(0);	//MP3通道
 		LED1=0; 	  
// 		Show_Str(60,210,200,16,"存储器测试...",16,0);
		VS_Ram_Test();	    
// 		Show_Str(60,210,200,16,"正弦波测试...",16,0); 	 	 
 		VS_Sine_Test();	   
//  Show_Str(60,210,200,16,"<<WAV录音机>>",16,0); 		 
		LED1=1;
		switch(USART_Func_select)
		{		
			case FUNC_PIC:	
				LED1=0;						
				break;	
			case FUNC_WAV:	
				printf("WAVE WAVE WAVE ");
				recoder_play();					
				break;	
			case FUNC_TEXT:	
				LED1=1;	
				while(Usart_Send_EN>0){					
					printf("TEXT:Receive_Info:  ");
					USART_SendStr(USART_RX_BUF,(USART_RX_STA)&0X3FFF); 
					printf("\r\n\r\n");	
					delay_ms(200);
					printf("TEXT:Receive_Info_COPY:  ");
					USART_SendStr(USART_RX_BUF,(USART_RX_STA)&0X3FFF); 
					printf("\r\n\r\n");
					USART_RX_STA = 0;
					Usart_Send_EN=0;					
				}		
				delay_ms(10);
				LED1=0;				
				break;	
			case PWM_SET:	
				LED1=1;					
				break;				
		}		
	}    					  
}


