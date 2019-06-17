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
#define curtMTUP	PBout(12)
//�½���ťLED
#define curtMTDN	PBout(13)
//ֹͣ��ťLED
#define curtMTSW	PBout(14)

//������ť
#define curtKeyUP	PBin(9)
//�½���ť
#define curtKeyDN	PBin(10)
//ֹͣ��ť
#define curtKeySW	PBin(11)

#define curtIOCHG_Kin()		{GPIOA->CRL&=0XFFFFF00F;GPIOB->CRL |= 0x00000440;}	
#define curtIOCHG_DB()		{curtain_logInit();}

typedef struct{
	
	bool CurtainENs;
	bool CurtainUpENs;
	bool CurtainDnENs;
	
	uint16_t valElec;
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

