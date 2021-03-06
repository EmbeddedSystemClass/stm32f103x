/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     RelayslightCM
 * @Author:     RanHongLiang
 * @Date:       2019-07-10 15:25:27
 * @Description: 
 * ————两路灯光控制模块，包括按键控制，状态上传，数据
 * ---------------------------------------------------------------------------*/

#include "RelayslightCM.h"//两路继电器驱动进程函数；

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明
extern ARM_DRIVER_USART Driver_USART2;

osThreadId tid_RelayslightCM_Thread;
osThreadDef(RelayslightCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  RelayslightCM_pool;								 
osPoolDef(RelayslightCM_pool, 10, RelayslightCM_MEAS);                  // 内存池定义
osMessageQId  MsgBox_RelayslightCM;
osMessageQDef(MsgBox_RelayslightCM, 2, &RelayslightCM_MEAS);            // 消息队列定义，用于模块线程向无线通讯线程
osMessageQId  MsgBox_MTRelayslightCM;
osMessageQDef(MsgBox_MTRelayslightCM, 2, &RelayslightCM_MEAS);          // 消息队列定义,用于无线通讯线程向模块线程
osMessageQId  MsgBox_DPRelayslightCM;
osMessageQDef(MsgBox_DPRelayslightCM, 2, &RelayslightCM_MEAS);          // 消息队列定义，用于模块线程向显示模块线程
/*---------------------------------------------------------------------------
 * @Description:模块初始化，包括两个按键，两个继电器
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------
 * @Description:模块线程，包括按键处理，状态上传，同步控制
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void RelayslightCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 light1_vlue = 0;
	u8 light2_vlue = 0;
	u8 UPLD_cnt;
	const u8 UPLD_period = 10;

	RelayslightCM_MEAS actuatorData;	//本地输入量
	static RelayslightCM_MEAS  Data_temp   = {0};	//下行数据输入量同步对比缓存
	static RelayslightCM_MEAS  Data_tempDP = {0};	//本地输入量显示数据对比缓存
	uint8_t Kcnt;
	RelayslightCM_MEAS *mptr = NULL;
	RelayslightCM_MEAS *rptr = NULL;
	
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelayslightCM, 50);
		if (evt.status == osEventMessage){		//等待消息指令
			
			rptr = evt.value.p;
			/*自定义本地线程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelayslightCM_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}
		
		if(Data_temp.relay_con != actuatorData.relay_con){
		
			Data_temp.relay_con = actuatorData.relay_con;
			
			
			light1_vlue = (Data_temp.relay_con >> 0) & 0x01;
			light2_vlue = (Data_temp.relay_con >> 1) & 0x01;
			
		}
	
		if(  Data_tempDP.relay_con != actuatorData.relay_con){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			gb_Exmod_key = true;
			gb_databuff[0] = actuatorData.relay_con;
			do{mptr = (RelayslightCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_DPRelayslightCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		if(UPLD_cnt < UPLD_period)
			UPLD_cnt ++;	//数据定时上传
		else{
			//Driver_USART2.Send(gb_databuff+10, 10);
			UPLD_cnt = 0;
			
			gb_Exmod_key = true;
			gb_databuff[0] = actuatorData.relay_con;
			
//			mptr = (RelayslightCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);
//			mptr->relay_con = actuatorData.relay_con;
//			osMessagePut(MsgBox_RelayslightCM, (uint32_t)mptr, 100);
	
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
			gb_Exmod_key = true;
			
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

		light1 = (actuatorData.relay_con >> 0) & 0x01;
		light2 = (actuatorData.relay_con >> 1) & 0x01;
		delay_ms(20);
	}
}
/*---------------------------------------------------------------------------
 * @Description:模块启动API
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void RelayslightCMThread_Active(void){

	RelayslightCM_Init();
	RelayslightCM_pool   = osPoolCreate(osPool(RelayslightCM_pool));	//创建内存池
	MsgBox_RelayslightCM 	= osMessageCreate(osMessageQ(MsgBox_RelayslightCM), NULL);	//创建消息队列
	MsgBox_MTRelayslightCM = osMessageCreate(osMessageQ(MsgBox_MTRelayslightCM), NULL);//创建消息队列
	MsgBox_DPRelayslightCM = osMessageCreate(osMessageQ(MsgBox_DPRelayslightCM), NULL);//创建消息队列
	
	tid_RelayslightCM_Thread = osThreadCreate(osThread(RelayslightCM_Thread),NULL);
}
