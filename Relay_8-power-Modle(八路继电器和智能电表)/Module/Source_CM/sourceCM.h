#ifndef SOURCE_CM_H
#define SOURCE_CM_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"
#include "Tips.h"


#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"


#include "debugUart.h"


//ģʽ����
#define RS485_TX_EN		PBout(12)	//485ģʽ����.0,����;1,����.


#define  datsTransCMD_FLAG1	0x01
#define  datsTransCMD_FLAG2	0x02
#define  datsTransCMD_FLAG3	0x03
#define  datsTransCMD_FLAG4	0x04
	  
extern u8 RS485_RX_BUF[64]; 		//���ջ���,���64���ֽ�


typedef struct{

		uint8_t source_addr;
	
	bool Switch;
	uint32_t	 anaVal;
	uint32_t power_p ;
	uint32_t power_v ;
	uint32_t power_i ;
	
}sourceCM_MEAS;


 extern uint32_t	 gb_power_kwh;
 extern uint16_t	 gb_power_v;
 extern uint32_t	 gb_power_A;

extern osThreadId 	 tid_sourceCM_Thread;
extern osPoolId  	 sourceCM_pool;
extern osMessageQId  MsgBox_sourceCM;
extern osMessageQId  MsgBox_MTsourceCM;
extern osMessageQId  MsgBox_DPsourceCM;

void sourceCM_Init(void);
void sourceCM_Thread(const void *argument);
void sourceCMThread_Active(void);

#endif

