#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
   GTA-GXU01 ʵѵ���������ذ�
          2018-9-18 
					by: ������					
  
	����ʹ�ã�
	LED:   LED  PB9 ; 	
	
	BEEP: pc4
	
	���ڣ�RS485 (�ӵ�ѹ������)    UART1   PA9 PA10    con_pin1  PA8
	      RS485(ͨ��)            UART2   PA2  PA3    con_pin2  PA4
	  
  AC_CHECK    220V�Ƿ��ͨ����

  AC_con      220V �̵�������
				
****************************************/
#include "stdio.h"	
#include "stdbool.h"
#include "sys.h" 
#include <ctype.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h"

#include "bit_band.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "usart_cfg.h" 
#include "timer.h" 
#include "usart1_RS485_V&I.h"  
#include "usart2_RS485_comm.h"  
#include "driver_iic.h"
#include "adc.h"
#include "peripheral.h"
#include "SHT1X.h"
#include "LCD_144.h"
#include "tempMS.h"

  #define UID_BASE              0x1FFFF7E8U  

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
 #define  DTN_power_manage     22
 //#define  DTN_16L_2T_3A        23
 //#define  DTN_16L_5T_1A        24
 //#define  DTN_8L_2T_16A        25



extern  u32      MY_DEST_ADDR ;
extern  uint32_t MCU_UID[3];
extern u16       My_vol; 
extern u16		   My_curr;  
extern u16       My_watt; 
extern u32		   My_powr;


extern u8     search_addr_flg ;
extern u16    alarm_time ; 
 
extern u16   send_urt_time ;

extern  u8   POWER_220V_FLG ;  //AC_CHECK

/*********�궨������**********/
#define Fan_AdcIndex 				(0)
#define HD136_AdcIndex				(1)
#define RPC18_AdcIndex				(2)

/*Modbus��ַ��궨��*/
#define Soil_Detector_Addr			(0x03)			//������ʪ��
#define Anemograph_Addr				(0x04)			//���ټ�

/*****************************/

/*********�ṹ������**********/
typedef struct
{
	int Temperature;//ת�����¶�
	u16 Humidity;   //ת����ʪ��
    u16 DEW;
    float T_Result;
    float H_Result;
	u8 CRC8_Temperature; //У����
	u8 CRC8_Humidity;  //У����
}SHT1x;
/*****************************/


/********�����ⲿ����*********/
extern uint64_t SystemCnt;		//ϵͳ����ʱ��(ms)
extern bool TimeOutFlag;		//10ms�����־λ
extern u8  Tim1CaptureState;	//TIM1����״̬λ  		���λbit7-�ɹ����� �θ�λbit6-�Ѿ����������� bit5~bit0-�ߵ�ƽ���������
extern u16 Tim1CaptureCount;	//TIM1�������ֵ
extern bool HcSr04StartF;		//��ഫ������ʼ�ı�־
extern u8  SwitchType;			//8421���뿪�ؼ�ֵ
extern u32 AdcBuf[3];			//����ADC�ɼ�ֵ
extern u16 PwmSpeed;			//���ٷ���ת��
extern bool PwmPowerFlag;		//���ȿ�����־
extern SHT1x sht1x; 			//���崫�����ṹ��
extern float SoilTemp;			//�����¶�
extern float SoilHum;			//����ʪ��
extern float WindSpeed;			//����ֵ
extern bool  Key1Flag;			//KEY1����ֵ
extern bool  Key2Flag;			//KEY2����ֵ
extern uint64_t  BeepCnt;		//������������ʱ

extern u16 humi_val, temp_val;	/**/
extern float humi_val_real; 	//SHT11 ��ʪ����ر���
extern float temp_val_real;		/**/
extern float dew_point;			/**/

/*****************************/


#endif
