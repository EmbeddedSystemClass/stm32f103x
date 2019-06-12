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

void RelaysoutletCM_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}

void RelaysoutletCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 outlet1_vlue = 0;
	u8 outlet2_vlue = 0;
	u8 UPLD_cnt;
	const u8 UPLD_period = 5;

	RelaysoutletCM_MEAS actuatorData;	//����������
	static RelaysoutletCM_MEAS  Data_temp   = {0};	//��������������ͬ���ԱȻ���
	static RelaysoutletCM_MEAS  Data_tempDP = {0};	//������������ʾ���ݶԱȻ���
	
	RelaysoutletCM_MEAS *mptr = NULL;
	RelaysoutletCM_MEAS *rptr = NULL;
	
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelaysoutletCM, 100);
		if (evt.status == osEventMessage){		//�ȴ���Ϣָ��
			
			rptr = evt.value.p;
			/*�Զ��屾���߳̽������ݴ��������������������������*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelaysoutletCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
			rptr = NULL;
		}
		
		if(Data_temp.relay_con != actuatorData.relay_con){
		
			Data_temp.relay_con = actuatorData.relay_con;
			
			outlet1 = (Data_temp.relay_con >> 0) & 0x01;
			outlet2 = (Data_temp.relay_con >> 1) & 0x01;
		}
	
		if(Data_tempDP.relay_con != actuatorData.relay_con){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);	//1.44��Һ����ʾ��Ϣ����
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_DPRelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		if(UPLD_cnt < UPLD_period)
			UPLD_cnt ++;	//���ݶ�ʱ�ϴ�
		else{
		
			UPLD_cnt = 0;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_RelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}
		if(key1_vlue == 0)
		{
			osDelay(25);
			if(key1_vlue == 0)
				{
					outlet1_vlue =!outlet1_vlue;
					
					
				}
				
				
		}
		if(key2_vlue == 0)
		{
			osDelay(25);
			if(key2_vlue == 0)
				{
					outlet2_vlue =!outlet2_vlue;
					
				}
				
				
		}
		
		actuatorData.relay_con = outlet1_vlue | (outlet2_vlue<<1) ;
		
	}
}

void RelaysoutletCMThread_Active(void){

	RelaysoutletCM_Init();
	RelaysoutletCM_pool   = osPoolCreate(osPool(RelaysoutletCM_pool));	//�����ڴ��
	MsgBox_RelaysoutletCM 	= osMessageCreate(osMessageQ(MsgBox_RelaysoutletCM), NULL);	//������Ϣ����
	MsgBox_MTRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_MTRelaysoutletCM), NULL);//������Ϣ����
	MsgBox_DPRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_DPRelaysoutletCM), NULL);//������Ϣ����
	
	tid_RelaysoutletCM_Thread = osThreadCreate(osThread(RelaysoutletCM_Thread),NULL);
}
