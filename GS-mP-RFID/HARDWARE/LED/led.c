#include "led.h"
#include "delay.h"
 
#include  "global.h"
 
 
 
 
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

 
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ��AFIO���ܵ�ʱ��(������)
		
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //������ӳ��
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //������ӳ��
	

 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					  
 
  MR_595=0;
  MR_595=1;


 	RS485_con=0; //485
	RS485_con=0; //485
	
	
}
 

void  BEEP_INIT(void) 
{
	
  
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB�˿�ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB�˿�ʱ��
  
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_15;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					  

 
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
  BEEP_OUT=0;
  LED_RFID=0;

  RS485_LED=0;

	
}	

 




