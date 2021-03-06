/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     pyroMS
 * @Author:     RanHongLiang
 * @Date:       2019-07-10 14:56:48
 * @Description: 
 * ————人体红外热释电检测模块
 * ---------------------------------------------------------------------------*/


#ifndef PYRO_MS_H
#define PYRO_MS_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

#define PYRO_DATA	PBin(8);

typedef struct{

	bool	 	VAL;
	uint16_t anaDAT;
}pyroMS_MEAS;

extern osThreadId 	 tid_pyroMS_Thread;
extern osPoolId  	 pyroMS_pool;
extern osMessageQId  MsgBox_pyroMS;
extern osMessageQId  MsgBox_MTpyroMS;
extern osMessageQId  MsgBox_DPpyroMS;

void pyroMS_Init(void);
void pyroMS_Thread(const void *argument);
void pyroMSThread_Active(void);

#endif

