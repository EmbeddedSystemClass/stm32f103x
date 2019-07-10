/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     pyroMS
 * @Author:     RanHongLiang
 * @Date:       2019-07-04 16:00:50
 * @Description: 
 * ������������������͵���ģ��
 * 
 *---------------------------------------------------------------------------*/

#include "pyroMS.h"//����������͵����������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

osThreadId tid_pyroMS_Thread;
osThreadDef(pyroMS_Thread,osPriorityNormal,1,512);
			 
osPoolId  pyroMS_pool;								 
osPoolDef(pyroMS_pool, 10, pyroMS_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_pyroMS;
osMessageQDef(MsgBox_pyroMS, 2, &pyroMS_MEAS);            // ��Ϣ���ж��壬����ģ���߳�������ͨѶ�߳�
osMessageQId  MsgBox_MTpyroMS;
osMessageQDef(MsgBox_MTpyroMS, 2, &pyroMS_MEAS);          // ��Ϣ���ж���,��������ͨѶ�߳���ģ���߳�
osMessageQId  MsgBox_DPpyroMS;
osMessageQDef(MsgBox_DPpyroMS, 2, &pyroMS_MEAS);          // ��Ϣ���ж��壬����ģ���߳�����ʾģ���߳�
/*---------------------------------------------------------------------------
 * @Description:��ƽ���IO
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
pyroDIO_Init(void){

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
}
/*---------------------------------------------------------------------------
 * @Description:ģ���ʼ��
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void pyroMS_Init(void){

	pyroDIO_Init();
}
/*---------------------------------------------------------------------------
 * @Description:���̣߳��������ݶ�ʱ�ϴ��������ϴ�
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void pyroMS_Thread(const void *argument){
	
	osEvent  evt;
    osStatus status;	
	
	const bool UPLOAD_MODE = false;	//1�����ݱ仯ʱ���ϴ� 0�����ڶ�ʱ�ϴ�
	
	const uint8_t upldPeriod = 5;	//�����ϴ�����������UPLOAD_MODE = false ʱ��Ч��
	
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;
	
	const uint8_t dpSize = 30;
	const uint8_t dpPeriod = 40;
	
	static uint8_t Pcnt = 0;
	char disp[dpSize];
	
	pyroMS_MEAS	sensorData;
	static pyroMS_MEAS Data_temp = {1};
	static pyroMS_MEAS Data_tempDP = {1};
	
	pyroMS_MEAS *mptr = NULL;
	fireMS_MEAS *rptr = NULL;
	
	for(;;){
		
		evt = osMessageGet(MsgBox_MTpyroMS, 100);
		if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾���߳̽������ݴ��������������������������*/
			

			do{status = osPoolFree(pyroMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}
		
		sensorData.VAL = PYRO_DATA;	//���ݲɼ�
		
		if(!UPLOAD_MODE){	//ѡ���ϴ�����ģʽ
		
			if(UPLDcnt < upldPeriod)UPLDcnt ++;
			else{
			
				UPLDcnt = 0;
				UPLD_EN = true;
			}
		}else{
			
			if(Data_temp.VAL != sensorData.VAL){	//�������ͣ����ݸ���ʱ�Ŵ�����
			
				Data_temp.VAL = sensorData.VAL;
				UPLD_EN = true;
			}
		}
		
		if(UPLD_EN){
			
			UPLD_EN = false;
			
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;
			osDelay(10);
		}
		
		if(Data_tempDP.VAL != sensorData.VAL){	//�������ͣ����ݸ���ʱ�Ŵ�����
		
			Data_tempDP.VAL = sensorData.VAL;
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;
			do{mptr = (pyroMS_MEAS *)osPoolCAlloc(pyroMS_pool);}while(mptr == NULL);
			mptr->VAL = sensorData.VAL;
			osMessagePut(MsgBox_DPpyroMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
		
		if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;}
		else{
		
			Pcnt = 0;
			memset(disp,0,dpSize * sizeof(char));
			sprintf(disp,"\n\ris anybody here now? : %d\n\r", !sensorData.VAL);			
			Driver_USART1.Send(disp,strlen(disp));
			osDelay(20);
		}
		
		osDelay(10);
	}
}
/*---------------------------------------------------------------------------
 * @Description:ģ������API
 * @Param:      ��
 * @Return:     ��
 *---------------------------------------------------------------------------*/
void pyroMSThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		pyroMS_pool   = osPoolCreate(osPool(pyroMS_pool));	//�����ڴ��
		MsgBox_pyroMS 	= osMessageCreate(osMessageQ(MsgBox_pyroMS), NULL);   //������Ϣ����
		MsgBox_MTpyroMS = osMessageCreate(osMessageQ(MsgBox_MTpyroMS), NULL);//������Ϣ����
		MsgBox_DPpyroMS = osMessageCreate(osMessageQ(MsgBox_DPpyroMS), NULL);//������Ϣ����
		
		memInit_flg = true;
	}

	pyroMS_Init();
	tid_pyroMS_Thread = osThreadCreate(osThread(pyroMS_Thread),NULL);
}
