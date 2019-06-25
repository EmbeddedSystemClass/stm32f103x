 
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

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ��AFIO���ܵ�ʱ��(������)
		
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //������ӳ��
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //������ӳ��
	
	
 // RSR485_CON
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	 // LED  PA6
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	
	//key
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12 |GPIO_Pin_15  ;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 //��������
 GPIO_Init(GPIOA, &GPIO_InitStructure);				
	
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14   ;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 //��������
 GPIO_Init(GPIOB, &GPIO_InitStructure);		 

/* 
	ST_595    PB7
	SH_595    PB8
	MR_595    PB9
	DS_595    PB6  */

 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9 ;	  //  �˿�����
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);

   
 MR_595 = 0;
 MR_595 = 1;
   
 JOIN_LED = 1;
 
}
  
 

void BSP_init(void)
{
	LED_Init();  
}

