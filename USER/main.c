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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(256000);	 	//���ڲ����ʳ�ʼ��Ϊ256000
 	LED_Init();			     //LED�˿ڳ�ʼ��
//	TPAD_Init();		//��ʼ����������
	LCD_Init();				//LCD��ʼ��
	KEY_Init();	 			//������ʼ��
	Audiosel_Init();	//��ʼ����Դѡ��
//	usmart_dev.init(72);//usmart��ʼ��
 	mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	
 	VS_Init();	  
	TIM3_PWM_Init(93,0);	 //����Ƶ��PWMƵ��=72000/94=765.957Khz
	TIM_SetCompare2(TIM3,70);  //��ʼ��ռ�ձ� 25%
 	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
  	f_mount(0,fs[0]); 		 		//����SD�� 
 	f_mount(1,fs[1]); 				//����FLASH.
	POINT_COLOR=BLUE;   
	   
 	while(font_init()){ 				//����ֿ�
		    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//�����ʾ	     
	}

	Show_Str(10,50,200,16,"ˮ�����⼤��ͨ��ϵͳ",16,0);				    	 
	Show_Str(80,70,200,16,"--���Ʒ��Ͷ�",16,0);				    	 
	// Show_Str(10,70,200,16,"WAV����������ʵ��",16,0);				    	 
	Show_Str(90,110,200,16,"--2015��1��16��",16,0);
	POINT_COLOR=RED;
	Show_Str(10,130,200,16,"KEY0:Wav_Exit",16,0);
	Show_Str(10,150,200,16,"KEY_UP:AGC+ KEY1:AGC-",16,0);
 	POINT_COLOR=BLUE;	
	Show_Str(10,170,200,16,"1-PWM_SET;  2-FUNC_TEXT;",16,0);	
	Show_Str(10,190,200,16,"3-FUNC_WAV; 4-FUNC_PIC;",16,0);
	Show_Str(10,210,200,16,"Function: 01",16,0); //90

	while(1)
	{
		Audiosel_Set(0);	//MP3ͨ��
 		LED1=0; 	  
// 		Show_Str(60,210,200,16,"�洢������...",16,0);
		VS_Ram_Test();	    
// 		Show_Str(60,210,200,16,"���Ҳ�����...",16,0); 	 	 
 		VS_Sine_Test();	   
//  Show_Str(60,210,200,16,"<<WAV¼����>>",16,0); 		 
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


