/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ����Ŀ
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-06-20 14:52:41
 * @Description: 
 *	���ܲ�����
 *	����ʱ��������˸��ʾ�������������˸��ʾδ������
 *	��������ʱ��
 * 		���ΰ����л�״̬����������3S���������л�״̬
 * 		��������Դָʾ�����Ʊ�ʾͨ��δ����е磬��Ʒ�֮��
 * 		�������˸��Ϊ�л�����״̬��������˸Ϊ��������
 *---------------------------------------------------------------------------*/

#include "ALL_Includes.h"

//����CPU�ڲ�ʱ��
#define SYS_CLOCK 16
#define TIM4_PERIOD 124

u8 Data_Len = 0;

u8 uart_rx_flg = 0;

u8 curtain_status = 2;

u16 motor_ck_time = 0;

u16 LED_time = 0;

u16 alarm_time = 0;

u8 join_flg = 0; //����״̬

uint32_t ad_ck_0_num = 0;

uint32_t MCU_UID[3];

u8 key_status = 2;

u8 sampling_in_flg = 0;

u16 UART_NO_DAT_TIME = 0;

u16 sampling_time = 0;
u16 sampling_status = 0;

u8 program_num = 0;

u8 LIGHT_1_STATUS = 0;
u8 LIGHT_2_STATUS = 0;
u8 LIGHT_3_STATUS = 0;

//
u8 maxcnt = 0;
uint32_t SystemCnt = 0; //ϵͳ����ʱ��(ms)

TRAN_D_struct TRAN_info1;

NETstatus gb_Status = net_error;
NETstatus gb_Status_pre = net_error;
u16 gb_countdown = 0;
u16 gb_init_countdown = 0;
u16 gb_countdown_uart = 0;
u16 gb_sensor_data[2];

void Delay(uint16_t nCount);

void All_Config(void);
void IO_Init(void);

void TIM4_Config(void);

void jiance(void);
void jiance_light_key(void);

void JDQ_WINDOW_PAUSE(void);
void JDQ_WINDOW_UP(void);
void JDQ_WINDOW_DOWN(void);

u8 chcek_run_status(u8);
void check_key(void);
void net_led_status(void);
void check_sensor(void);
u16 get_adc(u8 times);
void HAL_GetUID(uint32_t *UID);

void main(void)
{
	All_Config();

	TRAN_info1.dest_dev_num = 0; //Ŀ���豸��

	TRAN_info1.dest_addr[0] = 0x00; //Ŀ���ַ
	TRAN_info1.dest_addr[1] = 0x00;
	TRAN_info1.dest_addr[2] = 0x00;
	TRAN_info1.dest_addr[3] = 0x00;

	TRAN_info1.source_addr[0] = (u8)MCU_UID[0]; //Դ��ַ
	TRAN_info1.source_addr[1] = (u8)(MCU_UID[0] >> 8);
	TRAN_info1.source_addr[2] = (u8)(MCU_UID[0] >> 16);
	TRAN_info1.source_addr[3] = (u8)(MCU_UID[0] >> 24);

	TRAN_info1.TYPE_NUM = 3; //���ݸ�ʽ��: ���������ݸ�ʽ  ����ZIGBEE���Ǳ�׼��  �豸��
				 //  GPIO_Init(SHT10_SCK_PORT, SHT10_SCK_PIN,GPIO_MODE_OUT_PP_HIGH_FAST);
				 //    while(1)
				 //    	{ SHT10_Dly();
				 //                SHT10_SCK_H();
				 //                SHT10_Dly();
				 //                SHT10_SCK_L();}
	while (1)
	{
		check_key();
		net_led_status();
		check_sensor();
		;
		Rs485_COMM_uart_fuc();
	}
}
void check_sensor(void)
{
#ifdef sensor_md_BODY

	//�иı���ϴ�

	if ((body_check != gb_sensor_data[0]) || (gb_countdown_uart < 1))
	{

		delay_ms(20);
		if ((body_check != gb_sensor_data[0]) || (gb_countdown_uart < 1))
		{
			u8 dat[20];
			dat[0] = 0;
			dat[1] = upload_info;
			dat[2] = 0;

			dat[4] = (u8)MCU_UID[0];
			dat[5] = (u8)(MCU_UID[0] >> 8);
			dat[6] = (u8)(MCU_UID[0] >> 16);
			dat[7] = (u8)(MCU_UID[0] >> 24);

			memset(dat + 8, 0, 8);

			TRAN_info1.source_dev_num = (SENSOR_body << 8 | SENSOR_body >> 8);

			TRAN_info1.data_len = 16 + 1;

			dat[3] = SENSOR_body; //

			dat[8 + 8] = body_check;
			gb_sensor_data[0] = body_check;

			//��ն�ʱ��
			gb_countdown_uart = 6000;
			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}

	//#else
	//�иı���ϴ�,
	if ((get_adc(1) > 850) || (fire_check != gb_sensor_data[1]) || (gb_countdown_uart < 1))
	{
		u16 buff = get_adc(10);
		delay_ms(20);
		if ((buff > 850) || (fire_check != gb_sensor_data[1]) || (gb_countdown_uart < 1))
		{
			u8 dat[20];
			dat[0] = 0;
			dat[1] = upload_info;
			dat[2] = 0;

			dat[4] = (u8)MCU_UID[0];
			dat[5] = (u8)(MCU_UID[0] >> 8);
			dat[6] = (u8)(MCU_UID[0] >> 16);
			dat[7] = (u8)(MCU_UID[0] >> 24);

			memset(dat + 8, 0, 8);

			TRAN_info1.source_dev_num = (DTN_86_SENSOR_MQ << 8 | DTN_86_SENSOR_MQ >> 8);

			TRAN_info1.data_len = 16 + 2;

			dat[3] = DTN_86_SENSOR_MQ; //

			if (buff > 850)
			{
				dat[8 + 8] = 1;
			}
			else
			{
				dat[8 + 8] = 0;
			}

			dat[8 + 8 + 1] = fire_check;

			gb_sensor_data[1] = fire_check;

			gb_countdown_uart = 6000;

			if ((buff > 850) || (fire_check == 1))
			{

				BEEP_s(1);
			}
			else
				BEEP_s(0);

			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}

#endif
#ifdef tem_hum_light

	if (gb_countdown_uart < 1)
	{
		u8 dat[30] = {0};

		gb_countdown_uart = 6000;

		dat[0] = 0;
		dat[1] = upload_info;
		dat[2] = 0;

		dat[4] = (u8)MCU_UID[0];
		dat[5] = (u8)(MCU_UID[0] >> 8);
		dat[6] = (u8)(MCU_UID[0] >> 16);
		dat[7] = (u8)(MCU_UID[0] >> 24);

		memset(dat + 8, 0, 8);

		TRAN_info1.source_dev_num = (SENSOR_tem_hum_light << 8 | SENSOR_tem_hum_light >> 8);

		TRAN_info1.data_len = SENSOR_tem_LEN;

		///------------------ͨ������-------------------------------------------------///
		dat[3] = SENSOR_tem_hum_light; //

		memset(dat + 16, 0, 12);
		tempMS_Thread(dat + 0x10);
		lightMS_Thread(dat + 0x18);
		//				SHT1X_Config();
		//				dat[24] = SHT1X_GetValue(&tem,&hum,&temp_real,&hum_real);
		//
		//
		//				//f =CLK_GetClockFreq();
		//				memcpy(dat+16,&tem,2);
		//				memcpy(dat+20,&hum,2);

		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

#endif
#ifdef power_outlet

	if (gb_countdown_uart < 1)
	{
		u8 dat[30] = {0};

		gb_countdown_uart = 6000;

		dat[0] = 0;
		dat[1] = upload_info;
		dat[2] = 0;

		dat[4] = (u8)MCU_UID[0];
		dat[5] = (u8)(MCU_UID[0] >> 8);
		dat[6] = (u8)(MCU_UID[0] >> 16);
		dat[7] = (u8)(MCU_UID[0] >> 24);

		memset(dat + 8, 0, 8);

		TRAN_info1.source_dev_num = (DTN_86_power_outlet << 8 | DTN_86_power_outlet >> 8);

		TRAN_info1.data_len = power_outlet_LEN;

		///------------------ͨ������-------------------------------------------------///
		dat[3] = DTN_86_power_outlet; //

		memset(dat + 16, 0, 1);
		
		dat[16] = Rly_Vlue;
		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

#endif
	;
}

void check_key(void)
{
	u8 i = 0, dat[2];
	if (KEY)
	{

		delay_ms(20);
		while (KEY && ++i < 100)
			delay_ms(50);
		if (i < 3)
		{
			
#ifdef power_outlet	
			Rly(!Rly_Vlue);
			LED_b(0);
			LED_r(0);
			Rly_Vlue ? LED_r(1) : LED_b(1);
#endif
		}
		if (i > 90)
		{
			TRAN_info1.data_len = 2;

			dat[0] = 0;

			if (gb_Status == net_online) //���߼�����
			{
				gb_Status_pre = gb_Status;
				gb_Status = net_error;

				dat[1] = leave_net;
			}
			else
			{
				gb_Status_pre = gb_Status;
				gb_Status = net_wait;
				gb_countdown = 6000 * 5; //5s

				dat[1] = join_sw;
			}

			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
		}
	}
}

void net_led_status(void)
{
#ifdef tem_hum_light
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED(0);
			delay_ms(300);
			LED(1);
			delay_ms(300);
		}
		else //��ʱ---->>>>>>δ����
		{
			gb_Status = net_error;
			LED(0);
		}
	}
	else if (gb_Status == net_error)
	{
		//if(gb_Status_pre != net_error)
		LED(0);
	}
	else if (gb_Status == net_online)
	{ //if(gb_Status_pre != net_online)
		LED(1);
	}
#endif
#ifdef sensor_md_BODY
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED(0);
			delay_ms(300);
			LED(1);
			delay_ms(300);
		}
		else //��ʱ---->>>>>>δ����
		{
			gb_Status = net_error;
			LED(0);
		}
	}
	else if (gb_Status == net_error)
	{
		//if(gb_Status_pre != net_error)
		LED(0);
	}
	else if (gb_Status == net_online)
	{ //if(gb_Status_pre != net_online)
		LED(1);
	}
#endif
#ifdef power_outlet	
	//����δ�޸�Ϊ�궨�壬��Ҫ�ֶ���д
	if (gb_Status == net_wait)
	{

		if (gb_countdown > 1)
		{
			LED_b(0);
			LED_r(0);
			delay_ms(300);
			LED_r(1);
			delay_ms(300);
		}
		else //��ʱ---->>>>>>δ����
		{
			gb_Status = net_error;
			LED_b(0);
			LED_r(0);
			Rly_Vlue ? LED_r(1) : LED_b(1);
			
		}
	}
	else if (gb_Status == net_error)
	{
		u8 i = 0;
		LED_b(0);
		for ( ; i < 7; i++)
		{
			LED_r(i%2);
			delay_ms(200);
		}
		LED_r(0);
		
	}
	else if (gb_Status == net_online)
	{ 
		u8 i = 0;
		LED_r(0);
		for ( ; i < 7; i++)
		{
			LED_b(i%2);
			delay_ms(200);
		}
		LED_b(0);
	}
#endif
	;
}

u8 chcek_run_status(u8 status)
{

	if (status != 2)
	{
		if (chcek_run != 0)
		{
			delay_ms(11);
			if (chcek_run != 0) //��δ����
				maxcnt++;
			else
			{
				maxcnt = 0;
				//KEY_MID_BLUE(1);
				//delay_ms(1100);
			}

			if (maxcnt > 9)
			{
				maxcnt = 0;
				//curtain_status = 2;
				return 2;
			}
		}
	}

	return status;
}

u16 get_adc(u8 times)
{

	u8 n;
	u32 ad_value = 0;
	// ADC1->CR1  =0x00;    // fADC = fMASTER/2����λ������͹���ģʽ

	for (n = 0; n < times; ++n)
	{

		ad_value += ADC1_GetConversionValue();
		;

		delay_ms(2);
	}

	return ad_value / times;
}

void HAL_GetUID(uint32_t *UID)
{
	UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

void jiance_light_key(void)
{
	u8 i = 0, dat[40];

	dat[0] = 0;
	dat[2] = 0; //

	dat[4] = (u8)MCU_UID[0];
	dat[5] = (u8)(MCU_UID[0] >> 8);
	dat[6] = (u8)(MCU_UID[0] >> 16);
	dat[7] = (u8)(MCU_UID[0] >> 24);

	memset(dat + 8, 0, 8);

	if (LIGHT_1_STATUS == 0)
	{
		KEY_LEFT_BLUE(1);
		//KEY_LEFT_RED(0);
		KEY_LEFT_RED(0);
		//KEY_LEFT_BLUE(1);
	}
	else
	{
		KEY_LEFT_BLUE(0);
		KEY_LEFT_RED(1);
	}

	if (program_num != 2)
	{
		if (LIGHT_2_STATUS == 0)
		{
			KEY_MID_BLUE(1);
			KEY_MID_RED(0);
		}
		else
		{
			KEY_MID_BLUE(0);
			KEY_MID_RED(1);
		}
	}
	else
	{
		KEY_MID_BLUE(0);
		KEY_MID_RED(0);
	}

	if (LIGHT_3_STATUS == 0)
	{
		KEY_RIGHT_BLUE(1);
		KEY_RIGHT_RED(0);
	}
	else
	{
		KEY_RIGHT_BLUE(0);
		KEY_RIGHT_RED(1);
	}

	/////////////////////////////////
	if (KEY_1L == 0)
	{
		alarm_time = 0;
		while (KEY_1L == 0)
		{
			if (KEY_3L == 0)
			{
				//�ж�ʱ��//��ִ������ ����
				if (alarm_time >= 3500)
				{
					//����5S�󣬴���һ������������

					TRAN_info1.data_len = 2;

					dat[0] = 0;
					if (join_flg == 0) //�������5S  �������ؿ����м�ĵ���5��
					{
						join_flg = 1;
						dat[1] = join_sw; // //��������
						KEY_MID_BLUE(0);
						KEY_MID_RED(0);

						for (i = 0; i < 5; i++)
						{
							KEY_LEFT_BLUE(1);
							KEY_LEFT_RED(0);

							KEY_RIGHT_BLUE(0);
							KEY_RIGHT_RED(0);

							LED_time = 300;
							while (LED_time != 0)
								;

							KEY_LEFT_BLUE(0);
							KEY_LEFT_RED(1);

							KEY_RIGHT_BLUE(1);
							KEY_RIGHT_RED(0);

							LED_time = 300;
							while (LED_time != 0)
								;
						}
					}
					else //�������5S  �������عأ��������ߵĵ���5��
					{
						join_flg = 0;
						dat[1] = leave_net; //

						KEY_MID_BLUE(0);
						KEY_MID_RED(0);

						for (i = 0; i < 5; i++)
						{
							KEY_LEFT_BLUE(1);
							KEY_LEFT_RED(1);

							KEY_RIGHT_BLUE(1);
							KEY_RIGHT_RED(1);

							LED_time = 300;
							while (LED_time != 0)
								;

							KEY_LEFT_BLUE(0);
							KEY_LEFT_RED(0);

							KEY_RIGHT_BLUE(0);
							KEY_RIGHT_RED(0);

							LED_time = 300;
							while (LED_time != 0)
								;
						}
					}
					Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
					return;
				}
			}
		}

		if (LIGHT_1_STATUS != 0)
		{
			LIGHT_1_STATUS = 0;

			KEY_LEFT_BLUE(1);
			KEY_LEFT_RED(0);

			RLY1_OUT(RLY_OFF);
		}
		else
		{
			LIGHT_1_STATUS = 1;

			KEY_LEFT_BLUE(0);
			KEY_LEFT_RED(1);

			RLY1_OUT(RLY_ON);
		}

		dat[1] = upload_info;

		if (program_num == 2)
		{

			TRAN_info1.source_dev_num = (DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8);

			TRAN_info1.data_len = 16 + 3;

			dat[3] = DTN_86_LIGHT_2; //

			dat[8 + 8] = 2; //1 2 3  ��ʾ·��

			dat[8 + 8 + 1] = LIGHT_1_STATUS;
			dat[8 + 8 + 2] = LIGHT_3_STATUS;
		}
		if (program_num == 3)
		{

			TRAN_info1.source_dev_num = (DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8);

			TRAN_info1.data_len = 16 + 4;

			dat[3] = DTN_86_LIGHT_3; //

			dat[8 + 8] = 3; //1 2 3  ��ʾ·��

			dat[8 + 8 + 1] = LIGHT_1_STATUS;
			dat[8 + 8 + 2] = LIGHT_2_STATUS;
			dat[8 + 8 + 3] = LIGHT_3_STATUS;
		}

		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

	if (KEY_2L == 0 && program_num == 3)
	{
		while (KEY_2L == 0)
			;

		if (LIGHT_2_STATUS != 0)
		{
			LIGHT_2_STATUS = 0;

			KEY_MID_BLUE(1);
			KEY_MID_RED(0);

			RLY2_OUT(RLY_OFF);
		}
		else
		{
			LIGHT_2_STATUS = 1;

			KEY_MID_BLUE(0);
			KEY_MID_RED(1);

			RLY2_OUT(RLY_ON);
		}
		dat[1] = upload_info;

		TRAN_info1.source_dev_num = (DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8);
		TRAN_info1.data_len = 16 + 4;

		dat[3] = DTN_86_LIGHT_3; //

		dat[8 + 8] = 3; //1 2 3  ��ʾ·��

		dat[8 + 8 + 1] = LIGHT_1_STATUS;
		dat[8 + 8 + 2] = LIGHT_2_STATUS;
		dat[8 + 8 + 3] = LIGHT_3_STATUS;

		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}

	if (KEY_3L == 0)
	{
		while (KEY_3L == 0)
		{
			if (KEY_1L == 0)
			{
				//�ж�ʱ��  ��ִ������ ����

				if (alarm_time >= 3500)
				{

					//����5S�󣬴���һ������������

					TRAN_info1.data_len = 2;

					dat[0] = 0;

					if (join_flg == 0) //�������5S  �������ؿ����м�ĵ���5��
					{
						join_flg = 1;

						dat[1] = join_sw; //����

						KEY_MID_BLUE(0);
						KEY_MID_RED(0);

						for (i = 0; i < 5; i++)
						{
							KEY_LEFT_BLUE(1);
							KEY_LEFT_RED(0);

							KEY_RIGHT_BLUE(1);
							KEY_RIGHT_RED(0);

							LED_time = 300;
							while (LED_time != 0)
								;

							KEY_LEFT_BLUE(0);
							KEY_LEFT_RED(1);

							KEY_RIGHT_BLUE(0);
							KEY_RIGHT_RED(1);

							LED_time = 300;
							while (LED_time != 0)
								;
						}
					}
					else //�������5S  �������عأ��������ߵĵ���5��
					{
						join_flg = 0;
						dat[1] = leave_net; //����

						KEY_MID_BLUE(1);
						KEY_MID_RED(1);

						for (i = 0; i < 5; i++)
						{
							KEY_LEFT_BLUE(1);
							KEY_LEFT_RED(1);

							KEY_RIGHT_BLUE(1);
							KEY_RIGHT_RED(1);

							LED_time = 300;
							while (LED_time != 0)
								;

							KEY_LEFT_BLUE(0);
							KEY_LEFT_RED(0);

							KEY_RIGHT_BLUE(0);
							KEY_RIGHT_RED(0);

							LED_time = 300;
							while (LED_time != 0)
								;
						}
					}
					Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
					return;
				}
			}
		}

		if (LIGHT_3_STATUS != 0)
		{
			LIGHT_3_STATUS = 0;

			KEY_RIGHT_BLUE(1);
			KEY_RIGHT_RED(0); //�ұ�������

			RLY3_OUT(RLY_OFF);
		}
		else
		{
			LIGHT_3_STATUS = 1;

			KEY_RIGHT_BLUE(0);
			KEY_RIGHT_RED(1); //�ұߺ����

			RLY3_OUT(RLY_ON);
		}

		dat[1] = upload_info;

		if (program_num == 2)
		{
			TRAN_info1.source_dev_num = (DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8);
			TRAN_info1.data_len = 16 + 3;

			dat[3] = DTN_86_LIGHT_2; //

			dat[8 + 8] = 2; //1 2 3  ��ʾ·��

			dat[8 + 8 + 1] = LIGHT_1_STATUS;
			dat[8 + 8 + 2] = LIGHT_3_STATUS;
		}
		if (program_num == 3)
		{
			TRAN_info1.source_dev_num = (DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8);
			TRAN_info1.data_len = 16 + 4;

			dat[3] = DTN_86_LIGHT_3; //

			dat[8 + 8] = 3; //1 2 3  ��ʾ·��

			dat[8 + 8 + 1] = LIGHT_1_STATUS;
			dat[8 + 8 + 2] = LIGHT_2_STATUS;
			dat[8 + 8 + 3] = LIGHT_3_STATUS;
		}

		Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
	}
}

/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
void TIM4_Config(void)
{
}

void All_Config(void)
{

	CLK->CKDIVR &= ~(BIT(4) | BIT(3));

	IO_Init();
	UART1_DeInit();
	UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
	UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
	UART1_Cmd(ENABLE);

	HAL_GetUID(MCU_UID);

	TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
	/* Set the Autoreload value */
	TIM4->ARR = (uint8_t)(TIM4_PERIOD);
	TIM4->SR1 = (uint8_t)(~TIM4_FLAG_UPDATE);
	TIM4->IER |= (uint8_t)TIM4_IT_UPDATE;
	TIM4->CR1 |= TIM4_CR1_CEN;

	enableInterrupts();
#ifdef power_outlet
	gb_init_countdown = 6000*5;
	while (gb_init_countdown) {
		net_led_status();
		Rs485_COMM_uart_fuc();
	}
#endif
}

void JDQ_WINDOW_UP(void)
{
	RLY1_OUT(RLY_OFF);
	RLY2_OUT(RLY_ON);
}

void JDQ_WINDOW_DOWN(void)
{
	RLY1_OUT(RLY_ON);
	RLY2_OUT(RLY_OFF);
}

void JDQ_WINDOW_PAUSE(void)
{
	RLY1_OUT(RLY_OFF);
	RLY2_OUT(RLY_OFF);
}

void jiance(void)
{
	u8 i = 0;
	u8 re = 0;
	u8 dat[50];

	dat[0] = 0;
	dat[1] = upload_info; //

	dat[2] = 0;  //
	dat[3] = 51; //   �豸���ͺ� ����ȥ��

	dat[4] = (u8)MCU_UID[0];
	dat[5] = (u8)(MCU_UID[0] >> 8);
	dat[6] = (u8)(MCU_UID[0] >> 16);
	dat[7] = (u8)(MCU_UID[0] >> 24);

	memset(dat + 8, 0, 8);

	TRAN_info1.source_dev_num = (DTN_curtain << 8 | DTN_curtain >> 8);

	//check run status

	re = chcek_run_status(curtain_status);

	if (re == 2)
	{
		if (curtain_status != 2)
		{
			TRAN_info1.data_len = 17;

			dat[1] = upload_info; //
			dat[2] = 0;	   //
			dat[3] = DTN_curtain; //   �豸��

			dat[8 + 8] = 2; // 1���ֽڣ�1�� 0�£�2��ͣ

			curtain_status = 2;
			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len); //��������
												       // delay_ms(1000);
		}

		LED_PAUSE();

		JDQ_WINDOW_PAUSE();
		key_status = 2;

		if (curtain_status != 2)
		{
			TRAN_info1.data_len = 17;
			dat[8 + 8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ
			curtain_status = 2;
			Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
			// delay_ms(1000);
		}
	}

	//	  if( SAMPING_PD2_Pin_RD != 0 )  //�������50MS  ˵����ֹͣ���е�
	//	  {
	//		    if( sampling_time > 300 )
	//		    {
	//			      sampling_time = 300;
	//			      sampling_in_flg = 0;  //ֹͣ
	//		    }
	//	  }
	//	  else
	//	  {
	//		    sampling_time = 0;
	//		    sampling_in_flg = 1;
	//		    motor_ck_time = 500;
	//	  }
	//
	//	  if( motor_ck_time == 0 )
	//	  {
	//		    if( sampling_in_flg == 0 )
	//		    {
	//			      if(curtain_status != 2)
	//			      {
	//				        TRAN_info1.data_len = 17 ;
	//
	//				        dat[1] = upload_info; //
	//				        dat[2] = 0;  //
	//				        dat[3] = DTN_curtain; //   �豸��
	//
	//				        dat[8+8] = 2; // 1���ֽڣ�1�� 0�£�2��ͣ
	//
	//				        curtain_status = 2;
	//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  //��������
	//			        	// delay_ms(1000);
	//			      }
	//
	//			      LED_PAUSE();
	//
	//			      JDQ_WINDOW_PAUSE();
	//
	//			      if( curtain_status != 2 )
	//			      {
	//				        TRAN_info1.data_len = 17   ;
	//				        dat[8+8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ
	//				        curtain_status = 2;
	//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );
	//				        // delay_ms(1000);
	//			      }
	//		    }
	//	  }
	//
	if (DOWN_IN == 0)
	{
		Delay(5000);

		if (DOWN_IN == 0)
		{
			alarm_time = 0;
			motor_ck_time = 1500;

			sampling_time = 0;

			LED_DOWN();

			JDQ_WINDOW_DOWN();

			if (key_status != 0)
			{
				TRAN_info1.data_len = 17;

				dat[2] = 0;	   //
				dat[3] = DTN_curtain; //   �豸��

				dat[8 + 8] = 0; //1���ֽڣ�1�� 0�£�2��ͣ
				curtain_status = 0;
				Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
			}
			key_status = 0; //down
		}
	}
	else if (PAUSE_IN == 0)
	{
		Delay(5000);

		if (PAUSE_IN == 0)
		{

			LED_PAUSE();

			JDQ_WINDOW_PAUSE();

			if (alarm_time >= 3500) //����5S�󣬴���һ������������
			{
				TRAN_info1.data_len = 2;

				dat[0] = 0;
				dat[2] = 0;	   //
				dat[3] = DTN_curtain; //   �豸��

				if (join_flg == 0) //�������5S  �������ؿ����м�ĵ���5��
				{
					join_flg = 1;

					dat[1] = join_sw; // ��������

					//LED_JOIN();
					for (i = 0; i < 5; i++)
					{
						KEY_MID_RED(1);
						KEY_MID_BLUE(0);

						LED_time = 300;
						while (LED_time != 0)
							;

						KEY_MID_RED(0);
						KEY_MID_BLUE(1);

						LED_time = 300;
						while (LED_time != 0)
							;
					}
				}
				else //�������5S  �������عأ��������ߵĵ���5��
				{
					join_flg = 0;
					dat[1] = leave_net; //����

					// LED_JOIN(join_flg);

					for (i = 0; i < 5; i++)
					{
						KEY_LEFT_RED(0);
						KEY_MID_RED(0);
						KEY_RIGHT_RED(0);

						KEY_LEFT_BLUE(1);
						KEY_MID_BLUE(1);
						KEY_RIGHT_BLUE(1);

						LED_time = 300;
						while (LED_time != 0)
							;

						KEY_LEFT_RED(1);
						KEY_MID_RED(0);
						KEY_RIGHT_RED(1);

						KEY_LEFT_BLUE(0);
						KEY_MID_BLUE(1);
						KEY_RIGHT_BLUE(0);

						LED_time = 300;
						while (LED_time != 0)
							;
					}
				}

				Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);

				alarm_time = 0;
			}
			else
			{
				if (key_status != 2)
				{
					TRAN_info1.data_len = 17;

					dat[2] = 0;	   //
					dat[3] = DTN_curtain; //   �豸��

					dat[8 + 8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ

					curtain_status = 2;
					Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
				}
				key_status = 2;
			}
		}
	}
	else if (UP_IN == 0)
	{
		Delay(5000);
		if (UP_IN == 0)
		{
			alarm_time = 0;
			motor_ck_time = 1500;

			sampling_time = 0;

			LED_UP(); //������״̬

			JDQ_WINDOW_UP();

			if (key_status != 1)
			{
				TRAN_info1.data_len = 17;

				dat[2] = 0;	   //
				dat[3] = DTN_curtain; //   �豸��

				dat[8 + 8] = 1; //1���ֽڣ�1�� 0�£�2��ͣ
				curtain_status = 1;
				Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat, TRAN_info1.data_len);
			}

			key_status = 1; //up
		}
	}
	else
	{
		alarm_time = 0;
	}
}

void IO_Init(void)
{

#ifdef sensor_md_BODY

	GPIOA->DDR |= Pin(3); //���ģʽ
	GPIOA->CR1 |= Pin(3); //�������

	GPIOA->DDR &= ~Pin(1); //����ģʽ
	GPIOA->DDR &= ~Pin(2); //����ģʽ

	//#else

	//mq5ú����������fire���洫����
	GPIOA->DDR |= Pin(3); //���ģʽ
	GPIOA->CR1 |= Pin(3); //�������

	GPIOC->DDR |= Pin(7); //���ģʽ
	GPIOC->CR1 |= Pin(7); //�������

	GPIOC->DDR &= ~Pin(6); //����ģʽ
	GPIOD->DDR &= ~Pin(2); //����ģʽ
	GPIOD->DDR &= ~Pin(3); //����ģʽ

	GPIOD->CR1 &= ~Pin(2); //��������
	//ADC1 channel3
	ADC1_DeInit();
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3,
		  ADC1_PRESSEL_FCPU_D2, ADC1_EXTTRIG_TIM, DISABLE,
		  ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);

	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
	BEEP_s(0);

	LED(0);

#endif

#ifdef tem_hum_light

	//KEY
	GPIOC->DDR &= ~Pin(6); //����ģʽ
	//led1
	GPIOC->DDR |= Pin(7); //���ģʽ
	GPIOC->CR1 |= Pin(7); //�������
	LED(0);
#endif

#ifdef power_outlet

	//KEY
	GPIOA->DDR &= ~GPIO_PIN_1; //����ģʽ
	//led_r
	GPIOD->DDR |= GPIO_PIN_2; //���ģʽ
	GPIOD->CR1 |= GPIO_PIN_2; //�������
	//led_b
	GPIOD->DDR |= GPIO_PIN_3; //���ģʽ
	GPIOD->CR1 |= GPIO_PIN_3; //�������
	//z_rest
	GPIOD->DDR |= GPIO_PIN_4; //���ģʽ
	GPIOD->CR1 |= GPIO_PIN_4; //�������
	//Rly
	GPIOC->DDR |= GPIO_PIN_7; //���ģʽ
	GPIOC->CR1 |= GPIO_PIN_7; //�������
	LED_r(0);
	LED_b(0);
	Rly(0);
#endif

	//����-TX
	GPIOD->DDR |= BIT(5); //���ģʽ
	GPIOD->CR1 |= BIT(5); //�������

	//����-RX
	GPIOD->DDR &= ~BIT(6); //����ģʽ

	
}

/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}

/**
  * ������ʱ
  *
  *
  *
  */

void delay_ms(uint16_t nCount)
{
	u16 i = 0, n = 0;

	for (i = 0; i < nCount; i++)
	{
		n = 1000;
		while (n != 0)
		{
			n--;
		}
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
