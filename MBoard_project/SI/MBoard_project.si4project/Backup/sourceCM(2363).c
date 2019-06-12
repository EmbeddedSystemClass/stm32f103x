#include "sourceCM.h"    //���ܵ���������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

extern osThreadId tid_USARTDebug_Thread;

 uint32_t	 gb_power_kwh;
 uint16_t	 gb_power_v;
 uint32_t	 gb_power_A;

//extern datsTransCMD_FLAG1;
	
//Ĭ��RS485 9600B

osThreadId tid_sourceCM_Thread;
osThreadDef(sourceCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  sourceCM_pool;								 
osPoolDef(sourceCM_pool, 10, sourceCM_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_sourceCM;
osMessageQDef(MsgBox_sourceCM, 2, &sourceCM_MEAS);            // ��Ϣ���ж��壬����ģ���߳�������ͨѶ�߳�
osMessageQId  MsgBox_MTsourceCM;
osMessageQDef(MsgBox_MTsourceCM, 2, &sourceCM_MEAS);          // ��Ϣ���ж���,��������ͨѶ�߳���ģ���߳�
osMessageQId  MsgBox_DPsourceCM;
osMessageQDef(MsgBox_DPsourceCM, 2, &sourceCM_MEAS);          // ��Ϣ���ж��壬����ģ���߳�����ʾģ���߳�

//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
	

void sourceCM_ioInit(void){//�˿ڳ�ʼ��

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	                

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(unsigned char  *buf,unsigned char  len)
{
	
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	osDelay(1);
  	Driver_USART1.Send(buf,len);	//���ݷ���
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET){};//�ȴ����ͽ���

	RS485_TX_EN=0;
	RS485_TX_EN=0;	
		
 
				//����Ϊ����ģʽ	
}

void SOURCE_USARTInitCallback(uint32_t event){

	;
}

void USART1source_Init(uint16 booter){

	/*Initialize the USART driver */
	Driver_USART1.Initialize(SOURCE_USARTInitCallback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_EVEN |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, booter);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	
}

void sourceCM_Init(void){
	
	//debug_Init();
	//uart_init(9600);
	//debug_Init();
	sourceCM_ioInit();
	//sourceCM_ADCInit();
}

unsigned char ADD_CHECK_8b(u8 dats [ ], u8 length)
{
	u8 loop;
	u8	result = 0;
	
	for(loop = 0;loop < length;loop ++){
	
		result += dats[loop];
	}
	return result;
}

unsigned char empty_id(unsigned char *id)
{
	char i = 0;
	for ( ; i  < 6; i++)
		{
			if(id[i] != 0xAA)
				return 0;
		}
	return 1;
}

unsigned char read_data2id(unsigned char *data,unsigned char *id)
{
	unsigned char i = 4;
	unsigned char add = 0;
	unsigned char len = data[13]+14;
	if(data[4] != 0x68 )
			return 0;
	
	for( i = 4; i  < len; i++)//data[4]->data[len-2]�ĺ�У��
		{
			
			add += data[i];
			
		}
	if(add == data[len])
	{
		for ( i = 0; i  < 6; i++)
		{
			id[i] = data[i+5];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;

    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}


void sourceCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;
	unsigned char temp_id[6] 	= {0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA };
	unsigned char read_id[12] 	= {0x68 ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0x68 ,0x13 ,0x00 ,0xDF ,0x16};
	//FE FE FE FE 68 94 41 03 26 12 18 68 93 06 C7 74 36 59 45 4B EB 16
	unsigned char strpower_kwh[4]	= {0x33,0x33,0x34,0x33};//����ָ���λkwh,���ַ��ÿλ������Ҫ��0x33
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 08 33 33 34 33 59 33 33 33 50 16 
	unsigned char strpower_v[4]	= {0x33,0x32,0x34,0x35};//��ѹָ���λV
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 0A 33 32 34 35 36 56 33 33 33 33 B9 16 
	unsigned char strpower_a[4] 	= {0x33,0x32,0x35,0x35};//����ָ���λA
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 0D 33 32 35 35 BB 34 33 33 33 33 33 33 33 B9 16 
	unsigned char send_buff[24]	= {0x68 ,
									0,0,0,0,0,0,
									0x68,
									0x11,0x04,
									0x00,0x00,0x00,0x00,
									0x00,
									0x16};
		
	const bool UPLOAD_MODE = false;	//1�����ݱ仯ʱ���ϴ� 0�����ڶ�ʱ�ϴ�
	
	const uint8_t upldPeriod = 5;	//�����ϴ�����������UPLOAD_MODE = false ʱ��Ч��
	
	static char flag=0;
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;
	const uint8_t dpSize = 40;
	const uint32_t dpPeriod = 2000;

	uint16 enble_count = 0;
	
	static uint32_t Pcnt = 0;
	char disp[dpSize];
	char len_b = 0;
	sourceCM_MEAS actuatorData ={0,0,0,0,0,0};	//����������
	//static sourceCM_MEAS Data_temp   = {1};	//��������������ͬ���ԱȻ���
	static sourceCM_MEAS Data_tempDP = {1};	//������������ʾ���ݶԱȻ���
	
	sourceCM_MEAS *mptr = NULL;
	sourceCM_MEAS *rptr = NULL;


	osThreadTerminate(tid_USARTDebug_Thread);   //�ر�debug���������軥��
	USART1source_Init(2400);
	
	for(;;){
			float p=actuatorData.power_p/100.0;
			float v=actuatorData.power_v/10.0;
			float i=actuatorData.power_i/1000.0;

			if(enble_count > 5)
			{
				enble_count = 0;
				Driver_USART1.Uninitialize();
				USART1source_Init(9600);
			}
				evt = osMessageGet(MsgBox_MTsourceCM, 100);
				if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
					
					rptr = evt.value.p;
					/*�Զ��屾���߳̽������ݴ��������������������������*/
					if (rptr->source_addr == datsTransCMD_FLAG1)
					{
						actuatorData.Switch = rptr->Switch;
						//actuatorData.anaVal	= rptr->anaVal;
						flag=1;
		
						//Data_temp.Switch = actuatorData.Switch; 	//����������ͬ���������������ݻط�
						//Data_temp.anaVal = actuatorData.anaVal;
					}
		
					do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK); //�ڴ��ͷ�
					rptr = NULL;
					
					//soceRELAY = actuatorData.Switch;//��actuatorData��ֵ����PA0
					
		/*ִ�����������һ��״̬*/do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//�������ݴ�����Ϣ����
					mptr->power_p = actuatorData.power_p;
					mptr->power_v = actuatorData.power_v;
					mptr->power_i = actuatorData.power_i;
					osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 50);
				}	
				
				//actuatorData.anaVal = soceGet_Adc_Max(4,100);//��0��ADC
				
				if(!UPLOAD_MODE){	//ѡ���ϴ�����ģʽ
				
					if(UPLDcnt < upldPeriod)UPLDcnt ++;
					else{
					
						UPLDcnt = 0;
						UPLD_EN = true;
					}
				}
				
				if(UPLD_EN){
					
					UPLD_EN = false;
					gb_Exmod_key = true;

					
					memcpy(gb_databuff, &(p), 4);
						memcpy(gb_databuff+4, &(v), 4);
						memcpy(gb_databuff+8, &(i), 4);
						
				}
				
				if( Data_tempDP.power_p != actuatorData.power_p || 
					Data_tempDP.power_v != actuatorData.power_v || 
					Data_tempDP.power_i != actuatorData.power_i)					
					{

 						Data_tempDP.power_p = actuatorData.power_p;
 						Data_tempDP.power_v = actuatorData.power_v;
 						Data_tempDP.power_i = actuatorData.power_i;

						gb_Exmod_key = true;
						memcpy(gb_databuff, &(p), 4);
						memcpy(gb_databuff+4, &(v), 4);
						memcpy(gb_databuff+12, &(i), 4);
						
						do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
						if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
						
						mptr->power_p = actuatorData.power_p;
 						mptr->power_v = actuatorData.power_v;
 						mptr->power_i = actuatorData.power_i;
						osMessagePut(MsgBox_DPsourceCM, (uint32_t)mptr, 100);
						osDelay(10);
						enble_count = 0;
				   }
			
				if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;enble_count++;}
				else{
				
					Pcnt = 0;
					memset(disp,0,dpSize * sizeof(char));
					sprintf(disp,"��-------------��\n\r����״̬ : %d\nģ������%d\n\n\r", actuatorData.Switch,actuatorData.anaVal);			
					//Driver_USART1.Send(disp,strlen(disp));
					osDelay(20);
				}
				
				osDelay(10);


	//δ��ȡID
		
		if( empty_id(temp_id) == 1  )
		{
			RS485_Send_Data(read_id, 12);
			
			Driver_USART1.Receive(RS485_RX_BUF,64);		
			osDelay(300);
			
			if(read_data2id(RS485_RX_BUF, temp_id) == 1 )
			{
				memcpy(temp_id, 	RS485_RX_BUF+5 , 6);
				memcpy(send_buff+1, RS485_RX_BUF+5 , 6);//��ID�����ͻ���
				
				}
			osDelay(500);
		}
		else //if(Pcnt == 200)//�Ի�ȡID
		{

			
			//��ȡ����
			memcpy(send_buff+10,  strpower_kwh , 4);			
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);			
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(300);
			//�����̣߳�
			if(Driver_USART1.GetRxCount() < 10)
				Exmod_rest = false;

			
			//Driver_USART5.Send(RS485_RX_BUF,64);
			len_b = RS485_RX_BUF[13]+16;
			osDelay(20);
			
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
			{
				actuatorData.power_p= 	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000;
				
				
			}
			
		
			
			
			//��ȡ��ѹ
			memcpy(send_buff+10,  strpower_v, 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(300);
			
			len_b = RS485_RX_BUF[13]+16;
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
				{
				actuatorData.power_v =	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000+
										bcd_to_hex(RS485_RX_BUF[22] - 0x33)*100000000;
				
			}
			//��ȡ����
			memcpy(send_buff+10,  strpower_a , 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);
			
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(500);
			
			len_b = RS485_RX_BUF[13]+16;
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
			{
				actuatorData.power_i =	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000+
										bcd_to_hex(RS485_RX_BUF[22] - 0x33)*100000000;
				
			}
		}
		//memset(RS485_RX_BUF, 0, 64);//���п��ޣ����ڻ�������ʼ�ն���
		delay_ms(200);


	}
}

void sourceCMThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		sourceCM_pool   = osPoolCreate(osPool(sourceCM_pool));	//�����ڴ��
		MsgBox_sourceCM = osMessageCreate(osMessageQ(MsgBox_sourceCM), NULL);   //������Ϣ����
		MsgBox_MTsourceCM = osMessageCreate(osMessageQ(MsgBox_MTsourceCM), NULL);//������Ϣ����
		MsgBox_DPsourceCM = osMessageCreate(osMessageQ(MsgBox_DPsourceCM), NULL);//������Ϣ����
		
		memInit_flg = true;
	}

	sourceCM_Init();
	tid_sourceCM_Thread = osThreadCreate(osThread(sourceCM_Thread),NULL);
}
