#ifndef CURTAIN_CM_H
#define CURTAIN_CM_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"


//������ťLED
#define curtconUP	PBout(12)
//�½���ťLED
#define curtconDN	PBout(13)
//ֹͣ��ťLED
#define curtconSTP	PBout(14)

//������ť
#define curtKeyUP	PBin(9)
//�½���ť
#define curtKeyDN	PBin(10)
//ֹͣ��ť
#define curtKeySTP	PBin(11)

//��λ
#define curtainENABLE	PAin(10)


#define CMD_CURTUP	0x01
#define CMD_CURTSTP	0x02
#define CMD_CURTDN	0x03
#define CMD_CURTmax	0x04


#define  valACT_NULL	100

#define curtIOCHG_Kin()		{GPIOA->CRL&=0XFFFFF00F;GPIOB->CRL |= 0x00000440;}	
#define curtIOCHG_DB()		{curtain_logInit();}

typedef struct{
	
	uint8_t valACT;
}curtainCM_MEAS;

extern osThreadId 	 tid_curtainCM_Thread;
extern osPoolId  	 curtainCM_pool;
extern osMessageQId  MsgBox_curtainCM;
extern osMessageQId  MsgBox_MTcurtainCM;
extern osMessageQId  MsgBox_DPcurtainCM;

void curtainCM_Init(void);
void curtainCM_Thread(const void *argument);
void curtainCMThread_Active(void);

#endif

