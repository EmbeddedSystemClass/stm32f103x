/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		driver_i2c.h
����   :      I2C��������ͷ�ļ�
����   :      hongxiang.liu
�汾   :      V1.0
�޸�   :  
�������:		2019.3.21
************************************************************************************************************************/
#ifndef _DRIVER_IIC_H_
#define _DRIVER_IIC_H_

/*************HEADS**********/
#include "global.h"
/***************************/

/**********�궨��***********/
//���Ŷ���
#define SHT1X_DAT_PERIPH			RCC_APB2Periph_GPIOB
#define SHT1X_DAT_PORT				GPIOB
#define SHT1X_DAT_PIN				GPIO_Pin_3
#define SHT1X_DAT_PINNUM			(3)

#define SHT1X_SCK_PERIPH			RCC_APB2Periph_GPIOB
#define SHT1X_SCK_PORT				GPIOB
#define SHT1X_SCK_PIN				GPIO_Pin_4
#define SHT1X_SCK_PINNUM			(4)

//IO����
#define IIC_SDA_IN()	{GPIOB->CRL&=~(0x0F<<(3*2));GPIOB->CRL|=0x08<<(3*4);}	//PB3(������)����ģʽ
#define IIC_SDA_OUT()	{GPIOB->CRL&=~(0x0F<<(3*2));GPIOB->CRL|=0x03<<(3*4);}	//PB3(����)���ģʽ

#define IIC_SCL_H()		(PBout(4)=1)
#define IIC_SCL_L()		(PBout(4)=0)

#define IIC_SDA_H()		(PBout(3)=1)
#define IIC_SDA_L()		(PBout(3)=0)

#define IIC_RD_SDA()	(PBin(3))

#define IIC_ACK			(0)
#define IIC_NACK		(1)
/***************************/

/**********��������***********/
void Iic_GpioConfig(void);
void IicStart(void);
void IicStop(void);
u8 IicWaitAck(void);
void IicAck(void);
void IicNAck(void);
void IicSendByte(u8 Data);
u8 IicReceiveByte(u8 Ack);
/*****************************/

#endif

