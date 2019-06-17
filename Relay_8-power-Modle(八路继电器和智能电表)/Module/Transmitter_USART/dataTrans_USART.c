#include <dataTrans_USART.h>//���ݴ����������̺�����������wifi��zigbee��

extern ARM_DRIVER_USART Driver_USART4;
extern ARM_DRIVER_USART Driver_USART5;

extern osThreadId tid_keyMboard_Thread;	//�������尴������ID�����ڴ�����Ϣ����ʹ���ź�

osThreadId tid_USARTWireless_Thread;
osThreadId tid_UsartRx_Thread;
uint8 gb_databuff[50];


static bool up_status = false;

static bool online = false;
static u16 sendcount = 0;
volatile bool gb_Exmod_key=false;
static bool RX_FLG = false; //��Ч���ݻ�ȡ��־
static uint8 recvbuff[100]={0};

osThreadDef(USARTWireless_Thread,osPriorityNormal,1,1024);
osThreadDef(UsartRx_Thread,osPriorityNormal,1,512);


/*********************************************************************
 *  crc
 */
uint8_t TRAN_crc8(uint8_t *buf,uint8_t len)
{
	uint8_t crc;
	uint8_t i;

	crc = 0;

	while(len--) 
	{
		crc ^= *buf++;

		for(i = 0; i < 8; i++) 
		{
			if(crc & 0x01)
				crc = (crc >> 1) ^ 0x8C;
			else
				crc >>= 1;
		}
	}
	return crc;
}

void USART4Wirless_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/*Initialize the USART driver */
	Driver_USART4.Initialize(myUSART4_callback);
	/*Power up the USART peripheral */
	Driver_USART4.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART4.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART4.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART4.Control (ARM_USART_CONTROL_RX, 1);

//	Driver_USART4.Send("i'm usart2 for wireless datstransfor\r\n", 38);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);		//ʹ�ܻ���ʧ��APB2����ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;					//�װ�ָʾ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//����������50MHz
	//��������ᵼ���޷���λ�������ڶ�����zigbee�����Բ��ÿ�©���
	//��������ᵼ���޷���λ�������ڶ�����zigbee�����Բ��ÿ�©���
	//��������ᵼ���޷���λ�������ڶ�����zigbee�����Բ��ÿ�©���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	MSGZigbee_rest = 1;


	
}

void USART5Wirless_Init(void){
	
	 
	/*Initialize the USART driver */
	Driver_USART5.Initialize(myUSART4_callback);
	/*Power up the USART peripheral */
	Driver_USART5.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART5.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART5.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART5.Control (ARM_USART_CONTROL_RX, 1);

//	Driver_USART4.Send("i'm usart2 for wireless datstransfor\r\n", 38);
	
	
}

void myUSART4_callback(uint32_t event){

	uint32_t TX_mask;
	uint32_t RX_mask;
  	TX_mask =ARM_USART_EVENT_TRANSFER_COMPLETE |
	         ARM_USART_EVENT_SEND_COMPLETE     |
	         ARM_USART_EVENT_TX_COMPLETE; 
         	
     RX_mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
			   ARM_USART_EVENT_TRANSFER_COMPLETE |
			   ARM_USART_EVENT_RX_TIMEOUT;
  if (event & TX_mask) {
    /* Success: Wakeup Thread |
			   ARM_USART_EVENT_RX_TIMEOUT*/
    osSignalSet(tid_UsartRx_Thread, 0x01);//������ɣ��������signal
    osSignalSet(tid_USARTWireless_Thread, 0x01);
    
  }
  if (event & RX_mask) {
    /* Success: Wakeup Thread */
    osSignalSet(tid_UsartRx_Thread, 0x02);//������ɣ��������signal
    osSignalSet(tid_USARTWireless_Thread, 0x02);//������ɣ��������signal
  	}
}


void UsartRx_Thread(const void *argument	)
{
	uint8 recv_b[100]={0};
	TRAN_D_struct recv_tranbuff;
	u8 datacount = 0;
	u8 datacount_last = 0;
	
	delay_ms(200);
	MSGZigbee_rest = 0;
	delay_ms(10);
	MSGZigbee_rest = 1;
	
	
	
	while(1)
	{
		
		

		memset(recv_b,0,100);
		//osSignalWait(0x02, osWaitForever);
		
		Driver_USART4.Receive(recv_b,64);
		osSignalWait(0x02, osWaitForever);
		memcpy(recvbuff,recv_b,100);//���뻺��

		if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa  )
		{
			if( recvbuff[18] == 3)//��ѯCMD
			{
				if(recvbuff[19] == 0)
				{
					online = false;
					LED4 = 0;
					delay_ms(50);
					LED4 = 1;
					delay_ms(50);
					LED4 = 0;
					delay_ms(50);
					LED4 = 0;
					delay_ms(50);
					LED4 = !Relay4;
					
				}
				else if(recvbuff[19] == 1)
				{
					online = true;
					up_status = true;
					osSignalSet(tid_USARTWireless_Thread, 0x03);
					LED5 = 0;
					delay_ms(50);
					LED5 = 1;
					delay_ms(50);
					LED5 = 0;
					delay_ms(50);
					LED5 = 0;
					delay_ms(50);
					LED5 = !Relay5;
				}
			}
			else if( recvbuff[18] == 5)
			{
				RX_FLG = true;
				
			}
			else if( recvbuff[18] == 6)
			{
				gb_Exmod_key = true;
				delay_ms(5);
				osSignalSet(tid_USARTWireless_Thread, 0x03);
				
			}

			
		}
		
		if(online)
				led2_status = led2_b;
			else
				led2_status = led2_g;
		
			
	/*************�������������������������������������������������������������������������ݽ��գ��������������������̡�������������������������������������������������������������������������*********************/
			if(RX_FLG){
				RX_FLG = false;
	
	
				//���ҶԱ�
				if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa)
				{
					uint8  crc=0xfe;
					memcpy(&recv_tranbuff,recvbuff+3,17);
					crc = TRAN_crc8(recvbuff,19+recv_tranbuff.data_len);//17+recv_tranbuff.data_len+2
	
					if(crc == recvbuff[19+recv_tranbuff.data_len])//17+recv_tranbuff.data_len+2
					{
						datacount = recvbuff[2];//��Ű����
						if(datacount != datacount_last)//����ͬ��
						{
							datacount_last = datacount;
							
						}
						

						(Relay1 = recvbuff[16+recv_tranbuff.data_len]>>7);
						(Relay2 = recvbuff[16+recv_tranbuff.data_len]>>6);
						(Relay3 = recvbuff[16+recv_tranbuff.data_len]>>5);
						(Relay4 = recvbuff[16+recv_tranbuff.data_len]>>4);
						(Relay5 = recvbuff[16+recv_tranbuff.data_len]>>3);
						(Relay6 = recvbuff[16+recv_tranbuff.data_len]>>2);
						(Relay7 = recvbuff[16+recv_tranbuff.data_len]>>1);
						(Relay8 = recvbuff[16+recv_tranbuff.data_len]>>0);

						LED1 = ~Relay1;
						LED2 = ~Relay2;
						LED3 = ~Relay3;
						LED4 = ~Relay4;
						LED5 = ~Relay5;
						LED6 = ~Relay6;
						LED7 = ~Relay7;
						LED8 = ~Relay8;

						
	
					}
				}
				
				
		}
	
	}
	
}
////ÿ�������ɹ���Ҫ��һ������״̬֪ͨ��CMD = 0x03
void zigbee_status_updata(u8 EXmodID)
{
	
	uint8 sendbuff[100]={0};
	
	TRAN_D_struct updataTRAN;

	memset(sendbuff,0,100);
	memset(&updataTRAN,0,sizeof(TRAN_D_struct));

	memset(sendbuff,0xAA,2);
	updataTRAN.TYPE_NUM = 0x03;//��ģ��stm32--->·����zigbee
	sendbuff[2] = sendcount++;
	sendbuff[18] = 0x03;//CMD----->updata
	sendbuff[20] = EXmodID;


	updataTRAN.data_len = 13+16;					
	memcpy(sendbuff+4+updataTRAN.data_len,gb_databuff, 13);//17+updataTRAN.data_len-13
	


	memcpy(sendbuff+3,&updataTRAN,sizeof(TRAN_D_struct));
	
	memset(sendbuff+17+updataTRAN.data_len,0xBB,2);

	sendbuff[19+updataTRAN.data_len] = TRAN_crc8(sendbuff,19+updataTRAN.data_len);//17+data_len+2
	Driver_USART5.Send(sendbuff,20+updataTRAN.data_len);//sendbuff,17+2+2+1
	osSignalWait(0x01, 100);
		
	
}

//�л�zigbee״̬������ <----> ����
LEDstatus zigbee_sw(void)
{
	uint8 sendbuff[100]={0};
	
	
	if(led2_status == led2_b_flash)
		{
			uint8 i = 0;
			
			for(;i < 200;i++ )
			{

				LED1 = 1;
				LED2 = 1;
				LED3 = 1;
				if (led2_status == led2_b_flash)
				{
					TRAN_D_struct joinTRAN;

					memset(sendbuff,0,100);
					

					memset(&joinTRAN,0,sizeof(joinTRAN));

					memset(sendbuff,0xAA,2);

					memset(sendbuff+17+2,0xBB,2);

					joinTRAN.TYPE_NUM = 0x03;//��ģ��stm32--->·����zigbee

					joinTRAN.data_len = 2;

					sendbuff[2] = sendcount++;

					memcpy(sendbuff+3,&joinTRAN,sizeof(joinTRAN));
					
					if( online )//��ʼ����ѯ���Ƿ�����
					{
						sendbuff[17] = 0;
						sendbuff[18] = 2;
						
					}
					else 
					{
						sendbuff[17] = 0;
						sendbuff[18] = 1;
					}
					if( i == 40)
					{
						//sendbuff[18] = 3;
					}
					
					sendbuff[17+2+2] = TRAN_crc8(sendbuff,17+2+2);

					if( i == 3 || i == 80)//ֻ������
					{
						Driver_USART5.Send(sendbuff,17+2+2+1);
						osSignalWait(0x01, 100);

							
					}

					
					if( i == 40)
					{
						Driver_USART5.Send(sendbuff,17+2+2+1);
						osSignalWait(0x01, 100);

							
					}
				}
				else if(led2_status == led2_b)
					return led2_status;
				else
					return led2_g;
				
				
				delay_ms(100);
				LED1 = 0;
				LED2 = 0;
				LED3 = 0;
				delay_ms(100);
				}
		}
	else
	{
		return led2_status;
	}

	
	return led2_g;
}
uint8 zigbee_updata(uint8 EXmodID)
{
	uint8 sendbuff[100]={0};
	
	TRAN_D_struct updataTRAN;

	memset(sendbuff,0,100);
	memset(&updataTRAN,0,sizeof(TRAN_D_struct));

	memset(sendbuff,0xAA,2);
	updataTRAN.TYPE_NUM = 0x03;//��ģ��stm32--->·����zigbee
	sendbuff[2] = sendcount++;
	sendbuff[18] = 0x04;//CMD----->updata
	sendbuff[20] = EXmodID;


	updataTRAN.data_len = 13+16;					
	memcpy(sendbuff+4+updataTRAN.data_len,gb_databuff, 13);//17+updataTRAN.data_len-13
	


	memcpy(sendbuff+3,&updataTRAN,sizeof(TRAN_D_struct));
	
	memset(sendbuff+17+updataTRAN.data_len,0xBB,2);

	sendbuff[19+updataTRAN.data_len] = TRAN_crc8(sendbuff,19+updataTRAN.data_len);//17+data_len+2
	Driver_USART5.Send(sendbuff,20+updataTRAN.data_len);//sendbuff,17+2+2+1
	osSignalWait(0x01, 100);


	
	
	
	return 0;
}

void USARTWireless_Thread(const void *argument){
	
	
	

	for(;;){

			osSignalWait(0x03, 100);
			
			led2_status = zigbee_sw();
			if(LED1 == Relay1 || LED2 == Relay2|| LED3 == Relay3)
			{
				LED1 = !Relay1;
				LED2 = !Relay2;
				LED3 = !Relay3;
			}
			
			if( online && gb_Exmod_key)//�����߳�
			{	

				gb_Exmod_key = false;
				zigbee_updata(Moudle_GTA.Extension_ID);
			}

			if(up_status)
			{	
				up_status = false;
				delay_ms(50);
				zigbee_status_updata(Moudle_GTA.Extension_ID);
			}

	}
}
void USART_WirelessInit(void){


	USART4Wirless_Init();
	USART5Wirless_Init();

}

void wirelessThread_Active(uint8 status){
	
	if(status && led2_status == led2_r)
	{	
		led2_status = led2_g;
		USART_WirelessInit();
		tid_USARTWireless_Thread = osThreadCreate(osThread(USARTWireless_Thread),NULL);
		tid_UsartRx_Thread = osThreadCreate(osThread(UsartRx_Thread),NULL);
	}
	else if(status == 0)
	{
		led2_status = led2_r;
		osThreadTerminate(tid_USARTWireless_Thread);
		osThreadTerminate(tid_UsartRx_Thread);
	}
	
}
