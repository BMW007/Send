#include "sys.h"
#include "usart.h"	 
#include "stm32f10x_tim.h"
#include "timer.h"
#include "led.h"
#include "lcd.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮������������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

//Use USART_x Send Str
void USART_SendStr(char *SendStr,int len)
 {   
	 int t1;
	 for(t1=0;t1<len;t1++)
	 {
		 USART_SendData(USART1, SendStr[t1]);	//�򴮿�1��������
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);  //�ȴ����ͽ���
	 }	 
 }
//Use USART_x Send Str

void USART_SendStr_Enter(char *SendStr,int len)
 {   
	 int t1=0;
	 for(t1=0;t1<len;t1++)
	 {
		 USART_SendData(USART1, SendStr[t1]);	//�򴮿�1��������
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);  //�ȴ����ͽ���
	 }
	 USART_SendData(USART1,0x0D);//�򴮿�1��������
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
 }
	static	u16 Fre=768;
	static	u8 ZKB=70;
	u8 USART_Func_select = 3;
	u8 Usart_Send_EN=0;
void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Usart_Send_EN=1;
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
 		USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
		if(USART_RX_BUF[USART_RX_STA]=='+' && USART_RX_BUF[USART_RX_STA-1] =='N' && USART_RX_BUF[USART_RX_STA-2] =='G' ){
			USART_Func_select++;
			if(USART_Func_select>FUNC_PIC){
				USART_Func_select=PWM_SET;
			}
			USART_RX_STA=0;
			LCD_ShowxNum(90,210,USART_Func_select,2,16,0X80);
		}
//ֻ�üӷ������������ƹ���ѡ���ˡ�	�Ż���	
// 		if(USART_RX_BUF[USART_RX_STA]=='-' && USART_RX_BUF[USART_RX_STA-1] =='N' && USART_RX_BUF[USART_RX_STA-2] =='G' ){
// 			USART_Func_select--;
// 			if(USART_Func_select<PWM_SET){
// 				USART_Func_select=FUNC_PIC;
// 			}
// 			LCD_ShowxNum(90,210,USART_Func_select,2,16,0X80);
// 		}
			if(USART_Func_select == PWM_SET){
				if(USART_RX_BUF[(USART_RX_STA)&0X3FFF] =='R' && USART_RX_BUF[(USART_RX_STA-1)&0X3FFF] =='+'){
						ZKB=ZKB+1;
						SetPWM_Fre(72000/Fre-1,0);	 //��Ƶ
						TIM_SetCompare2(TIM3,ZKB);  //ռ�ձ�
						LED1= !LED1; 	
				}	
				else if(USART_RX_BUF[(USART_RX_STA)&0X3FFF] =='R' && USART_RX_BUF[(USART_RX_STA-1)&0X3FFF] =='-'){
						ZKB=ZKB-1;
						SetPWM_Fre(72000/Fre-1,0);	
						TIM_SetCompare2(TIM3,ZKB);  //ռ�ձ� 
						LED1= !LED1; 	
				}	
				else if(USART_RX_BUF[(USART_RX_STA)&0X3FFF] =='F' && USART_RX_BUF[(USART_RX_STA-1)&0X3FFF] =='+'){
						Fre=Fre+2;
						SetPWM_Fre(72000/Fre-1,0);	 
						TIM_SetCompare2(TIM3,ZKB);  //ռ�ձ� 
						LED1= ~LED1; 	
				}		
				else if(USART_RX_BUF[(USART_RX_STA)&0X3FFF] =='F' && USART_RX_BUF[(USART_RX_STA-1)&0X3FFF] =='-'){
						Fre=Fre-2;
						SetPWM_Fre(72000/Fre-1,0);	
						TIM_SetCompare2(TIM3,ZKB);  
						LED1= ~LED1; 	
				}
			}
// 		if(USART_Func_select == FUNC_TEXT){
// 			USART_RX_STA|=0x8000;
// 		}
		USART_RX_STA++;
		if(USART_RX_STA>(USART_REC_LEN)) USART_RX_STA=0;//�������ݴ���,���¿�ʼ����			
     } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	
