#include "RelaysoutletCM.h"//��·�̵����������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

osThreadId tid_RelaysoutletCM_Thread;

osThreadDef(RelaysoutletCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  RelaysoutletCM_pool;								 
osPoolDef(RelaysoutletCM_pool, 10, RelaysoutletCM_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_RelaysoutletCM;
osMessageQDef(MsgBox_RelaysoutletCM, 2, &RelaysoutletCM_MEAS);            // ��Ϣ���ж��壬����ģ���߳�������ͨѶ�߳�
osMessageQId  MsgBox_MTRelaysoutletCM;
osMessageQDef(MsgBox_MTRelaysoutletCM, 2, &RelaysoutletCM_MEAS);          // ��Ϣ���ж���,��������ͨѶ�߳���ģ���߳�
osMessageQId  MsgBox_DPRelaysoutletCM;
osMessageQDef(MsgBox_DPRelaysoutletCM, 2, &RelaysoutletCM_MEAS);          // ��Ϣ���ж��壬����ģ���߳�����ʾģ���߳�
static	unsigned char light1_vlue ;

void RelaysoutletCM_AIOinit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M


	//PA0  ��Ϊģ��ͨ����������  ADC12_IN8                       

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
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
//���ADCֵ
//ch:ͨ��ֵ 0~3
uint16_t Get_Adc(uint8_t ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

uint32_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	u32 temp_val=0;
	uint32 buff =0;
	uint32 max_buff =0;
	uint8_t t;
	
	for(t=0;t<times;t++)
	{
		buff = ((Get_Adc(ch) *100)/ (41*3));
		
		if(buff > 2300)
			temp_val = buff -2300 ;
		else
			temp_val = 2300 - buff ;
	
		max_buff = max_buff > temp_val ? max_buff : temp_val;
		delay_ms(2);
	}
	return temp_val/times;
} 	

void RelaysoutletCM_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}

void RelaysoutletCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 Kcnt = 0;
	u8 ADcnt = 0;
	uint32 AD_temp = 0;
	//u8 UPLD_cnt;
	//const u8 UPLD_period = 5;

	RelaysoutletCM_MEAS actuatorData;	//����������
	
	static RelaysoutletCM_MEAS  Data_tempDP = {0};	//������������ʾ���ݶԱȻ���
    
	static RelaysoutletCM_MEAS *mptr = NULL;
	static RelaysoutletCM_MEAS *rptr = NULL;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	outlet1 = 0;
	outlet1_led = 1;
	light1_vlue = 0;
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelaysoutletCM, 100);
		if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾���߳̽������ݴ��������������������������*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelaysoutletCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}
		

		
	
		if(Data_tempDP.relay_con != actuatorData.relay_con
		  || Data_tempDP.Current != actuatorData.Current){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			Data_tempDP.Current 	= actuatorData.Current;
			
			outlet1 	= (Data_tempDP.relay_con >> 0) & 0x01;
			outlet1_led	= !(Data_tempDP.relay_con >> 0) & 0x01;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
			mptr->relay_con = actuatorData.relay_con;
			mptr->Current 	= actuatorData.Current;
			osMessagePut(MsgBox_DPRelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		
		if(key_vlue == 0)
		{
			Kcnt = 100;
			while(key1_vlue == 0 && Kcnt--)
				{
					osDelay(25);
					
				}
			light1_vlue =!light1_vlue;

				
		}
		
		
		actuatorData.relay_con = light1_vlue  ;
		if(ADcnt < 10)
		{
			AD_temp += Get_Adc_Average(0,10);//Vi (mv) =data* 3.3* 1000 /4096
			ADcnt++;
			}
		else
		{
			actuatorData.Current = AD_temp ;
			AD_temp = 0;
			ADcnt = 0;
			
		}
		
		
		
	}
}


void RelaysoutletCMThread_Active(void){

	RelaysCM_Init();
	RelaysoutletCM_AIOinit();
	RelaysoutletCM_pool   = osPoolCreate(osPool(RelaysoutletCM_pool));	//�����ڴ��
	MsgBox_RelaysoutletCM 	= osMessageCreate(osMessageQ(MsgBox_RelaysoutletCM), NULL);	//������Ϣ����
	MsgBox_MTRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_MTRelaysoutletCM), NULL);//������Ϣ����
	MsgBox_DPRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_DPRelaysoutletCM), NULL);//������Ϣ����
	tid_RelaysoutletCM_Thread = osThreadCreate(osThread(RelaysoutletCM_Thread),NULL);
}
