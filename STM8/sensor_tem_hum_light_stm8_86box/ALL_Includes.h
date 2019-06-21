

#ifndef _ALL_Includes_H
#define _ALL_Includes_H

#include "LED_CON.h"
#include "stdio.h"
#include "stm8s.h"
#include "stm8s_adc1.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "uart.h"
#include <ctype.h>
#include <string.h>

#include "SHT1X.h"
#include "lightMS.h" //����ǿ�ȼ���������̺�����
#include "tempMS.h"

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define KEY3_PC3_Pin (1 << 3)
#define KEY1_PC4_Pin (1 << 4)
#define KEY2_PC5_Pin (1 << 5)

#define KEY1_PC4_Pin_RD (GPIOC->IDR & KEY1_PC4_Pin)
#define KEY2_PC5_Pin_RD (GPIOC->IDR & KEY2_PC5_Pin)
#define KEY3_PC3_Pin_RD (GPIOC->IDR & KEY3_PC3_Pin)

//����Ļ��
#define DOWN_IN KEY1_PC4_Pin_RD
#define PAUSE_IN KEY2_PC5_Pin_RD
#define UP_IN KEY3_PC3_Pin_RD

//���ڿ��ص�
#define KEY_3L KEY1_PC4_Pin_RD
#define KEY_2L KEY2_PC5_Pin_RD
#define KEY_1L KEY3_PC3_Pin_RD

#define SAMPING_PD2_Pin (1 << 2)

#define SAMPING_PD2_Pin_RD (GPIOD->IDR & SAMPING_PD2_Pin)
#define chcek_run (GPIOD->IDR & SAMPING_PD2_Pin)

#define MODE_AD_PD3_Pin (1 << 3)

#define MODE_AD_PD3_Pin_RD (GPIOD->IDR & MODE_AD_PD3_Pin_Pin)

#define LED1_PD4_Pin (1 << 4)
#define LED2_SWIM_PD1_Pin (1 << 1)
#define LED3_PB5_Pin (1 << 5)
#define LED4_PA1_Pin (1 << 1)
#define LED5_PB4_Pin (1 << 4)
#define LED6_PA2_Pin (1 << 2)

#define LED1_PD4_Pin_OUT(a) \
        (a != 0) ? (GPIOD->ODR &= ~LED1_PD4_Pin) : (GPIOD->ODR |= LED1_PD4_Pin)
#define LED2_SWIM_PD1_Pin_OUT(a) \
        (a != 0) ? (GPIOD->ODR &= ~LED2_SWIM_PD1_Pin) : (GPIOD->ODR |= LED2_SWIM_PD1_Pin)
#define LED3_PB5_Pin_OUT(a) \
        (a != 0) ? (GPIOB->ODR &= ~LED3_PB5_Pin) : (GPIOB->ODR |= LED3_PB5_Pin)
#define LED4_PA1_Pin_OUT(a) \
        (a != 0) ? (GPIOA->ODR &= ~LED4_PA1_Pin) : (GPIOA->ODR |= LED4_PA1_Pin)
#define LED5_PB4_Pin_OUT(a) \
        (a != 0) ? (GPIOB->ODR &= ~LED5_PB4_Pin) : (GPIOB->ODR |= LED5_PB4_Pin)
#define LED6_PA2_Pin_OUT(a) \
        (a != 0) ? (GPIOA->ODR &= ~LED6_PA2_Pin) : (GPIOA->ODR |= LED6_PA2_Pin)

#define KEY_LEFT_RED(a) LED1_PD4_Pin_OUT(a)
#define KEY_LEFT_BLUE(a) LED2_SWIM_PD1_Pin_OUT(a)

#define KEY_MID_RED(a) LED3_PB5_Pin_OUT(a)
#define KEY_MID_BLUE(a) LED4_PA1_Pin_OUT(a)

#define KEY_RIGHT_RED(a) LED5_PB4_Pin_OUT(a)
#define KEY_RIGHT_BLUE(a) LED6_PA2_Pin_OUT(a)

//#define   LED_ON                0
//#define   LED_OFF               1

#define RLY1_PA3_Pin (1 << 3)
#define RLY2_PC6_Pin (1 << 6)
#define RLY3_PC7_Pin (1 << 7)

#define RLY1_PA3_Pin_OUT(a) \
        (a != 0) ? (GPIOA->ODR |= RLY1_PA3_Pin) : (GPIOA->ODR &= ~RLY1_PA3_Pin)
#define RLY2_PC6_Pin_OUT(a) \
        (a != 0) ? (GPIOC->ODR |= RLY2_PC6_Pin) : (GPIOC->ODR &= ~RLY2_PC6_Pin)
#define RLY3_PC7_Pin_OUT(a) \
        (a == 0) ? (GPIOC->ODR &= ~RLY3_PC7_Pin) : (GPIOC->ODR |= RLY3_PC7_Pin)

#define RLY1_OUT(a) RLY1_PA3_Pin_OUT(a)
#define RLY2_OUT(a) RLY2_PC6_Pin_OUT(a)
#define RLY3_OUT(a) RLY3_PC7_Pin_OUT(a)

#define RLY_ON 1
#define RLY_OFF 0

#define DTN_motor_window 0x32  //�豸��   �綯����
#define DTN_curtain 0x33       //�豸��   �綯Ļ��
#define DTN_POWER_SUBAREA 0x34 //�豸��   ��Դ��������

// 86���ص����1· 	54
// 86���ص����2· 	55
// 86���ص����3· 	56
#define DTN_86_LIGHT_1 0x36       //�豸��   һ·�ƹ�
#define DTN_86_LIGHT_2 0x37       //�豸��   ��·�ƹ�
#define DTN_86_LIGHT_3 0x38       //�豸��   ��·�ƹ�
#define DTN_86_SENSOR_MQ 0x39     //�豸��   ú��й©�ͻ��洫����
#define SENSOR_body 0x3A          //�豸��   ���崫����
#define SENSOR_tem_hum_light 0x3B //�豸��   ��ʪ�Ⱥ͹���ǿ��
#define DTN_86_power_outlet 0x3C  //�豸��   ���ܲ���

#define SENSOR_tem_LEN 0x1c //��ʪ��ģ��-���ݳ���

#define READ_REG(REG) ((REG))
#define UID_BASE 0X4865

    typedef enum {
            join_sw = 1,       // ��������
            leave_net,         // ��������
            notify_net_status, // ֪ͨ����״̬
            upload_info,       // �ϴ���Ϣ��Android
            control_info,      // �����豸
            query_dev_info     // ��ѯ�豸��Ϣ

    } dev_cmd;

typedef enum
{
        net_error,
        net_wait,
        net_online

} NETstatus;
//#define sensor_md_BODY
#define tem_hum_light

#define Pin(P) (1 << P)
//TODO: ����һ��δ�����ķ�֧
// SENSOR������ʹ�ܣ�������:ʹ�����壬��֮ʹ��ú��������
#ifdef sensor_md_BODY

#define body_check (((GPIOA->IDR & Pin(1)) == 0) ? 1 : 0)
#define KEY \
        (((GPIOA->IDR & Pin(2)) == 0) ? 1 : 0) // (GPIOA->IDR & Pin(2) )
#define LED(a) \
        (a == 1) ? (GPIOA->ODR &= ~Pin(3)) : (GPIOA->ODR |= Pin(3))

//#else

// mq5ú����������fire���洫����

#define fire_check (((GPIOD->IDR & Pin(3)) == 0) ? 1 : 0)
#define KEY (((GPIOC->IDR & Pin(6)) == 0) ? 1 : 0)
#define LED(a) \
        (a == 1) ? (GPIOA->ODR &= ~Pin(3)) : (GPIOA->ODR |= Pin(3))
#define BEEP_s(a) \
        (a == 0) ? (GPIOC->ODR &= ~Pin(7)) : (GPIOC->ODR |= Pin(7))

#endif

#ifdef tem_hum_light

#define KEY (((GPIOC->IDR & Pin(6)) == 0) ? 1 : 0)

#define LED(a) \
        (a == 1) ? (GPIOC->ODR &= ~Pin(7)) : (GPIOC->ODR |= Pin(7))
#endif

extern NETstatus gb_Status;
extern NETstatus gb_Status_pre;
extern u16 gb_countdown;
extern u16 gb_countdown_uart;
extern uint32_t SystemCnt;

extern uint32_t MCU_UID[3];

extern u16 motor_ck_time;
extern u16 alarm_time;

extern u16 LED_time;

extern u32 ANDROID_DEST_ADDR;

extern u8 search_addr_flg;

extern u8 curtain_status;

extern u8 join_flg;

extern u8 Data_Len;

extern u8 uart_rx_flg;

extern u8 sampling_in_flg;

extern u16 UART_NO_DAT_TIME;

extern u16 sampling_time;
extern u16 sampling_status;

extern u8 program_num;

extern u8 LIGHT_1_STATUS;
extern u8 LIGHT_2_STATUS;
extern u8 LIGHT_3_STATUS;

extern TRAN_D_struct TRAN_info1;

extern void JDQ_WINDOW_PAUSE(void);
extern void JDQ_WINDOW_DOWN(void);
extern void JDQ_WINDOW_UP(void);
extern void Delay(uint16_t nCount);
extern void delay_ms(uint16_t nCount);
extern void jiance(void);
extern void jiance_light_key(void);
extern u16 get_adc(u8 times);

#endif
