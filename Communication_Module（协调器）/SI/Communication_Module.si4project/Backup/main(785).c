/*************************************
���ƣ�������ͨѶ��չ��
ʱ�䣺2019-05-14
���ߣ�Ƚ����
��Ҫ��
****************************************/
#include "global.h"


 
u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;

extern u8 gb_dat[18];

uint32_t         	MCU_UID[3];

IR_CAP_struct    	IR_CAP1;

u8               	IR_CAP_START_FLG = 0;

u8              	m_mode = 0;

u16              	join_LED_S = 0;

u16              	send_urt_time = 0;

u8               	JDQ_NOW_STATUS = 0;
u8               	JDQ_WILL_STATUS = 0;

u16             	JDQ_time = 0;

u8              	PC_KEY_FLAG;


void IR_js( void);
void  remote_cap_study( void  );
void HAL_GetUID(uint32_t *UID);
void  IR_send_waveform( u8 CAP_IR_num  ) ;
extern void TIM2_Int_Init(u16 arr,u16 psc);


void  Init(void) ;

void cnt_delay(u16 num)
{
  u16 i,j;
  for(i=0;i<num;i++)
    for(j=0;j<0x800;j++);
}


int main(void)
{
    	TRAN_D_struct   TRAN_info1;
	 u8   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
	 u8 KEY_VLUE = 255;
	Init();
	
	if(1)
	{
		LCD_1_44_Clear(BLACK); //����

		

		Show_Str(32,5,BLUE,WHITE,"ϵͳ���",16,0);

		Show_Str(5,25,RED,YELLOW,"�¶�     ��",24,1);
		LCD_1_44_ShowNum2412(5+48,25,RED,YELLOW,":24",24,1);

		Show_Str(5,50,YELLOW,YELLOW,"ʪ��     ��",24,1);
		LCD_1_44_ShowNum2412(5+48,50,YELLOW,YELLOW,":32",24,1);

		Show_Str(5,75,WHITE,YELLOW,"��ѹ      ��",24,1);
		LCD_1_44_ShowNum2412(5+48,75,WHITE,YELLOW,":3.2",24,1);
			
		Show_Str(5,100,GREEN,YELLOW,"����      ��",24,1);
		LCD_1_44_ShowNum2412(5+48,100,GREEN,YELLOW,":0.2",24,1);
		
		
	}
	
	printf("this is a test\n");
	while (1)
		{

					 

			if (KEY2 == 0)
			{
				delay_ms(20);
				while(KEY2 == 0)
					{
					delay_ms(20);
					}
				KEY_VLUE = !KEY_VLUE;
				LED2 = ~LED2;				
			}


			if(KEY_VLUE == 1)
			{
				int i,n;
				TRAN_info1.dest_dev_num = 0;

				TRAN_info1.dest_addr[0] = 0x00;
				TRAN_info1.dest_addr[1] = 0x00;
				TRAN_info1.dest_addr[2] = 0x00;
				TRAN_info1.dest_addr[3] = 0x00;

				TRAN_info1.source_dev_num = 0;

				TRAN_info1.source_addr[0] = 0;
				TRAN_info1.source_addr[1] = 0;
				TRAN_info1.source_addr[2] = 0;
				TRAN_info1.source_addr[3] = 0;

				TRAN_info1.TYPE_NUM = 4;   //��4��
				TRAN_info1.data_len = 3; 

				dat[0] = 0;
				dat[1] = 1;
				dat[2] = 60;
				join_LED_S = 60 ;

				Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len ); 
				delay_ms(100);
				Android_dma_send_buf(); 
				printf("��������:\n");
					for(n = 0; n < 5; n++ )
					{	
						if(Android_CHECK_BUF_VALID(UART3_android_st.SDBuff[n])==TRUE)
						for(i = 0; i < 23; i++)
									printf("%3.2x",UART3_android_st.SDBuff[n][i]);

							printf("\n");
						}
				KEY_VLUE = !KEY_VLUE;
			}

			if(UART3_android_st.uart_Rx_lun > 0)
			{
			
				int i,n;
				printf("��������:\n");
				for(n = 0; n < 5; n++ )
				{
					if(Android_CHECK_BUF_VALID(UART3_android_st.RXBuff[n])==TRUE)
						{

							for(i = 0; i < 30; i++)
								printf("%4x",UART3_android_st.RXBuff[n][i]);

							printf("\n");
							memset(UART3_android_st.RXBuff[n], FALSE,10);
						}
					}
			}
			
			delay_ms(100);
			 
		}
}


//
//
//
//
//int main(void)
//{  
//	TRAN_D_struct   TRAN_info1;
//
//	uint8_t   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
//	//uint8_t   dat2[100] = {25,26,27,28,29,30,31,32,33,34,35,36,37};
//
//	//u8  M_tagfounds = 1;
//
//	Init();
//
//	memset( (u8*)&IR_CAP1, 0,sizeof(IR_CAP_struct) );
//
//	LCD_1_44_Clear(BLACK); //����
//	
//	Show_Str(5,50,YELLOW,BLACK,"���ǲ���!!!!",24,1);
//	
//			 
//	while(1)
//	{  
//		if(join_key == 0)
//		{
//			delay_ms(10);
//			while(join_key == 0) {};
//
//			TRAN_info1.dest_dev_num = 0;
//
//			TRAN_info1.dest_addr[0] = 0x00;
//			TRAN_info1.dest_addr[1] = 0x00;
//			TRAN_info1.dest_addr[2] = 0x00;
//			TRAN_info1.dest_addr[3] = 0x00;
//
//			TRAN_info1.source_dev_num = 0;
//
//			TRAN_info1.source_addr[0] = 0;
//			TRAN_info1.source_addr[1] = 0;
//			TRAN_info1.source_addr[2] = 0;
//			TRAN_info1.source_addr[3] = 0;
//
//			TRAN_info1.TYPE_NUM = 4;   //��4��
//			TRAN_info1.data_len = 3; 
//
//			dat[0] = 0;
//			dat[1] = 1;
//			dat[2] = 60;
//			join_LED_S = 60 ;
//
//			ZIG1_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len ); 
//		}
//		
//		if( PC_KEY_FLAG )//������,״̬Ҫ�л�һ��
//		{
//			if(JDQ_NOW_STATUS != JDQ_WILL_STATUS)
//			{
//				JDQ_PIN = 1;
//
//				if( PC_STATUS == 0 )
//				{
//					JDQ_NOW_STATUS = 2;
//				}
//				else
//				{
//					JDQ_NOW_STATUS = 0;
//				}
//			}
//			else
//			{ 
//				JDQ_PIN = 0; 
//				PC_KEY_FLAG = 0; 
//
//				TRAN_info1.dest_dev_num = 0;
//
//				TRAN_info1.dest_addr[0] = 0x00;
//				TRAN_info1.dest_addr[1] = 0x00;
//				TRAN_info1.dest_addr[2] = 0x00;
//				TRAN_info1.dest_addr[3] = 0x00;
//
//				TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );
//
//				TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
//				TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
//				TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
//				TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);
//
//
//				TRAN_info1.TYPE_NUM = 4;   //��2��
//				TRAN_info1.data_len = 3;
//
//				dat[0] = 00;  
//				dat[1] = 0x0c;   // 
//				dat[2] = JDQ_NOW_STATUS;  
//							 
//				RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
//				delay_ms(50);
//				
//				TRAN_info1.data_len = 18;
//
//				
//				gb_dat[1] = 0x0b; 
//				gb_dat[2] = 0;
//				gb_dat[2+5]=JDQ_NOW_STATUS;
//				Android_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, gb_dat ,TRAN_info1.data_len );
//				//delay_ms(50);
//				//RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
//				//delay_ms(50);
//				//RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
//			}		  
//		}
//		
//		switch( m_mode )
//		{
//			case 0: 
//			{
//				// 	disable_all_uart_interupt(   ); 
//				//  CR95HF_FUC(  M_tagfounds );    //ˢ����  drv95HF_SPIPollingCommand 
//				//  (M_tagfounds>=TRACK_NFCTYPE5)?( M_tagfounds=TRACK_NFCTYPE1 ):( M_tagfounds*=2  );
//				// enable_all_uart_interupt(   );
//
//
//				ZIG1_uart_fuc() ;
//				ZIG1_dma_send_buf();
//				Android_uart_fuc() ;
//				Android_dma_send_buf(); 
//
//				RS485_uart_fuc();
//				RS485_dma_send_buf();
//
//			} break;
//			
//			case 1: //����1ģʽ ����ѧϰ����ģʽ��ʱ����60S
//			{
//				if(IR_CAP1.CAP_IR_WAVEFORM_TIME > 0)
//				{
//					if(IR_CAP_START_FLG == 1)
//					{
//						remote_cap_study();
//					} 
//				}
//				else 
//				{  
//					IR_CAP1.CAP_IR_WAVEFORM_TIME = 0;
//					IR_CAP_START_FLG = 0;
//					m_mode = 0;
//				}
//			} break;
//			
//			case 2:  //���� �������ң�ز��� 
//			{
//				IR_send_waveform( IR_CAP1.CAP_IR_store_num );
//				LED_HW = 1;
//				m_mode = 0;
//			} break;
//				
//			default: break; 
//		} 
//	}
//}

void  Init(void)
{
	
	delay_init();	        	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	HAL_GetUID(MCU_UID);     //��õ�Ƭ��Ψһ��

	TIM2_Int_Init(9,72);
	   
	LCD144_Init();
	IO_Init();
	
	uart3_init(115200);
	//usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	uart1_init(115200);
	
}

u8  main_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i = 0;

	for(i=0;i<3;i++)
	{
		if(  p[i] != 0  )
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





