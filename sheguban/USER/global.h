#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
  GS-P2T-GXU01 ��ʰ�   3 x 16 ·
          2018-9-18 
					by: ������					
			 
	����ʹ�ã�
	LED-595: 	  
		595-ST:PB0   595-SH:PB1   595-MR:PB10 595-DS:PB11
		
	Out-595: 	  
		ST:PB3   SH:PB4   MR:PB5  DS:PB6
	
	BEEP: PB9  �ߵ�ƽ��Ч
	
	���ڣ�RS485(ͨ��)            
	     UART1   TX:PA9  TX:PA10  DIR:PA11
			 
	ADDR X 10:
		8:PB15  4:PB14  2:PB13 	1:PB12
	ADDR X 1:
		8:PA7  	4:PA6  	2:PA5 	1:PA4	 

	Mode
		Mode-SW1:
		Mode-SW2: 
		Mode-SW3: PA8
		Mode-SW4: PB7
		Mode-SW5: PB8
		
	Type
		Type-SW1: PC13
		Type-SW2: PC14
		Type-SW3:	PC15			
****************************************/
#include "stdio.h"	
#include <stdlib.h>
#include "sys.h" 
#include <ctype.h>
#include <string.h>
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"

#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "usart_cfg.h" 

#include "timer.h" 

#include "usart1_RS485_V&I.h"   

#include  "fuc_595.h"

 
 /*
�豸�����	�ն˰�׿��	20
	ʵѵ����������	21
	��Դ����������	22
	16·2̬3A��ʰ�	23
	16·5̬1A��ʰ�	24
	8·2̬16A��ʰ�	25
*/

 #define  DTN_android          20      //�豸�� 
// #define  DTN_main_manage      21    
// #define  DTN_power_manage     22
#define  DTN_16L_2T_3A        23  //0x17
#define  DTN_16L_5T_1A        24  //0x18
#define  DTN_8L_2T_16A        25  //0x19


//����̵���5̬
#define  OPEN                 0 
#define  CLOSE                1    
#define  VIRTUAL_CONNECT      2
#define  VIRTUAL_EARTH        3
#define  ACCIDENTAL           4 
 
#define  UID_BASE              0x1FFFF7E8U  

extern u16    MY_SWTYPE ;
extern u8     MY_CODE ;
extern uint32_t MCU_UID[3];
extern u16    My_vol; 
extern u16		My_curr;  
extern u16    My_watt; 
extern u32		My_powr;
 
extern u8     search_addr_flg ;
extern u16    alarm_time ; 
extern u32 		ACCIDENTAL_time[16];  
extern u16    rand_seed ;
  
extern u16   	send_urt_time ;
 
extern  u8    POWER_220V_FLG ;  //AC_CHECK

extern  u8    MY_JDQ_TAB[16];

extern  u8 		read_code( void );

#endif
