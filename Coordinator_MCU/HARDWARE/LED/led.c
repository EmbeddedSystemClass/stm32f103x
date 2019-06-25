#include "led.h"
#include "delay.h"
 
#include  "global.h"
 
 
 
 
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB�˿�ʱ��


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ��AFIO���ܵ�ʱ��(������)

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //������ӳ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //������ӳ��


	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|  GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_12;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			 


	ZIGBEE_RST = 1;

	LED_232 = 0;
	LED_RFID = 0;
	
}
 

void  BEEP_INIT(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_13;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  


	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	JDQ_PIN = 0;

	LED_HW = 1;

	RS485_con = 0; //485
	RS485_con = 0; //485
}	


void  EXTI_START(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	  //  �˿�����
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		  
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	IR_TX = 0;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);

	EXTI_InitStructure.EXTI_Line = EXTI_Line6;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 

}


