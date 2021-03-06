/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     smokeMS
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 14:14:29
 * @Description: 
 * ————粉尘检测模块
 * ---------------------------------------------------------------------------*/

#ifndef SMOKE_MS_H
#define SMOKE_MS_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"


typedef struct{

	bool	 	VAL;
	uint16_t anaDAT;
}smokeMS_MEAS;

extern osThreadId 	 tid_smokeMS_Thread;
extern osThreadId 	 tid_smokeMS_led_Thread;
extern osPoolId  	 smokeMS_pool;
extern osMessageQId  MsgBox_smokeMS;
extern osMessageQId  MsgBox_MTsmokeMS;
extern osMessageQId  MsgBox_DPsmokeMS;

void smokeMS_Init(void);
void smokeMS_Thread(const void *argument);
void smokeMS_led_Thread(const void *argument);

void smokeMSThread_Active(void);

#endif

