
/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		my_define.c
����   :      ���ȫ�ֱ�������
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.22
************************************************************************************************************************/
/*************HEADS*********/
#include "global.h"
/***************************/

/****** ȫ�ֱ������� ******/
uint64_t SystemCnt;		//ϵͳ����ʱ��(ms)
bool TimeOutFlag;		//10ms�����־λ
u8  Tim1CaptureState;	//TIM1����״̬λ  		���λbit7-�ɹ����� �θ�λbit6-�Ѿ����������� bit5~bit0-�ߵ�ƽ���������
u16	Tim1CaptureCount;	//TIM1�������ֵ
bool HcSr04StartF;		//��ഫ������ʼ�ı�־
u8  SwitchType;			//8421���뿪�ؼ�ֵ
u32 AdcBuf[3];			//����ADC�ɼ�ֵ
u16 PwmSpeed;			//���ٷ���ת��
bool PwmPowerFlag;		//���ȿ�����־
SHT1x sht1x; 			//���崫�����ṹ��
float SoilTemp;			//�����¶�
float SoilHum;			//����ʪ��
float WindSpeed;		//����ֵ
bool  Key1Flag;			//KEY1����ֵ
bool  Key2Flag;			//KEY2����ֵ
uint64_t  BeepCnt;			//������������ʱ

u16 humi_val,temp_val;		/**/
float humi_val_real = 0.0; 	//SHT11 ��ʪ����ر���
float temp_val_real = 0.0;	/**/
float dew_point = 0.0;		/**/
/**************************/




