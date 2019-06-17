#include "shakeMS.h"//�𶯴�������������Ľ��̺���

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

osThreadId tid_shakeMS_Thread;
osThreadDef(shakeMS_Thread,osPriorityNormal,1,512);
			 
osPoolId  shakeMS_pool;								 
osPoolDef(shakeMS_pool, 10, shakeMS_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_shakeMS;
osMessageQDef(MsgBox_shakeMS, 2, &shakeMS_MEAS);            // ��Ϣ���ж��壬����ģ�����������ͨѶ����
osMessageQId  MsgBox_MTshakeMS;
osMessageQDef(MsgBox_MTshakeMS, 2, &shakeMS_MEAS);          // ��Ϣ���ж���,��������ͨѶ������ģ�����
osMessageQId  MsgBox_DPshakeMS;
osMessageQDef(MsgBox_DPshakeMS, 2, &shakeMS_MEAS);          // ��Ϣ���ж��壬����ģ���������ʾģ�����

void shakeDIO_Init(void){

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
}

void shakeMS_Init(void){

	shakeDIO_Init();
}

void shakeMS_Thread(const void *argument){

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
	
	shakeMS_MEAS	sensorData;
	static shakeMS_MEAS Data_temp = {1};
	static shakeMS_MEAS Data_tempDP = {1};
	
	shakeMS_MEAS *mptr = NULL;
	shakeMS_MEAS *rptr = NULL;
	
	for(;;){
		
	/***********************���ؽ������ݽ���***************************************************/
	//�������������ݽ����ϴ����������ݹ��ܱ�������ʱ����
		evt = osMessageGet(MsgBox_MTshakeMS, 100);
		if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾�ؽ��̽������ݴ��������������������������*/
			

			do{status = osPoolFree(shakeMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}

	/***********************�������ݲɼ�*****************************************************/
		sensorData.VAL = shake_DATA;		//���ݲɼ�
		
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

	/***********************������������*****************************************************/		
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
			do{mptr = (shakeMS_MEAS *)osPoolCAlloc(shakeMS_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
			mptr->VAL = sensorData.VAL;
			osMessagePut(MsgBox_DPshakeMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
	/***********************Debug_log*********************************************************/		
		if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;}
		else{
		
			Pcnt = 0;
			memset(disp,0,dpSize * sizeof(char));
			sprintf(disp,"\n\ris firing now? : %d\n\r", Data_temp.VAL);			
			Driver_USART1.Send(disp,strlen(disp));
			osDelay(20);
		}
		osDelay(10);
	}
}

void shakeMSThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		shakeMS_pool   = osPoolCreate(osPool(shakeMS_pool));	//�����ڴ��
		MsgBox_shakeMS 	= osMessageCreate(osMessageQ(MsgBox_shakeMS), NULL);   //������Ϣ����
		MsgBox_MTshakeMS = osMessageCreate(osMessageQ(MsgBox_MTshakeMS), NULL);//������Ϣ����
		MsgBox_DPshakeMS = osMessageCreate(osMessageQ(MsgBox_DPshakeMS), NULL);//������Ϣ����
		
		memInit_flg = true;
	}

	shakeMS_Init();
	tid_shakeMS_Thread = osThreadCreate(osThread(shakeMS_Thread),NULL);
}
