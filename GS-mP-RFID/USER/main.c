/*************************************
  
****************************************/
#include "global.h"

 
u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;

uint32_t 	MCU_UID[3];

u8    		LED_DAT[2];

u8   			m_mode = 0;

u16     	join_LED_S = 0;

u16    		send_urt_time = 0;


u8  main_CHECK_BUF_VALID(u8 *p) 
{  
	u8  i = 0;

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



int main(void)
{  
	TRAN_D_struct   TRAN_info1;

	u8  M_tagfounds = 1;

	delay_init();	        	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	HAL_GetUID(MCU_UID);     //��õ�Ƭ��Ψһ��

	LED_Init();			       //LED�˿ڳ�ʼ�� 
	BEEP_INIT() ; 
	KEY_Init(); 
	uart3_init(57600);

	TIM2_Int_Init(9,72);//72M/72   1000Khz�ļ���Ƶ�ʣ�������9Ϊ10us 

	Interrupts_Config(); 

	CR95HF_ConfigManager_HWInit();
 
	while(1)
	{  
		// 	disable_all_uart_interupt(   ); 
		CR95HF_FUC( M_tagfounds );    //ˢ����  drv95HF_SPIPollingCommand 
		(M_tagfounds>=TRACK_NFCTYPE5)?( M_tagfounds=TRACK_NFCTYPE1 ):( M_tagfounds*=2  );
		// enable_all_uart_interupt(   );
		// key_fuc();
		// RS485_uart_fuc( ); 
		RS485_dma_send_buf();
	}
}
