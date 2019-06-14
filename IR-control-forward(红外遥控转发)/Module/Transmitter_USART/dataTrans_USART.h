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


#include "infraTrans.h"

#define  WIRLESS_THREAD_EN	123

#define  datsTransCMD_UPLOAD	0x10
#define  datsTransCMD_DOWNLOAD	0x20

#define	 uint8	u8


#define	 ABNORMAL_DAT	0xfe

extern uint8 gb_databuff[50];

extern volatile bool gb_Exmod_key;

extern osThreadId tid_USARTWireless_Thread;
	
void USART2Wireless_wifiESP8266Init(void);

void myUSART2_callback(uint32_t event);
void USART2Wirless_Init(void);
void USART_WirelessInit(void);
void USARTWireless_Thread(const void *argument);
void UsartRx_Thread(const void *argument);	

void wirelessThread_Active(uint8);

#endif
