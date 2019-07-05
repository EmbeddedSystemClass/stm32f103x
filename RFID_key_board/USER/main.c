/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ����Ŀ
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-07-05 14:09:28
 * @Description: 
 * ��������ʵѵ�ҽ�̨��尴�����������
 * ---------------------------------------------------------------------------*/


#include "global.h"

u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;


uint32_t         MCU_UID[3];

u8               LED_DAT[2];

u8               m_mode = 0;

u16              join_LED_S = 0;

u16              send_urt_time = 0;

u16              PC_ON_TIME = 0;
 

u8  main_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i = 0;

	for(i=0;i<3;i++)
	{
		if( p[i] != 0 )
		{
			return TRUE;
		} 
	}
	return FALSE;
}

void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 
/*
 * ���� IWDG �ĳ�ʱʱ��
 * Tout = prv/40 * rlv (s)
 *      prv������[4,8,16,32,64,128,256]
 * prv:Ԥ��Ƶ��ֵ��ȡֵ���£�
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:Ԥ��Ƶ��ֵ��ȡֵ��ΧΪ��0-0XFFF
 * �������þ�����
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
 */

void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// ʹ�� Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// ����Ԥ��Ƶ��ֵ
	IWDG_SetPrescaler( prv );
	
	// ������װ�ؼĴ���ֵ
	IWDG_SetReload( rlv );
	
	// ����װ�ؼĴ�����ֵ�ŵ���������
	IWDG_ReloadCounter();
	
	// ʹ�� IWDG
	IWDG_Enable();	
}

// ι��
void IWDG_Feed(void)
{
	// ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
	// ����������ֵ����0��ʱ������ϵͳ��λ
	IWDG_ReloadCounter();
}

 int main(void)
{  
	TRAN_D_struct   TRAN_info1;

	u8 M_tagfounds = 1;

	delay_init();	        	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	HAL_GetUID(MCU_UID);     //��õ�Ƭ��Ψһ��

	LED_Init();			       	//LED�˿ڳ�ʼ�� 
	BEEP_INIT(); 
	KEY_Init(); 
	uart3_init(57600);
	
	TIM2_Int_Init(9,72);//72M/72   1000Khz�ļ���Ƶ�ʣ�������9Ϊ10us 

	LED_DAT[0] = 0x00;
	LED_DAT[1] = 0x00;
	write_595_LED( LED_DAT,2);
	
	IWDG_Config(IWDG_Prescaler_64,625);
	Interrupts_Config(); 

	CR95HF_ConfigManager_HWInit();
	
	while(1)
	{  
		// 	disable_all_uart_interupt(   ); 
		CR95HF_FUC( M_tagfounds );    //ˢ����  drv95HF_SPIPollingCommand 
		(M_tagfounds>=TRACK_NFCTYPE5)?( M_tagfounds=TRACK_NFCTYPE1 ):( M_tagfounds*=2  );
		// enable_all_uart_interupt(   );
		key_fuc();

		RS485_uart_fuc(); 
		RS485_dma_send_buf();
		IWDG_Feed();
		if( key_tab[5] == 1 )//=1 ���ڿ���  ��LED��
		{
			if(PC_ON_TIME >= 1000)
			{
				PC_ON_TIME = 0;

				if( LED_DAT[1] & L_R2C2 )
				{
					LED_DAT[1] &=~ L_R2C2;
				}
				else
				{
					LED_DAT[1] |= L_R2C2;
				}

				LED_DAT[0] = 0x00;

				write_595_LED( LED_DAT,2);
			}
		}
	}
}
