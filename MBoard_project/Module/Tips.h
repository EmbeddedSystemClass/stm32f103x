/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     Tips
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 16:36:56
 * @Description: 
 * —————文件内包括外部提示文件
 * ---------------------------------------------------------------------------*/

#ifndef _TIPS_H__
#define _TIPS_H__

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"

#include "IO_Map.h"
#include "delay.h"

#define MSGZigbee_rest		PEout(2)

#define LED_MSGZigbee_OK	PEout(3)
#define LED_EXECIfr_OK		PBout(13)

#define LED_SYS			PEout(7)
#define LED_MSG			PEout(10)
#define LED_EXT			PEout(13)
#define LED_MSG_N		PEout(3)
#define LED_EXT_N		PDout(1)//PEout(9)



#define LED1_SYS_g		PEout(7)
#define LED1_SYS_r		PEout(8)
#define LED1_SYS_b		PEout(9)

#define LED2_MSG_g		PEout(10)
#define LED2_MSG_r		PEout(11)
#define LED2_MSG_b		PEout(12)

#define LED3_EXT_g		PEout(13)
#define LED3_EXT_r		PEout(14)
#define LED3_EXT_b		PEout(15)




#define OBJ_SYS			1
#define OBJ_MSG			2
#define OBJ_EXT			3

#define EVTSIG_SYS_A		0x0000001A
#define EVTSIG_MSG_A		0x0000001B
#define EVTSIG_EXT_A		0x0000001C

#define EVTSIG_SYS_B		0x0000000A
#define EVTSIG_MSG_B		0x0000000B
#define EVTSIG_EXT_B		0x0000000C

typedef enum
{
	led1_r,led1_g,led1_b,
	led2_r,led2_g,led2_b,led2_b_flash,
	led3_r,led3_g,led3_b,led3_b_flash,

}LEDstatus;

extern osThreadId tid_tips;
extern volatile LEDstatus led1_status;
extern volatile LEDstatus led2_status;
extern volatile LEDstatus led3_status;


void tipsLEDActive(void);
void tipsInit(void);
void tipsBoardActive(void);
void tipsThread(void const *argument);

void tips_beep(u8 tones, u16 time, u8 vol);
void beeps(u8 num);

void TTask0(void const *argument);

#endif
