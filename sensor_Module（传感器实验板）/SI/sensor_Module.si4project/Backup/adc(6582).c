/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		adc.c
����   :      adc���������ļ�
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.22
************************************************************************************************************************/

/*************HEADS**********/
#include "adc.h"
/***************************/

/* ȫ�ֱ��� */
/****************/

/***************************************************************************************************************
*��������		Adc_Config
*������		ADC��ʼ��		//PA5 PA6 PA7
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.19
*�汾��¼��	
****************************************************************************************************************/
void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;

	/*1.��ʱ��*/
	RCC_APB2PeriphClockCmd(HD_136_PERIPH|RCC_APB2Periph_ADC1|RCC_AHBPeriph_DMA1, ENABLE);
	//ʹ��GPIOA��ADC1��DMA1��ʱ��

	/*2.GPIO����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin  = FAN_TAC_PIN|HD_136_PIN|RP_C18_3_PIN;
	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
	//��ʼ��PA5��PA6��PA7����ģ������ģʽ					//PA5-FAN_TAC(����) PA6 HD_136(ˮλ) PA7 RP_C18_3(ѹ��)

	/*3.ADC����*/
	ADC_DeInit(ADC1);	//��λADC1
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	//����ģʽ
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	//ɨ��ģʽ
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	//����ת��ģʽ
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//�������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	//�����Ҷ���
	ADC_InitStruct.ADC_NbrOfChannel = 3;
	//ͨ������3
	ADC_Init(ADC1, &ADC_InitStruct);
	//��ʼ��ADC1
	
	/*4.���ù���ͨ������˳���ʱ�� PA5-Channel_5 PA6-Channel_6 PA7-Channel_7*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
	//ͨ��5����һ������Ŀ�꣬����55.5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
	//ͨ��6���ڶ�������Ŀ�꣬����55.5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
	//ͨ��7������������Ŀ�꣬����55.5

	/*5.����ADC��У׼*/
	ADC_Cmd(ADC1,ENABLE);
	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);
	//ʹ�ܸ�λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1));
	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);
	//����ADCУ׼
	while(ADC_GetCalibrationStatus(ADC1));
	//�ȴ�ADCУ׼����
	
	/*5.DMA����*/
	DMA_DeInit(DMA1_Channel1);//DMA1��ͨ��1��λ

	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	//DMA�������ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)AdcBuf;
	//DMA�ڴ����ַ
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	//DMA���ݴ��䷽�򡪡�������Ϊ������Դ
	DMA_InitStruct.DMA_BufferSize = 3;
	//���仺���С��HD_136 RP_C18_3��
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//�����ַ�����ر�
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//�ڴ��ַ������
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	//�������ݴ�С-���֣�16λ��
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//�ڴ����ݴ�С
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	//ѭ��ģʽ
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	//ͨ�������ȼ�������
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	//������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	//���ݲ�����ʼ��DMA1��ͨ��1
		
	ADC_DMACmd(ADC1,ENABLE);		//ʹ��ADC��DMA����
	DMA_Cmd(DMA1_Channel1,ENABLE);	//ʹ��ADC1_DMA��ָʾ��ͨ��
	
	// /*6.DMA�ж�����*/
	// NVIC_InitTypeDef NVIC_InitStruct;
	// NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;//�ж�ͨ��
	// NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	// NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�
	// NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStruct);
	
	// DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//����DMA�Ĵ�������ж�
}



