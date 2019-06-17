#ifndef DATATRANS_USART_H
#define DATATRANS_USART_H

#define osObjectsPublic                     // define objects in main module

#pragma pack(1)
typedef  struct
{   
	unsigned short   dest_dev_num;     //Ŀ���豸��
	unsigned char    dest_addr[4];     //Ŀ���ַ
	unsigned short   source_dev_num;   //Դ�豸��
	unsigned char    source_addr[4];   //Դ��ַ 
	unsigned char    TYPE_NUM ;        //���ݸ�ʽ��
	unsigned char    data_len ;        //���ݳ���	 
}
TRAN_D_struct;    //��ͷ�ļ�ǰ



#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include <dataTrans_USART.h>
#include "Driver_USART.h"
#include "string.h"

#include <Key&Tips.h>
#include "Moudle_DEC.h"
#include "Tips.h"


#include "sourceCM.h"



#define	 uint8	u8

extern uint8 gb_databuff[50];

extern volatile bool gb_Exmod_key;

extern osThreadId tid_USARTWireless_Thread;
	

void myUSART4_callback(uint32_t event);

void USART4Wirless_Init(void);
void USART_WirelessInit(void);

void USARTWireless_Thread(const void *argument);
void UsartRx_Thread(const void *argument);	
void zigbee_status_updata(u8 EXmodID);

void wirelessThread_Active(uint8);

#endif
