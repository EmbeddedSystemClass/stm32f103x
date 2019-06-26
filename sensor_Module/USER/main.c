/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ������ʵ�����Ŀ
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-06-20 09:22:43
 * @Description: 
 *��������
 *---------------------------------------------------------------------------*/

#include "global.h"

u32 MY_DEST_ADDR = 0;

u8 POWER_220V_FLG = 0; //AC_CHECK

u16 My_vol;
u16 My_curr;
u16 My_watt;
u32 My_powr;

u8 search_addr_flg = 0;
u16 alarm_time = 0;
u16 send_urt_time = 0;

u8 gb_up_time_out = 0;
u32 gb_HX711_calibration = 0;
u8 global_mode = 0;
const char swicth_code[10] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};
u8 dat1[8] = {01, 03, 00, 0x48, 00, 05, 05, 0xdf};

uint32_t MCU_UID[3];
bool online = false;
bool gb_status_net = false; //��������״̬
__O short gb_fan_lv = 2;

void aiction(u8 type, u8 type_pre);
void ui_main_sw(u8 type, u8 type_pre);
void updata(u8 *data, u8 len);
void init_zigbee(void);
void zigbee_login(bool);

void HAL_GetUID(uint32_t *UID)
{
	UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

int main(void)
{
	volatile TRAN_D_struct TRAN_info1;
	volatile u16 i = 0;
	volatile u8 dat[8] = {01, 03, 00, 0x48, 00, 05, 05, 0xdf};

	volatile u8 SoilDetectorDat[8] = {Soil_Detector_Addr, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB}; //������ʪ����ѯ֡
	volatile u8 AnemographDat[8] = {Anemograph_Addr, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};      //���ټƷ�����ѯ֡

	u8 check_Zigbee_buff = 2;

	/*test*/

	u8 swicth_type_pre;
	volatile u32 distance = 0;
	//	u32 timetest = 0;
	//	u32 timetestttt = 0;
	/***test_end***/
	delay_init();	 //��ʱ������ʼ��
	NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	BSP_init();

	// VAL_init();

	HAL_GetUID(MCU_UID);
	//��õ�Ƭ��Ψһ��

	//	delay_ms(1000);

	uart1_init(4800);   //�뽻����ѹ�����ƽ���  ���������ѹ�����Ƴ���Ĭ��4800  ���Ը��Ĳ�����
	uart2_init(115200); //debug
	uart3_init(115200); //debug

	TIM2_Int_Init(9, 7199); //72M/7200   10Khz�ļ���Ƶ�ʣ�������9Ϊ1ms

	Key_Init(); //KEY1 KEY2��ʼ��
	Adc_Init();
	Iic_GpioConfig();

	Fan_Init(2000);
	CodeSwtich_Init();
	SwitchType = GetSwitchType();
	Beep_Init();

	/*
	4.4.4 ֱ������ָ�����������
	��1����ȡ�����ָ���ģ���ַΪ 0x01 ��������
	�������ݣ�01 03 00 48 00 05 05 DF ���� 0048 ��ʼ�� 5 ���Ĵ�����
	�������ݣ�01 03 0A 02 6C 11 D7 01 1B 01 02 03 04 DB 6F
	��ɫ����Ϊ��ص�������ֽ�����Ľ������ͼ��
	�ֽ����ݣ��õ��Ľ��Ϊ����ѹ=0x026C/100=620/100=6.2V
	����=0x11D7/1000=4567/1000=4.567A
	����=0x011B/10=283/10=28.3W
	����=0x01020304/100=16909060/100=169090.6Wh
	*/

	//�豸������ѯ ��һ����ַ
	alarm_time = 3000;
	search_addr_flg = 0;

	send_urt_time = 100; //�������ܲ���� ��������������

	/*STH1Xģ���ʼ��*/
	SHT1X_Config();
	//	SHT1x_Init();
	//	SHT1x_Config();

	//����ģ���ʼ��
	HX711_Init();

	/*HC_SR04���ģ���ʼ��*/
	//HcSr04_Init();
	//HcSr04Start();

	Relay_Init();

	//Һ������ʼ��
	LCD144_Init();
	/*��������2000ms*/
	//BeepOn(2000);
	LCD_1_44_Clear(BLACK);

	init_zigbee();

	while (1)
	{
		u16 i = 0;

		//����
		if (SwitchType != GetSwitchType())
		{
			delay_ms(20);
			if (SwitchType != GetSwitchType())
				SwitchType = GetSwitchType();
		}
		//����
		if (SwitchType != 0x02)
		{
			while (KEY_1_VALUE() == 0 && i < 110)
			{
				delay_ms(20);
				i++;
			}
			if (i > 100 && !gb_status_net) //���� 2S
			{
				//printf("����������Ч��\n");
				zigbee_login(online);
				gb_status_net = true; //�ڶ�ʱ���ж���ʵ��LED��˸
			}
		}

		//�л�����

		aiction(SwitchType, swicth_type_pre);

		if (swicth_type_pre != SwitchType)
		{
			swicth_type_pre = SwitchType;
		}

		android_uart_fuc();
		zigbee_uart_fuc();
		if (check_Zigbee != check_Zigbee_buff)
		{
			check_Zigbee_buff = check_Zigbee;
			LCD_1_44_ClearS(BLACK, 127 - 6 * 10, 5, 127, 20); //
			if (check_Zigbee)
			{
				Show_Str(127 - 6 * 6, 5, RED, YELLOW, "msg: Z", 12, 1);
			}
			else
			{
				Show_Str(127 - 6 * 6, 5, RED, YELLOW, "msg:--", 12, 1);
			}
		}
	}
}
//zigbee����
void updata(u8 *data, u8 len)
{
	TRAN_D_struct TRAN_info;

	memset(&TRAN_info, 0, sizeof(TRAN_D_struct));

	memcpy(TRAN_info.source_addr, MCU_UID, 3);
	TRAN_info.TYPE_NUM = 0x04;
	TRAN_info.data_len = len;
	android_SD_load_buf(0xaaaa, 0xbbbb, &TRAN_info, data, TRAN_info.data_len);
	android_dma_send_buf();
	if (online)
	{
		zigbee_SD_load_buf(0xaaaa, 0xbbbb, &TRAN_info, data, TRAN_info.data_len);
		zigbee_dma_send_buf();
	}
	//printf("UPDATA");
}
//ҳ���л�
void aiction(u8 type, u8 type_pre)
{

	char disp[30];
	unsigned char sendbuff[30];

	u32 result = 0; //���շ���ֵ
	//�����棬��ʵʱ
	ui_main_sw(type, type_pre);
	sendbuff[1] = 0x04;		       //CMD----��������
	sendbuff[3] = swicth_code[SwitchType]; //�豸����

	switch (type)
	{
	case 0:
	{
		if (!KEY_2_VALUE())
		{
			while (!KEY_2_VALUE())
				delay_ms(10);
			PAout(15) = ~PAout(15);
			//PAout(0) = ~ PAout(0);

			//printf("12345678");
			LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%d", (bool)PAout(15));
			LCD_1_44_ShowNum2412(53, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

			sendbuff[16] = (bool)PAout(15); //�豸״̬
			updata(sendbuff, 16 + 1);
		}
	}
	break;
	case 1:
	{
		if (!KEY_2_VALUE())
		{
			while (!KEY_2_VALUE())
				delay_ms(10);
			PAout(15) = ~PAout(15);

			LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%d", (bool)PAout(15));
			LCD_1_44_ShowNum2412(53, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

			sendbuff[16] = (bool)PAout(15); //�豸״̬
			updata(sendbuff, 16 + 1);
		}
	}
	break;
	case 2:
	{
		//				if((result = Get_Adc_Average(5,10)) != 0)//AD���ܱ�ʾ��ǰ����
		//				{
		//
		//
		//					LCD_1_44_ClearS(BLACK, 53, 75, 96, 127);//5+48 50 2*24 12
		//					sprintf(disp, ":%.2d",(result/41));
		//					LCD_1_44_ShowNum2412(5+48,75,GREEN,YELLOW,(uint8_t *)disp,24,1);
		//
		//				}

		if (!KEY_2_VALUE() || !KEY_1_VALUE())
		{
			delay_ms(10);

			if (!KEY_2_VALUE()) //+10%
			{
				while (!KEY_2_VALUE())
					delay_ms(10);
				gb_fan_lv++;
				gb_fan_lv = gb_fan_lv > 9 ? 0 : gb_fan_lv;
			}

			if (!KEY_1_VALUE()) //-10%
			{
				while (!KEY_1_VALUE())
					delay_ms(10);
				gb_fan_lv--;
				gb_fan_lv = gb_fan_lv < 0 ? 9 : gb_fan_lv;
			}

			LCD_1_44_ClearS(BLACK, 5, 100, 96, 127); //5+48 50 2*24 12

			if (gb_fan_lv > 0 && gb_fan_lv < 9)
				sprintf(disp, "< %d >", gb_fan_lv);
			else if (gb_fan_lv == 0)
				sprintf(disp, "  %d >", gb_fan_lv);
			else if (gb_fan_lv == 9)
				sprintf(disp, "< %d  ", gb_fan_lv);

			Show_Str(5 + 30, 100, GREEN, YELLOW, (u8 *)disp, 24, 1);

			ChangeFanSpeed(99 * gb_fan_lv);

			memcpy(sendbuff + 16, (void *)(&gb_fan_lv), 1); //�豸״̬
			updata(sendbuff, 16 + 1);
		}
	}

	break;
	case 3:
	{

		if ((result = SHT1X_GetValue()) != 0)
		{

			LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%.0f", temp_val_real);
			LCD_1_44_ShowNum2412(5 + 48, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

			LCD_1_44_ClearS(BLACK, 53, 100, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%.0f", humi_val_real);
			LCD_1_44_ShowNum2412(5 + 48, 100, GREEN, YELLOW, (uint8_t *)disp, 24, 1);
			result = 0;
		}

		delay_ms(500);

		memcpy(sendbuff + 16, &temp_val_real, 4);     //�豸״̬
		memcpy(sendbuff + 16 + 4, &humi_val_real, 4); //�豸״̬
		updata(sendbuff, 16 + 4 + 4);
	}

	break;
	case 4:
	{

		if ((result = Get_Adc_Average(6, 10)) != 0)
		{

			LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%d", (result / 41));
			LCD_1_44_ShowNum2412(5 + 48, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);
		}

		delay_ms(500);

		memcpy(sendbuff + 16, &result, 4); //�豸״̬
		updata(sendbuff, 16 + 4);
		result = 0;
	}
	break;
	case 5:
	{
		LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
		sprintf(disp, ":%.0f", SoilTemp);
		LCD_1_44_ShowNum2412(5 + 48, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

		LCD_1_44_ClearS(BLACK, 53, 100, 96, 127); //5+48 50 2*24 12
		sprintf(disp, ":%.0f", SoilHum);
		LCD_1_44_ShowNum2412(5 + 48, 100, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

		delay_ms(500);

		memcpy(sendbuff + 16, &SoilTemp, 4);    //�豸״̬
		memcpy(sendbuff + 16 + 4, &SoilHum, 4); //�豸״̬
		updata(sendbuff, 16 + 4 + 4);
		//��ʱ��ѯ
		if (alarm_time > 2000)
		{
			alarm_time = 0;
			if (SwitchType == 0x05 || SwitchType == 0x06)
			{
				RS485_V_I_SD_load_buf2((u8 *)dat1, 8);
				RS485_V_I_dma_send_buf();
			}
		}
	}

	break;
	case 6:
	{

		LCD_1_44_ClearS(BLACK, 5, 100, 96, 127); //5+48 50 2*24 12
		sprintf(disp, "%.2f", WindSpeed);
		LCD_1_44_ShowNum2412(5 + 12, 100, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

		delay_ms(500);

		memcpy(sendbuff + 16, &WindSpeed, 4); //�豸״̬
		updata(sendbuff, 16 + 4);
		result = 0;
		//��ʱ��ѯ
		if (alarm_time > 2000)
		{
			alarm_time = 0;
			if (SwitchType == 0x05 || SwitchType == 0x06)
			{
				RS485_V_I_SD_load_buf2((u8 *)dat1, 8);
				RS485_V_I_dma_send_buf();
			}
		}
	}

	break;
	case 7:
	{

		//HcSr04Start();

		if ((result = GetHcSr04Value()) != 0)
		{
			if (result > 400)
			{
				LCD_1_44_ClearS(BLACK, 0, 100, 96, 127); //5+48 50 2*24 12
				Show_Str(5, 106, RED, YELLOW, "Over Range", 24, 1);
			}
			else
			{
				LCD_1_44_ClearS(BLACK, 0, 100, 96, 127); //5+48 50 2*24 12
				sprintf(disp, "%d", result);
				LCD_1_44_ShowNum2412(30, 100, GREEN, YELLOW, (uint8_t *)disp, 24, 1);
			}

			memcpy(sendbuff + 16, &result, 4); //�豸״̬
			updata(sendbuff, 16 + 4);

			result = 0;
		}

		delay_ms(100);
		//HcSr04_Init();
		HcSr04Start();
		delay_ms(20);
	}

	break;
	case 8:
	{
		if ((result = Get_Adc_Average(6, 10)) != 0)
		{

			LCD_1_44_ClearS(BLACK, 53, 75, 96, 127); //5+48 50 2*24 12
			sprintf(disp, ":%d", 99 - result / 41);
			LCD_1_44_ShowNum2412(5 + 48, 75, GREEN, YELLOW, (uint8_t *)disp, 24, 1);
		}

		delay_ms(500);

		memcpy(sendbuff + 16, &result, 4); //�豸״̬
		updata(sendbuff, 16 + 4);
		result = 0;
	}

	break;
	case 9:
	{
		float buff = 0;

		result = 0;
		if ((result = Read_HX711()) != 0)
		{
			result -= gb_HX711_calibration;
			LCD_1_44_ClearS(BLACK, 5, 100, 96, 127); //5+48 50 2*24 12
			result = 256 * (result > 50300 ? result - 50300 : 50300 - result) / 100000;
			sprintf(disp, "%d.%.4d", result / 1000, result % 1000);
			LCD_1_44_ShowNum2412(5, 100, GREEN, YELLOW, (uint8_t *)disp, 24, 1);

			buff = (float)result / 1000.0;
		}
		if (!KEY_2_VALUE())
		{
			u8 key_buff = 0;
			while (!KEY_2_VALUE())
			{
				delay_ms(10);
				key_buff++;
			}

			if (key_buff > 1 && key_buff < 5)
			{

				gb_HX711_calibration = result;
			}
		}

		if (gb_up_time_out < 1)
		{
			gb_up_time_out = 50;
			memcpy(sendbuff + 16, &buff, 4); //�豸״̬
			updata(sendbuff, 16 + 4);
		}
		else
		{
			delay_ms(10);
			gb_up_time_out--;
		}
	}

	break;
	default:

		break;
	}
}
void ui_main_sw(u8 type, u8 type_pre)
{

	if (type_pre != type)
	{
		char disp[30];

		LCD_1_44_Clear(BLACK);

		sprintf(disp, "sw_code:%d ", type);
		Show_Str(5, 5, RED, YELLOW, (uint8_t *)disp, 12, 1);
		if (check_Zigbee)
		{
			Show_Str(127 - 6 * 6, 5, RED, YELLOW, "msg: Z", 12, 1);
		}
		else
		{
			Show_Str(127 - 6 * 6, 5, RED, YELLOW, "msg:--", 12, 1);
		}

		//���PWM
		if (type != 2)
		{

			FanPowerOff();
		}
		if (type != 7)
		{
			HcSr04_OFF();
		}

		switch (type)
		{
		case 0:
		{

			Show_Str(5, 50, YELLOW, YELLOW, "Relay_Light", 24, 1);
			Show_Str(5, 75, GREEN, YELLOW, "״̬", 24, 1);
			Show_Str(55, 80, WHITE, BLACK, " X X", 24, 1);
		}
		break;
		case 1:
		{

			Show_Str(5, 50, YELLOW, YELLOW, "Relay_Door", 24, 1);
			Show_Str(5, 75, GREEN, YELLOW, "״̬", 24, 1);
			Show_Str(55, 80, WHITE, BLACK, " X X", 24, 1);
		}
		break;
		case 2:
		{
			FanPowerOn(99 * gb_fan_lv);
			Show_Str(5, 50, YELLOW, YELLOW, "FAN_PWM", 24, 1);
			Show_Str(5, 75, GREEN, YELLOW, "�����n   ", 24, 1);
			Show_Str(5 + 30, 100, GREEN, YELLOW, "< 2 >", 24, 1);
		}

		break;
		case 3:
		{
			Show_Str(5, 50, YELLOW, YELLOW, "AIR_Tem&Hum", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "�¶��n  ��", 24, 1);
			Show_Str(5, 100, GREEN_L, YELLOW, "ʪ���n  ��", 24, 1);
		}

		break;
		case 4:
		{

			Show_Str(5, 50, YELLOW, YELLOW, "Liquid_Lv", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "ˮλ      �M", 24, 1);
		}
		break;
		case 5:
		{
			Show_Str(5, 50, YELLOW, YELLOW, "Soil_Tem&Hum", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "�¶��n  ��", 24, 1);
			Show_Str(5, 100, GREEN_L, YELLOW, "ʪ���n  ��", 24, 1);
		}

		break;
		case 6:
		{
			Show_Str(5, 50, YELLOW, YELLOW, "Wind_Speed", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "�����n     ", 24, 1);
			Show_Str(96, 100 + 6, GREEN_L, YELLOW, "M/S", 24, 1);
		}

		break;
		case 7:
		{
			//�M
			Show_Str(5, 50, YELLOW, YELLOW, "Range_Finding", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "�����n", 24, 1);
			Show_Str(96, 100, GREEN_L, YELLOW, "�M", 24, 1);
			//HcSr04Start();
		}

		break;
		case 8:
		{
			Show_Str(5, 50, YELLOW, YELLOW, "Range_Pa", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "ѹ��      ", 24, 1);
			Show_Str(96, 75 + 8, GREEN_L, YELLOW, "N", 24, 1);
		}

		break;
		case 9:
		{
			Show_Str(5, 50, YELLOW, YELLOW, "Range_weigh", 24, 1);
			Show_Str(5, 75, GREEN_L, YELLOW, "�����n ", 24, 1);
			Show_Str(96, 100, GREEN_L, YELLOW, "�K", 24, 1);
		}

		break;
		default:

			break;
		}
	}
}
//zigbee��ʼ��
void init_zigbee(void)
{

	Zigbee_REST = 0;
	delay_ms(10);
	Zigbee_REST = 1;
	delay_ms(20);
}
//zigbee����or����

void zigbee_login(bool online_status)
{
	TRAN_D_struct joinTRAN;
	u8 data[2];
	joinTRAN.TYPE_NUM = 0x03; //��ģ��stm32--->·����zigbee

	joinTRAN.data_len = 2;
	data[0] = 0;
	if (online_status)
	{
		data[1] = 2; //����
	}
	else
	{

		data[1] = 1;
	}

	zigbee_SD_load_buf(0xaaaa, 0xbbbb, &joinTRAN, data, joinTRAN.data_len);
	zigbee_dma_send_buf();
}