/*************************************
   GTA-GXU01 ʵѵ���������ذ�
          2018-9-18 
					by: ������					
  
	����ʹ�ã�
	LED:   LED  PB9 ; 	
	
	BEEP: pc4
	
	���ڣ�RS485 (�ӵ�ѹ������)    		UART1		   PA9 PA10    con_pin1  PA8
	      RS485(ͨ��)            	UART2   PA2  PA3    con_pin2  PA4
	  
  AC_CHECK    220V�Ƿ��ͨ����

  AC_con      220V �̵�������

****************************************/
#include  "global.h"

u32   MY_DEST_ADDR=0;

u8    POWER_220V_FLG=0;  //AC_CHECK

u16   My_vol; 
u16		My_curr;  
u16   My_watt; 
u32		My_powr;


u8    search_addr_flg=0;
u16   alarm_time=0;
u16   send_urt_time=0;

u8    global_mode=0;

volatile u8 gb_is_DMA_send_OK = 1;

volatile u8 gb_time_count_send = 0;

volatile u8 gb_J_status ;



uint32_t MCU_UID[3];

//
void HAL_GetUID(uint32_t *UID)
{
	UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 
/**
  * @brief  Updata_J_status,��uart2_485�ϴ��̵���״̬
  * @param  0 ��1
  * @retval None
  */

void Updata_J_status(char status)
{
	TRAN_D_struct   TRAN_info;
	u8	data[3] = {0,3,0 };

	data[2] = status;

	TRAN_info.dest_dev_num = ( DTN_android << 8 | DTN_android >> 8 );
					
	TRAN_info.dest_addr[0] = (u8) DTN_android ;
	TRAN_info.dest_addr[1] = (u8) (DTN_android >> 8) ;
	TRAN_info.dest_addr[2] = 0x11;
	TRAN_info.dest_addr[3] = 0x11;
	
	TRAN_info.source_dev_num = ( DTN_power_manage << 8 | DTN_power_manage >> 8 );
	
	TRAN_info.source_addr[0] = (u8)MCU_UID[0];
	TRAN_info.source_addr[1] = (u8)(MCU_UID[0]>>8);
	TRAN_info.source_addr[2] = (u8)(MCU_UID[0]>>16);
	TRAN_info.source_addr[3] = (u8)(MCU_UID[0]>>24);
	
	TRAN_info.TYPE_NUM = 2;   
	TRAN_info.data_len = 3;


	Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info , data ,TRAN_info.data_len );  
}

int main(void)
{  
	TRAN_D_struct   TRAN_info1;
	u16	i = 0;
	
	u8  dat[8] = { 01,03, 00 ,0x48, 00 ,05 ,05 ,0xdf }; 

	u8  dat1[8] = { 01,03, 00 ,0x48, 00 ,05 ,05 ,0xdf };
	
	delay_init();	        	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//BSP_init(); //��ȥ��AC_CHECK����Դ�������
	//
	LED_Init();
	// VAL_init();

	HAL_GetUID(MCU_UID);     //��õ�Ƭ��Ψһ��

	delay_ms(1000);

	uart1_init(4800);		//�뽻����ѹ�����ƽ���  ���������ѹ�����Ƴ���Ĭ��4800  ���Ը��Ĳ����� 
	uart2_init(57600);	//��RS485_COMM ���ڽ��� 

	TIM2_Int_Init(9,7199);//72M/7200   10Khz�ļ���Ƶ�ʣ�������9Ϊ1ms 
	
	IWDG_Config(IWDG_Prescaler_64 ,625);
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
	
	gb_J_status = Read_Flash_J_status();
				  
	//�豸������ѯ ��һ����ַ   
	alarm_time = 3000;
	search_addr_flg = 0; 

	send_urt_time = 100;//�������ܲ���� ������������		 
	
	while(1)
	{ 
		if(search_addr_flg == 0)//
		{ 
			if( alarm_time > 2000 )//test
			//if(0)
			{  
				alarm_time = 0;
				//LED2 = 0; 	
				TRAN_info1.dest_dev_num = ( DTN_android << 8 | DTN_android >> 8 );
				
				TRAN_info1.dest_addr[0] = 0x00;
				TRAN_info1.dest_addr[1] = 0x00;
				TRAN_info1.dest_addr[2] = 0x00;
				TRAN_info1.dest_addr[3] = 0x00;

				TRAN_info1.source_dev_num = ( DTN_power_manage << 8 | DTN_power_manage >> 8 );

				TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
				TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM = 1;   //��һ��
				TRAN_info1.data_len = 2;

				dat[0] = 00;  
				dat[1] = 02;   //��ȡ��һ���豸����ʶ��ź͵�ַ CMD 0X0002   ���� 6���ֽ�

				Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
							  		
				LED2 = 0;	 
				for(i=0; i<250; i++)
				{	 
					BEEP = 0; 	  //  �����	 
					delay_us(150); 
					BEEP = 1; 	  //  ���   
					delay_us(150); 
				}
				LED2 = 1; 	
				//Updata_J_status( gb_J_status);//�ϴ��̵���״̬
				
			}						 
		}
		else
		{
		
			if( alarm_time > 3000 )
			{  
				gb_time_count_send++;
				alarm_time = 0;
				RS485_V_I_SD_load_buf2( dat1 ,8 ); 
				////��ȥ��AC_CHECK����Դ�������
				//POWER_220V_FLG = 0;//3S û���ж�  ����������־λ
				RS485_V_I_dma_send_buf();
			} 
		}
		Rs485_COMM_uart_fuc() ;
		if(gb_time_count_send < 30)//gb_is_DMA_send_OK == 1 && 
		{
			IWDG_Feed();
		}

		if( send_urt_time == 0 )
		{
			Rs485_COMM_dma_send_buf(); 
		}
  }
}
