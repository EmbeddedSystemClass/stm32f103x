

#ifndef _ALL_Includes_H
#define _ALL_Includes_H

#include "stdio.h"
#include "stm8s.h"
#include "stm8s_adc1.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "uart.h"
#include <ctype.h>
#include <string.h>

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

//�豸�ŵĶ���
#define DTN_motor_window 0x32  //�豸��   �綯����
#define DTN_curtain 0x33       //�豸��   �綯Ļ��
#define DTN_POWER_SUBAREA 0x34 //�豸��   ��Դ��������
// 86���ص����1· 	54
// 86���ص����2· 	55
// 86���ص����3· 	56
#define DTN_86_LIGHT_1  0x36       //�豸��   һ·�ƹ�
#define DTN_86_LIGHT_2  0x37       //�豸��   ��·�ƹ�
#define DTN_86_LIGHT_3  0x38       //�豸��   ��·�ƹ�
#define DTN_86_SENSOR_MQ 0x39     //�豸��   ú��й©�ͻ��洫����
#define SENSOR_body     0x3A          //�豸��   ���崫����
#define SENSOR_tem_hum_light 0x3B //�豸��   ��ʪ�Ⱥ͹���ǿ��
#define DTN_86_power_outlet 0x3C  //�豸��   ���ܲ���
//���ݰ�����
#define SENSOR_tem_LEN 0x1c //��ʪ��ģ��-���ݳ���
#define power_outlet_LEN 0x11 //���ܲ���ģ��-���ݳ���
//mcu�İ���ID�궨��͵�ַ
#define READ_REG(REG) ((REG))
#define UID_BASE 0X4865
//CMD��ö�ٱ���
typedef enum
{
        join_sw = 1,       // ��������
        leave_net,         // ��������
        notify_net_status, // ֪ͨ����״̬
        upload_info,       // �ϴ���Ϣ��Android
        control_info,      // �����豸
        query_dev_info     // ��ѯ�豸��Ϣ

} dev_cmd;
//����״̬��ö��
typedef enum
{
        net_error,
        net_wait,
        net_online

} NETstatus;

//ʹ��ģ���л�
//#define sensor_md_BODY
//#define tem_hum_light
#define power_outlet

//GPIO->PIN_X  ���ú궨��
#define Pin(P) (1 << P)

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

// ����ʹ����ʪ�Ⱥ͹���ǿ��ģ�飡
#ifdef tem_hum_light

#define KEY (((GPIOC->IDR & Pin(6)) == 0) ? 1 : 0)

#define LED(a) \
        (a == 1) ? (GPIOC->ODR &= ~Pin(7)) : (GPIOC->ODR |= Pin(7))
#endif

// ����ʹ�����ܲ���ģ�飡
#ifdef power_outlet

#define KEY (((GPIOA->IDR & GPIO_PIN_1) == 0) ? 1 : 0)

#define LED_r(a)    (a == 1) ? (GPIOD->ODR &= ~GPIO_PIN_2) : (GPIOD->ODR |= GPIO_PIN_2)
#define LED_b(a)    (a == 1) ? (GPIOD->ODR &= ~GPIO_PIN_3) : (GPIOD->ODR |= GPIO_PIN_3)
#define Z_rest(a)   (a == 0) ? (GPIOD->ODR &= ~GPIO_PIN_4) : (GPIOD->ODR |= GPIO_PIN_4)
#define Rly(a)      (a == 0) ? (GPIOC->ODR &= ~GPIO_PIN_7) : (GPIOC->ODR |= GPIO_PIN_7)
#define Rly_Vlue    (((GPIOC->ODR & GPIO_PIN_7) != 0) ? 1 : 0)
#endif

//�����ͱ��������÷�Χ������
extern NETstatus gb_Status;
extern NETstatus gb_Status_pre;
extern u16 gb_init_countdown;
extern u16 gb_countdown;
extern u16 gb_countdown_uart;
extern uint32_t SystemCnt;

extern uint32_t MCU_UID[3];
extern u32 ANDROID_DEST_ADDR;
extern u8 search_addr_flg;
extern u8 join_flg;
extern u8 Data_Len;
extern u8 uart_rx_flg;

extern u8 sampling_in_flg;

extern u16 UART_NO_DAT_TIME;
extern TRAN_D_struct TRAN_info1;

extern void Delay(uint16_t nCount);
extern void delay_ms(uint16_t nCount);
extern u16 get_adc(u8 times);

#endif
