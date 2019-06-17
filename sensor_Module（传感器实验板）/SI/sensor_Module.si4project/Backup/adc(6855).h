/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		adc.h
����   :      adc��������ͷ�ļ�
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.22
************************************************************************************************************************/
#ifndef _ADC_H_
#define _ADC_H_

/*************HEADS**********/
#include "global.h"
/***************************/

/**********�궨��***********/
//���Ŷ���
#define FAN_TAC_PERIPH			RCC_APB2Periph_GPIOA
#define FAN_TAC_PORT			GPIOA
#define FAN_TAC_PIN				GPIO_Pin_5
#define FAN_TAC_PINNUM			(5)

#define HD_136_PERIPH			RCC_APB2Periph_GPIOA
#define HD_136_PORT				GPIOA
#define HD_136_PIN				GPIO_Pin_6
#define HD_136_PINNUM			(6)

#define RP_C18_3_PERIPH			RCC_APB2Periph_GPIOA
#define RP_C18_3_PORT			GPIOA
#define RP_C18_3_PIN			GPIO_Pin_7
#define RP_C18_3_PINNUM			(7)

/***************************/

/**********��������***********/
void Adc_Init(void);
/*****************************/

#endif

