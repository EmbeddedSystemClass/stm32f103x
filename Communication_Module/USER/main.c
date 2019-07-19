/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-07-15 16:26:43
 * @Description: 
 * ������������ͨѶЭ���������ڽ�����λ����Ϣ��ת������׿
 * 	�����ȴ�ָʾ�ƣ�����ָʾ��
 * 
 * ʵѵ̨���豸->Zigbee-R==>Zigbee-C->Э����->��׿����
 * 
 * ---------------------------------------------------------------------------*/


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

/*---------------------------------------------------------------------------
 * @Description:��ȡSTM32����ID
 * @Param:      UID��ID���ָ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void HAL_GetUID(uint32_t *UID);
/*---------------------------------------------------------------------------
 * @Description:��ʱ��2 ��ʼ����������Ϊ�ⲿ����
 * @Param:      arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
extern void TIM2_Int_Init(u16 arr,u16 psc);
/*---------------------------------------------------------------------------
 * @Description:�����ʼ��
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void  Init(void) ;

/*---------------------------------------------------------------------------
 * @Description:������ڣ����ܰ���2����Ϊ����������zigbee����ת��
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
int main(void)
{
    	TRAN_D_struct   TRAN_info1;
	 u8   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
	 u8 KEY_VLUE = 255;
	 
	Init();
	
	
	
	//printf("this is a test\n");
	while (1)
		{

					 

			if (KEY2 == 0)
			{
				
				while(KEY2 == 0)
					{
					delay_ms(20);
					}
				KEY_VLUE = !KEY_VLUE;
					
			}


			if(KEY_VLUE == 1)
			{
				
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

				zigbee_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len ); 
				
				
				
				KEY_VLUE = !KEY_VLUE;
			}
			
			
			zigbee_uart_fuc() ;
			zigbee_dma_send_buf(); 
			
			android_uart_fuc() ;
			android_dma_send_buf(); 
			delay_ms(10);
			 
		}
}


/*---------------------------------------------------------------------------
 * @Description:ģ���ʼ�������ڣ�ʱ�����ߣ��жϣ�IO
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void  Init(void)
{
	
	delay_init();	        	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	HAL_GetUID(MCU_UID);     //��õ�Ƭ��Ψһ��

	TIM2_Int_Init(9,72);
	   
	//LCD144_Init();
	IO_Init();

	uart1_init(115200);
	//uart2_init(115200);
	uart3_init(115200);
	
	
	
}
/*---------------------------------------------------------------------------
 * @Description:Ϊ�ռ��
 * @Param:      ����
 * @Return:     ��/��
 *---------------------------------------------------------------------------*/
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


/*---------------------------------------------------------------------------
 * @Description:STM32�����Դ�ID
 * @Param:      ID���ָ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 





