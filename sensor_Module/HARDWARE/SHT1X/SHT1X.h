/*************************************************************
                             \(^o^)/
  Copyright (C), 2013-2020, ZheJiang University of Technology
  File name  : SHT10.h 
  Author     : ziye334    
  Version    : V1.0 
  Data       : 2014/3/10      
  Description: Digital temperature and humidity sensor driver code
  
*************************************************************/
#ifndef __SHT1X_H__
#define __SHT1X_H__
#include "global.h"

enum {TEMP, HUMI};

/* GPIO��غ궨�� */
#define SHT10_AHB2_CLK        RCC_APB2Periph_GPIOB
#define SHT10_DATA_PIN        GPIO_Pin_3
#define SHT10_SCK_PIN        GPIO_Pin_4
#define SHT10_DATA_PORT        GPIOB
#define SHT10_SCK_PORT        GPIOB

#define SHT10_DATA_H()        GPIO_SetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)                         //����DATA������
#define SHT10_DATA_L()        GPIO_ResetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)                         //����DATA������
#define SHT10_DATA_R()        GPIO_ReadInputDataBit(SHT10_DATA_PORT, SHT10_DATA_PIN)         //��DATA������

#define SHT10_SCK_H()        GPIO_SetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)                                 //����SCKʱ����
#define SHT10_SCK_L()        GPIO_ResetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)                         //����SCKʱ����

/* ��������غ궨�� */
#define        noACK        0
#define ACK                1
                                                                //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          д״̬�Ĵ���
#define STATUS_REG_R        0x07        //000         0011          1          ��״̬�Ĵ���
#define MEASURE_TEMP         0x03        //000         0001          1          �����¶�
#define MEASURE_HUMI         0x05        //000         0010          1          ����ʪ��
#define SOFTRESET       0x1E        //000         1111          0          ��λ


void SHT1X_Config(void);
u8 SHT1X_GetValue(void);
void SHT10_ConReset(void);
u8 SHT10_SoftReset(void);
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode);
void SHT10_Calculate(u16 t, u16 rh,float *p_temperature, float *p_humidity);
float SHT10_CalcuDewPoint(float t, float h);


#endif



///***********************************************************************************************************************
//Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
//�ļ��� :		SHT1X.h
//����   :      SHT1X��ʪ�ȴ�����ģ�����ͷ�ļ�
//����   :      hongxiang.liu
//�汾   :      V1.0
//�޸�   :  
//�������:		2019.3.21
//************************************************************************************************************************/
//#ifndef _SHT1X_H_
//#define _SHT1X_H_

///*************HEADS**********/
//#include "global.h"
//#include "sys.h"
//#include "delay.h"
//#include <math.h>
///***************************/

///***********�궨��************/
//#define SCL GPIO_Pin_4
//#define SDA GPIO_Pin_3

//#define noACK 0             //??????
//#define ACK   1             //???????
//                            //adr  command  r/w 
//#define STATUS_REG_W 0x06   //000   0011    0 
//#define STATUS_REG_R 0x07   //000   0011    1 
//#define MEASURE_TEMP 0x03   //000   0001    1 
//#define MEASURE_HUMI 0x05   //000   0010    1 
//#define RESET        0x1e   //000   1111    0 

//#define SDA_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOC->CRL|=4<<12;} //MODE3��15:12λֵλ10,��Ӧ��ģʽΪ���ģʽ
//#define SDA_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOC->CRL|=3<<12;} //MODE3��15:12λֵλ00,��Ӧ��ģʽΪ����ģʽ

///*****************************/

///**********��������***********/
//void SHT1x_Init(void);
//void SHT1x_Config(void);
//void SHT1x_GetValue(void);
//void SDA_SET_OUT(void);
//void SDA_SET_IN(void);
//void SCL_Pin_OUT(u8 out);
//void SDA_Pin_OUT(u8 out);
//void SHT1x_Start(void);
//void SHT1x_Reset(void);
//char SHT1X_softreset(void);
//char SHT1X_write_statusreg(unsigned char value);
//char SHT1X_read_statusreg(unsigned char *p_value);
//u8 SHT1x_Measure(u16 *p_value, u8 *p_checksum, u8 Mode);
//u8 SHT1x_Write_Byte(u8 value);
//u8 SHT1x_Read_Byte(u8 dat);
//void SHT1X_Caculation(float *p_temperature,float *p_humidity );
//void SHT1X_Caculation1(float *p_temperature,float *p_humidity );
//float SHT1X_dewpoint(float h,float t);
//float SHT1X_dewpoint1(float t,float h); //¶��
///*****************************/

//#endif

