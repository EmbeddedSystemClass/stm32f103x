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
//void Adc_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	ADC_InitTypeDef ADC_InitStruct;
////	DMA_InitTypeDef DMA_InitStruct;
//
//	/*1.��ʱ��*/
//	RCC_APB2PeriphClockCmd(HD_136_PERIPH|RCC_APB2Periph_ADC1|RCC_AHBPeriph_DMA1, ENABLE);
//	//ʹ��GPIOA��ADC1��DMA1��ʱ��
//
//	/*2.GPIO����*/
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_InitStruct.GPIO_Pin  = FAN_TAC_PIN|HD_136_PIN|RP_C18_3_PIN;
//	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
//	//��ʼ��PA5��PA6��PA7����ģ������ģʽ					//PA5-FAN_TAC(����) PA6 HD_136(ˮλ) PA7 RP_C18_3(ѹ��)
//
//	/*3.ADC����*/
//	ADC_DeInit(ADC1);	//��λADC1
//	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
//	//����ģʽ
//	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
//	//ɨ��ģʽ
//	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
//	//����ת��ģʽ
//	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//	//�������
//	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//	//�����Ҷ���
//	ADC_InitStruct.ADC_NbrOfChannel = 3;
//	//ͨ������3
//	ADC_Init(ADC1, &ADC_InitStruct);
//	//��ʼ��ADC1
//	
//	/*4.���ù���ͨ������˳���ʱ�� PA5-Channel_5 PA6-Channel_6 PA7-Channel_7*/
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
//	//ͨ��5����һ������Ŀ�꣬����55.5
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
//	//ͨ��6���ڶ�������Ŀ�꣬����55.5
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
//	//ͨ��7������������Ŀ�꣬����55.5
//
//	/*5.����ADC��У׼*/
//	ADC_Cmd(ADC1,ENABLE);
//	//ʹ��ָ����ADC1
//	ADC_ResetCalibration(ADC1);
//	//ʹ�ܸ�λУ׼
//	while(ADC_GetResetCalibrationStatus(ADC1));
//	//�ȴ���λУ׼����
//	ADC_StartCalibration(ADC1);
//	//����ADCУ׼
//	while(ADC_GetCalibrationStatus(ADC1));
//	//�ȴ�ADCУ׼����
//
//	return;
//
////	/*5.DMA����*/
////	DMA_DeInit(DMA1_Channel1);//DMA1��ͨ��1��λ
//
////	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
////	//DMA�������ַ
////	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)AdcBuf;
////	//DMA�ڴ����ַ
////	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
////	//DMA���ݴ��䷽�򡪡�������Ϊ������Դ
////	DMA_InitStruct.DMA_BufferSize = 3;
////	//���仺���С��HD_136 RP_C18_3��
////	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
////	//�����ַ�����ر�
////	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
////	//�ڴ��ַ������
////	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
////	//�������ݴ�С-���֣�16λ��
////	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
////	//�ڴ����ݴ�С
////	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
////	//ѭ��ģʽ
////	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
////	//ͨ�������ȼ�������
////	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
////	//������Ϊ�ڴ浽�ڴ洫��
////	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
////	//���ݲ�����ʼ��DMA1��ͨ��1
////		
////	ADC_DMACmd(ADC1,ENABLE);		//ʹ��ADC��DMA����
////	DMA_Cmd(DMA1_Channel1,ENABLE);	//ʹ��ADC1_DMA��ָʾ��ͨ��
//	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);\
//	//ADC_GetConversionValue(ADC1);
//	// /*6.DMA�ж�����*/
//	// NVIC_InitTypeDef NVIC_InitStruct;
//	// NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;//�ж�ͨ��
//	// NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
//	// NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�
//	// NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	// NVIC_Init(&NVIC_InitStruct);
//	
//	// DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//����DMA�Ĵ�������ж�
//}
void Adc_Init(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M


	//PC0 1 ��Ϊģ��ͨ����������  ADC12_IN8                       

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

//���ADCֵ
//ch:ͨ��ֵ 
uint16_t Get_Adc(uint8_t ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	u32 temp_val=0;
	uint8_t t;
	
	for(t=0;t<times;t++)
	{
		temp_val += Get_Adc(ch);
		delay_ms(2);
	}
	return temp_val / times;
} 	


