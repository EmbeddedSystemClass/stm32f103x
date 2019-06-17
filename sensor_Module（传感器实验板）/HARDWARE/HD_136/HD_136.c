/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		HD-136.c
����   :      HD-136Һλ������ģ������ļ�
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.21
************************************************************************************************************************/

/*************HEADS**********/
#include "HD_136.h"
/***************************/

/* ȫ�ֱ������� */
/****************/

/***************************************************************************************************************
*��������	HD_136_GpioConfig
*������		HD_136��GPIO����		
			//--PA6
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.19
*�汾��¼��	
****************************************************************************************************************/
void HD_136_GpioConfig(void)
{
	RCC_APB2PeriphClockCmd(HD_136_PERIPH, ENABLE);
	//��GPIOA��ʱ��

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin  = HD_136_PIN;
	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
	//��ʼ��HD_136��GPIO(ģ������)
}

/***************************************************************************************************************
*��������		ADC1_Config
*������		ADC1����		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.19
*�汾��¼��	
****************************************************************************************************************/
void ADC1_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	//����ģʽ
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	//��ɨ��ģʽ
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	//����ת��ģʽ
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//�������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	//�����Ҷ���
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	//ͨ������
}

