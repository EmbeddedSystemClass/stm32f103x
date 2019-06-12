#include "RelayslightCM.h"//��·�̵����������̺�����

extern ARM_DRIVER_USART Driver_USART1;		//�豸�����⴮��һ�豸����

osThreadId tid_RelayslightCM_Thread;
osThreadDef(RelayslightCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  RelayslightCM_pool;								 
osPoolDef(RelayslightCM_pool, 10, RelayslightCM_MEAS);                  // �ڴ�ض���
osMessageQId  MsgBox_RelayslightCM;
osMessageQDef(MsgBox_RelayslightCM, 2, &RelayslightCM_MEAS);            // ��Ϣ���ж��壬����ģ���߳�������ͨѶ�߳�
osMessageQId  MsgBox_MTRelayslightCM;
osMessageQDef(MsgBox_MTRelayslightCM, 2, &RelayslightCM_MEAS);          // ��Ϣ���ж���,��������ͨѶ�߳���ģ���߳�
osMessageQId  MsgBox_DPRelayslightCM;
osMessageQDef(MsgBox_DPRelayslightCM, 2, &RelayslightCM_MEAS);          // ��Ϣ���ж��壬����ģ���߳�����ʾģ���߳�

void RelayslightCM_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	light1 = 0;
	light2 = 0;
}

void RelayslightCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 light1_vlue = 0;
	u8 light2_vlue = 0;
	u8 UPLD_cnt;
	const u8 UPLD_period = 5;

	RelayslightCM_MEAS actuatorData;	//����������
	static RelayslightCM_MEAS  Data_temp   = {0};	//��������������ͬ���ԱȻ���
	static RelayslightCM_MEAS  Data_tempDP = {0};	//������������ʾ���ݶԱȻ���
	uint8_t Kcnt;
	RelayslightCM_MEAS *mptr = NULL;
	RelayslightCM_MEAS *rptr = NULL;
	
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelayslightCM, 100);
		if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾���߳̽������ݴ��������������������������*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelayslightCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}
		
		if(Data_temp.relay_con != actuatorData.relay_con){
		
			Data_temp.relay_con = actuatorData.relay_con;
			
			light1 = (Data_temp.relay_con >> 0) & 0x01;
			light2 = (Data_temp.relay_con >> 1) & 0x01;
		}
	
		if(Data_tempDP.relay_con != actuatorData.relay_con){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			
			do{mptr = (RelayslightCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_DPRelayslightCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		if(UPLD_cnt < UPLD_period)
			UPLD_cnt ++;	//���ݶ�ʱ�ϴ�
		else{
		
			UPLD_cnt = 0;
			
			do{mptr = (RelayslightCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);}while(mptr == NULL);
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_RelayslightCM, (uint32_t)mptr, 100);
			osDelay(10);
		}
		if(key1_vlue == 0)
		{
			Kcnt = 100;
			while(key1_vlue == 0 && Kcnt--)
				{
					osDelay(25);	
				}
			light1_vlue =!light1_vlue;
				
				
		}
		if(key2_vlue == 0)
		{
			Kcnt = 100;
			while(key2_vlue == 0 && Kcnt--)
				{
					osDelay(25);	
				}
			light2_vlue =!light2_vlue;
				
		}
		
		actuatorData.relay_con = light1_vlue | (light2_vlue<<1) ;
		
	}
}

void RelayslightCMThread_Active(void){

	RelayslightCM_Init();
	RelayslightCM_pool   = osPoolCreate(osPool(RelayslightCM_pool));	//�����ڴ��
	MsgBox_RelayslightCM 	= osMessageCreate(osMessageQ(MsgBox_RelayslightCM), NULL);	//������Ϣ����
	MsgBox_MTRelayslightCM = osMessageCreate(osMessageQ(MsgBox_MTRelayslightCM), NULL);//������Ϣ����
	MsgBox_DPRelayslightCM = osMessageCreate(osMessageQ(MsgBox_DPRelayslightCM), NULL);//������Ϣ����
	
	tid_RelayslightCM_Thread = osThreadCreate(osThread(RelayslightCM_Thread),NULL);
}
