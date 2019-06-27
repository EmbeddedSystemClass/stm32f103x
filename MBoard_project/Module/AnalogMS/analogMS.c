/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     analogMS
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:01:22
 * @Description: 
 *��������ģ���źŲɼ�ģ��
 *	����·���ֵ�������·���ֵ�ѹ���ɼ��߳�
 *---------------------------------------------------------------------------*/


#include "analogMS.h"//ģ���źż����ֵ�����������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

osThreadId tid_analogMS_Thread;				//�߳�ID
osThreadDef(analogMS_Thread,osPriorityNormal,1,512);	//Create a Thread Definition with function, priority, and stack requirements.
			 
osPoolId  analogMS_pool;								 
osPoolDef(analogMS_pool, 10, analogMS_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_analogMS;
osMessageQDef(MsgBox_analogMS, 2, &analogMS_MEAS);            // ��Ϣ���ж��壬����ģ�����������ͨѶ����
osMessageQId  MsgBox_MTanalogMS;
osMessageQDef(MsgBox_MTanalogMS, 2, &analogMS_MEAS);          // ��Ϣ���ж���,��������ͨѶ������ģ�����
osMessageQId  MsgBox_DPanalogMS;
osMessageQDef(MsgBox_DPanalogMS, 2, &analogMS_MEAS);          // ��Ϣ���ж��壬����ģ���������ʾģ�����
/**************************************
* @description: ��ʼ��GPIO->GPIOA (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5) 
*		ADC1 ��Ӧ0��1��4��5channel(Don't miss an N)
* @param:  void
* @return: void
************************************/
void analogMS_ADCInit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M                     

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
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


}
/**************************************
* @description: 
* @param �����òɼ�ͨ����ADCx->channel0~16��ѡ���Ӧͨ���ɼ������� ch=0��ȡGPIOA->GPIO_Pin_0
* @return: ���ض�Ӧͨ���ɼ���ֵ��  ����ʮ��λ���ȣ���MAX_Value=4096, 16bit->MAX_Value=65535,
*		12-bit configurable resolution -> 16-bit data 
************************************/
u16 analogGet_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
/**************************************
* @description: 
* @param���ɼ�ͨ��->ch���ɼ�����->times
* @return: ���ؾ�ֵ
************************************/
u16 analogGet_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val += analogGet_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
/**************************************
* @description:ģ���ʼ������ 
* @param 
* @return: 
************************************/
void analogMS_Init(void){

	analogMS_ADCInit();
}
/**************************************
* @description:ģ���̺߳�������ʱ�ɼ�����ʱ�����ϴ��ź�
* @param������ʱ�趨ֵ->argument
* @return: 
************************************/
void analogMS_Thread(const void *argument){

	osEvent  evt;
   	osStatus status;

	analogMS_MEAS sensorData;

	static analogMS_MEAS Data_tempDP = {1};
	
	analogMS_MEAS *mptr = NULL;
	analogMS_MEAS *rptr = NULL;
	
	for(;;){
		/*�Զ��屾�ؽ��̽������ݴ��������������������������*/
		evt = osMessageGet(MsgBox_MTanalogMS, 100);
		if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			
			//��
			//---------------��--------------------

			do{status = osPoolFree(analogMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}
		//----------------���ݲɼ�-------------------------
		sensorData.Ich1 = analogGet_Adc_Average(ADC_Channel_0,10);
		sensorData.Vch1 = analogGet_Adc_Average(ADC_Channel_1,10);
		sensorData.Ich2 = analogGet_Adc_Average(ADC_Channel_4,10);
		sensorData.Vch2 = analogGet_Adc_Average(ADC_Channel_5,10);

		//---------------�иı伴�ϴ�-----------------------
		if(Data_tempDP.Ich1 != sensorData.Ich1 ||
		   Data_tempDP.Vch1 != sensorData.Vch1||
		   Data_tempDP.Ich2 != sensorData.Ich2 ||
		   Data_tempDP.Vch2 != sensorData.Vch2){
		
			Data_tempDP.Ich1 = sensorData.Ich1;
			Data_tempDP.Vch1 = sensorData.Vch1;
			Data_tempDP.Ich2 = sensorData.Ich2;
			Data_tempDP.Vch2 = sensorData.Vch2;
			//------------�����ϴ��ź�------------------
			gb_Exmod_key = true;
			memcpy(gb_databuff,&Data_tempDP.Vch1, 4);
			memcpy(gb_databuff+4,&Data_tempDP.Ich1, 4);	
			memcpy(gb_databuff+8,&Data_tempDP.Vch2, 4);
			memcpy(gb_databuff+12,&Data_tempDP.Ich2, 4);
			//------------��ʾ�ı���Ϣ����----------------	
			do{mptr = (analogMS_MEAS *)osPoolCAlloc(analogMS_pool);}while(mptr == NULL);
			mptr->Ich1 = sensorData.Ich1;
			mptr->Vch1 = sensorData.Vch1;
			mptr->Ich2 = sensorData.Ich2;
			mptr->Vch2 = sensorData.Vch2;
			osMessagePut(MsgBox_DPanalogMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
		
		
		osDelay(100);
	}
}
/**************************************
* @description: �̴߳����ӿں���
* @param��
* @return: 
************************************/
void analogMSThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		analogMS_pool   = osPoolCreate(osPool(analogMS_pool));	//�����ڴ��
		MsgBox_analogMS 	= osMessageCreate(osMessageQ(MsgBox_analogMS), NULL);   //������Ϣ����
		MsgBox_MTanalogMS = osMessageCreate(osMessageQ(MsgBox_MTanalogMS), NULL);//������Ϣ����
		MsgBox_DPanalogMS = osMessageCreate(osMessageQ(MsgBox_DPanalogMS), NULL);//������Ϣ����
		
		memInit_flg = true;
	}

	analogMS_Init();
	tid_analogMS_Thread = osThreadCreate(osThread(analogMS_Thread),NULL);
}
