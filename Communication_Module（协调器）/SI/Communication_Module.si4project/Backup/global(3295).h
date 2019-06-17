#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
				
				
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

#include "usart1.h"
#include "usmart.h"
#include "LCD_144.h"
#include "GUI_144.h"









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



#pragma pack(push,1)
typedef  struct
{  
	u32   CAP_IR_WAVEFORM_TIME;
	u32   CAP_IR_store_num;
	u32   IR_tab_js ;

	u16   IR_tab_len ;  //���� 
	u16   IR_tab[1024];  //��ȡ��֡����
}
IR_CAP_struct;    
#pragma pack(pop)


#include "usart_cfg.h" 
#include "usart3_android.h" 
#include "usart4_ZIG1.h" 
#include "usart2_rs485.h"  
#include "LCD_144.h"//��ʾ�������̺����� 


#include "stmflash.h"


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

 

#define  DTN_JCZX_BOARD          	26
#define  DTN_JCZX_ANDROID        	27
#define  DTN_JCZX_fanzhuang       28
 

#define  shegu_tab_size       100 //�洢��ʰ�ı�ź��豸�ţ������������ֽ�
  
 
 

extern void  remote_cap_study( void  );

extern void IR_js( void );

extern  IR_CAP_struct   IR_CAP1;

 
extern  uint32_t MCU_UID[3];


extern uint8_t TagUID[16];

 
extern  u16  motor_ck_time ;
extern  u16 alarm_time ;

extern  u32 ANDROID_DEST_ADDR ;
 
extern  u8            m_mode ;
extern  u8            search_addr_flg ;

extern  u8            IR_CAP_START_FLG ;

extern  u16           join_LED_S ;
 
extern  u16           send_urt_time ;

extern  u8            JDQ_NOW_STATUS ;
extern u8             JDQ_WILL_STATUS ;
extern  u16           JDQ_time;

extern  u8            PC_KEY_FLAG;

#endif
