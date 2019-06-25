 
#include "global.h"
 /*
	����ʹ�ã�
	LED:   LED  PB9 ; 	
	
 
	
	���ڣ�RS485 (�ӵ�ѹ������)    UART1   PA9 PA10    con_pin1  PA8
	      RS485(ͨ��)            UART2   PA2  PA3    con_pin2  PA4
	  
  AC_CHECK  PA5    220V�Ƿ��ͨ����

  AC_con   PA12    220V �̵�������	 
*/	
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB�˿�ʱ��

	//BEEP
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				

	//LED
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_0;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_8|GPIO_Pin_4 ;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				

	RS485_CON2 = 0;
	RS485_CON1 = 0;//��ѹ������  һֱ����

	LED1 = 0;  
	LED2 = 0;  
	AC_con = 0; 
	BEEP = 0;
}
 
 
void EXTI9_5_IRQHandler(void)
{
	if( AC_CHECK == 0 )	 // 
	{
		POWER_220V_FLG = 1;
		LED1 =! LED1; 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line5);  //���LINE4�ϵ��жϱ�־λ  
}


void BSP_init(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB�˿�ʱ��

	//AC_CHECK
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	//GPIOA.5	  �ж����Լ��жϳ�ʼ������ �����ش��� PA5  WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 

	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 

	LED_Init();  
}
