/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		peripheral.c
����   :      ���贫�������������ļ�
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.25
************************************************************************************************************************/

/*************HEADS**********/
#include "peripheral.h"
/***************************/

/* ȫ�ֱ��� */
/****************/

/**************************���ģ��HC_SR04***************************/
/***************************************************************************************************************
*��������		HcSr04_Init
*������		��ഫ������ʼ��			//�����PA12   	���룺PA11
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25 09:40
*�汾��¼��	
****************************************************************************************************************/
void HcSr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(SR04_TRIG_PERIPH, ENABLE);
	//ʱ��ʹ��

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = SR04_TRIG_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(SR04_TRIG_PORT, &GPIO_InitStruct);
	//50MHzͨ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = SR04_ECHO_PIN;
	GPIO_Init(SR04_ECHO_PORT, &GPIO_InitStruct);
	//����

	TIM1_Init();
	//���벶��ʱ�����ó�ʼ��
}

/***************************************************************************************************************
*��������		HcSr04Start
*������		��ഫ�������Ϳ�ʼ			//�����PA12   	���룺PA11
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25 13:12
*�汾��¼��	
****************************************************************************************************************/
void HcSr04Start(void)
{
	Tim1CaptureState=0;			//�����־λ�����λ���Ա��ܹ���Ӧ�������Ĳ����жϺ�����жϣ�������һ�εĲ���
	HcSr04StartF = TRUE;		//��ʼ��־��λ
	SR04_TRIG_VALUE() = 1;		//����ߵ�ƽ
	delay_us(20);				//��ʱ20us
	SR04_TRIG_VALUE() = 0;		//����͵�ƽ
	TIM_Cmd(TIM1,ENABLE);		//ʹ��TIM1
}
void HcSr04_OFF(void)
{
	HcSr04StartF = FALSE;		//��ʼ��־��λ
	
	TIM_Cmd(TIM1,DISABLE);		//ʹ��TIM1
}

/***************************************************************************************************************
*��������	GetHcSr04Value
*������		��ȡ��ഫ����
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	u32 value����������ֵ (value==0��˵����ȡ����)
*��д���ڣ�	2019.3.25 13:17
*�汾��¼��	
****************************************************************************************************************/
u32 GetHcSr04Value(void)
{
	u32 value = 0;
	
	if((Tim1CaptureState&0x80) == 0)	//��־λ���λ��Ϊ1��˵�����û��������
	{
		return value;
	}

	value = Tim1CaptureState&0x3F;	//��ȡ�������
	value *= 1000;					//�õ����ֵ��Ӧ�ļ���ֵ(us)
	value += Tim1CaptureCount;		//�ټ���û������ļ���ֵ(us)
	value = value*340/2/1000000;	//�õ�����ֵ  		l=t*340/2(m)
	
	TIM_Cmd(TIM1,DISABLE);			//ʧ��TIM1
	Tim1CaptureState=0;				//�����־λ�����λ���Ա��ܹ���Ӧ�������Ĳ����жϺ�����жϣ�������һ�εĲ���

	return value;
}

/**************************����ģ��HX711***************************/
/***************************************************************************************************************
*��������		HX711_Init
*������		���ش�������ʼ��
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25 15:12
*�汾��¼��	
****************************************************************************************************************/
void HX711_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(HX711_SCK_PERIPH, ENABLE);
	//ʱ��ʹ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = HX711_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(HX711_SCK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = HX711_DAT_PIN;
	GPIO_Init(HX711_DAT_PORT, &GPIO_InitStruct);
}

/***************************************************************************************************************
*��������		Read_HX711
*������		��ȡ���ش�����ADֵ
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25 15:42
*�汾��¼��	
****************************************************************************************************************/
u32 Read_HX711(void)             
{ 
	u32 HX711_value = 0; 
	u8 i = 0; 
	u32 count = 0xfff;
	GPIO_SetBits(HX711_DAT_PORT,HX711_DAT_PIN);			//DOUT=1 
	GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN);		//SCK=0 
	while(GPIO_ReadInputDataBit(HX711_DAT_PORT,HX711_DAT_PIN))//DOUT=0  
	{
		if(count < 10)
		{
			return 0;
		}
		else
		{
			delay_us(1); 
		}
		count --;
	}
	for(i=0;i<24;i++) 
	{ 
	   GPIO_SetBits(HX711_SCK_PORT,HX711_SCK_PIN);		//SCK=1 
	   HX711_value = HX711_value<<1;					//����
	   delay_us(1);  
	   GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN);	//SCK=0 
	   if(GPIO_ReadInputDataBit(HX711_DAT_PORT,HX711_DAT_PIN))//DOUT=1 
	      HX711_value++; 
	   delay_us(1); 
	} 
	GPIO_SetBits(HX711_SCK_PORT,HX711_SCK_PIN); 
	delay_us(1); 
	GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN); 
	delay_us(1);  
	return HX711_value;  
} 

/***************************************************************************************************************
*��������	Relay_Init
*������		�̵�����ʼ��
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	��
*��д���ڣ�	2019.3.29 11:29
*�汾��¼��	
****************************************************************************************************************/
void Relay_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RELAY_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin  = RELAY_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_2MHz;

	GPIO_Init(RELAY_PORT, &GPIO_InitStruct);

	PAout(15) = 0;
}

