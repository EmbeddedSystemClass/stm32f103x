#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
   GTA-GXU01 ʵѵ���������ذ�
          2018-9-18 
					by: ������					
  
	����ʹ�ã�
	LED:  RFID  LED  PB5 ;
        RS232 LED  PB4 ;	
        RS485 LED  PB3 ;		
	
	BEEP: pc4
	
	���ڣ�RS485(����ʺ�)        UART3   PB10 PB11
	      RS232(��ANDROID����)   UART2   PA2  PA3
				��ӡ��� ������        UART1   PA9  PA10  

  RFID 13.56M   CR95HF  CR95V5: 
	      IRQ_IN            PC8
	      IRQ_OUT           PC9
	      CR95HF_NSS        PB12
	      CR95HF_CLK        PB13
	      CR95HF_MOSI       PB15
	      CR95HF_MISO       PB14
				CR95HF_INTERFACE  PC6
				
				
****************************************/

#include "stdio.h"	
#include "sys.h" 
#include <ctype.h>
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h" 
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "timer.h" 


#pragma pack(1)
typedef  struct
{   
	u16   dest_dev_num;     //Ŀ���豸��
	u8    dest_addr[4];     //Ŀ���ַ
	u16   source_dev_num;   //Դ�豸��
	u8    source_addr[4];   //Դ��ַ 
	u8    TYPE_NUM ;        //���ݸ�ʽ��
	u8    data_len ;        //���ݳ���	 
}
TRAN_D_struct;    //��ͷ�ļ�ǰ



 


#include "usart_cfg.h"  
#include "usart3_rs485.h"  
 


#include "CR95HF.h" 

#include "lib_ConfigManager.h"
#include "hw_config.h"
#include "drv_interrupt.h"
#include "lib_iso14443Apcd.h"
#include "lib_iso18092pcd.h"
#include "lib_iso14443Bpcd.h"

 

#include "stmflash.h"


#include  "fuc_595.h"



#define UID_BASE              0x1FFFF7E8U  
 
   
 
  /*
�豸�����	�ն˰�׿��	20
	ʵѵ����������	21
	��Դ����������	22
	16·2̬3A��ʰ�	23
	16·5̬1A��ʰ�	24
	8·2̬16A��ʰ�	25
	
	�������İ�     26
	���ذ�׿��     27
*/

  #define  DTN_TER_RIFD          19      //�豸�� 
  #define  DTN_android          20      //�豸�� 

 
 

 #define  L_R1C4    0X80
 #define  L_R1C3    0X40
 #define  L_R1C2    0X20
 #define  L_R1C1    0X10
 
 #define  L_R2C4    0X08
 #define  L_R2C3    0X04
 #define  L_R2C2    0X02
 #define  L_R2C1    0X01
 
 #define  L_R3C4    0X80
 #define  L_R3C3    0X40
 #define  L_R3C2    0X20
 #define  L_R3C1    0X10
 
 #define  L_R4C4    0X08
 #define  L_R4C3    0X04
 #define  L_R4C2    0X02
 #define  L_R4C1    0X01
 
extern  uint32_t MCU_UID[3];

extern ISO14443B_CARD 	ISO14443B_Card;
extern ISO14443A_CARD 	ISO14443A_Card;
extern uint8_t TagUID[16];
extern FELICA_CARD 	FELICA_Card;
 
extern  u16  motor_ck_time ;
extern  u16 alarm_time ;

extern  u32 ANDROID_DEST_ADDR ;
 
extern  u8            m_mode ;
extern  u8            search_addr_flg ;

extern  u8            LED_DAT[2];
extern  u16           join_LED_S ;
 
extern  u16           send_urt_time ;

#endif
