#include "sourceCM.h"    //��Դ�����������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����
//extern datsTransCMD_FLAG1;
	
//extern osThreadId tid_USARTDebug_Thread;


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

uint32_t	gb_power_kwh;
uint16_t	gb_power_v;
uint32_t	gb_power_A;
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT =0;   
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

void sourceCM_ioInit(void){//�˿ڳ�ʼ��

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	                

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//PAout(0)= 0;
}

void sourceCM_ADCInit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M                     

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   


	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

	ADC_StartCalibration(ADC1);	 //����ADУ׼

	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(unsigned char  *buf,unsigned char  len)
{
	
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	osDelay(10);
  	Driver_USART1.Send(buf,len);	//���ݷ���
  	//Driver_USART1.Send("i'm usart1 for debug log\r\n", 26);
	osDelay(100);	
	//RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

void SOURCE_USARTInitCallback(uint32_t event){

	;
}
//void SOURCE_USARTInitCallback1(uint32_t event){
//
//	;
//}
void USART1source_Init(void){

	/*Initialize the USART driver */
	Driver_USART1.Initialize(SOURCE_USARTInitCallback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_EVEN |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 9600);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	Driver_USART1.Send("i'm usart1 for debug log\r\n", 26);
}

void sourceCM_Init(void){
	osThreadTerminate(tid_USARTDebug_Thread);   //�ر�debug���������軥��
	USART1source_Init();

	sourceCM_ioInit();
	//sourceCM_ADCInit();
}

u16 soceGet_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 soceGet_Adc_Max(u8 ch,u8 times)
{
	static u16 temp_val = 0;
	u16 temp_max = 0;
	u8 t;
	
	for(t=0;t<times;t++)
	{
		temp_val = soceGet_Adc(ch);
		if(temp_val > temp_max)temp_max = temp_val;
		delay_ms(5);
	}
	return temp_max;
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

void sourceCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	
	const bool UPLOAD_MODE = false;	//1�����ݱ仯ʱ���ϴ� 0�����ڶ�ʱ�ϴ�
	
	const uint8_t upldPeriod = 5;	//�����ϴ�����������UPLOAD_MODE = false ʱ��Ч��
	
	static char flag=0;
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;
	
	const uint8_t dpSize = 40;
	const uint8_t dpPeriod = 10;
	
	static uint8_t Pcnt = 0;
	char disp[dpSize];
	char len_b = 0;

	sourceCM_MEAS actuatorData;	//����������
	static sourceCM_MEAS Data_temp   = {1};	//��������������ͬ���ԱȻ���
	static sourceCM_MEAS Data_tempDP = {1};	//������������ʾ���ݶԱȻ���
	
	sourceCM_MEAS *mptr = NULL;
	sourceCM_MEAS *rptr = NULL;

	gb_power_kwh = 999;
	gb_power_v	 = 999;
	gb_power_A	 = 999;
		
	for(;;){
		
		evt = osMessageGet(MsgBox_MTsourceCM, 100);
		if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾���߳̽������ݴ��������������������������*/
			if (rptr->source_addr == datsTransCMD_FLAG1)
			{
				actuatorData.Switch	= rptr->Switch;
				//actuatorData.anaVal	= rptr->anaVal;
				flag=1;

				Data_temp.Switch = actuatorData.Switch;		//����������ͬ���������������ݻط�
				//Data_temp.anaVal = actuatorData.anaVal;
			}

			do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
			
			//soceRELAY = actuatorData.Switch;//��actuatorData��ֵ����PA0
			
/*ִ�����������һ��״̬*/do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//�������ݴ�����Ϣ����
			mptr->source_addr = datsTransCMD_FLAG1;
			mptr->Switch = actuatorData.Switch;
			mptr->anaVal = actuatorData.anaVal;
			osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 50);
		}	
		
		//actuatorData.anaVal = soceGet_Adc_Max(4,100);//��0��ADC
		actuatorData.anaVal++;
		if(!UPLOAD_MODE){	//ѡ���ϴ�����ģʽ
		
			if(UPLDcnt < upldPeriod)UPLDcnt ++;
			else{
			
				UPLDcnt = 0;
				UPLD_EN = true;
			}
		}else{
		
		if(Data_temp.anaVal != actuatorData.anaVal){	
		
				Data_temp.anaVal = actuatorData.anaVal;
				UPLD_EN = true;
			}
		}
		
		if(UPLD_EN){
			
			UPLD_EN = false;
				
				do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//�������ݴ�����Ϣ����
				if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
				mptr->Switch = actuatorData.Switch;
				mptr->anaVal = actuatorData.anaVal;
				osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 100);
		}
		
		if(Data_tempDP.anaVal != actuatorData.anaVal || Data_tempDP.Switch != actuatorData.Switch){
		
				Data_tempDP.anaVal = actuatorData.anaVal;
				Data_tempDP.Switch = actuatorData.Switch;
		   
				do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
				if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
				mptr->anaVal = actuatorData.anaVal;
				mptr->Switch = actuatorData.Switch;
				osMessagePut(MsgBox_DPsourceCM, (uint32_t)mptr, 100);
				osDelay(10);
		   }
	
		if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;}
		else{
		
			Pcnt = 0;
			memset(disp,0,dpSize * sizeof(char));
			sprintf(disp,"��-------------��\n\r����״̬ : %d\nģ������%d\n\n\r", actuatorData.Switch,actuatorData.anaVal);			
			//Driver_USART1.Send(disp,strlen(disp));
			osDelay(20);
		}
		
		osDelay(10);
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
