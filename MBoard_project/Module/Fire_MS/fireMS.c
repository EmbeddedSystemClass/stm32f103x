/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     fireMS
 * @Author:     RanHongLiang
 * @Date:       2019-07-02 11:40:13
 * @Description: 
 *————火焰传感器模块
 *---------------------------------------------------------------------------*/

#include "fireMS.h" //火焰传感器检测驱动的进程函数

extern ARM_DRIVER_USART Driver_USART1; //设备驱动库串口一设备声明

osThreadId tid_fireMS_Thread;
osThreadDef(fireMS_Thread, osPriorityNormal, 1, 512);

osPoolId fireMS_pool;
osPoolDef(fireMS_pool, 10, fireMS_MEAS); // 内存池定义
osMessageQId MsgBox_fireMS;
osMessageQDef(MsgBox_fireMS, 2, &fireMS_MEAS); // 消息队列定义，用于模块进程向无线通讯进程
osMessageQId MsgBox_MTfireMS;
osMessageQDef(MsgBox_MTfireMS, 2, &fireMS_MEAS); // 消息队列定义,用于无线通讯进程向模块进程
osMessageQId MsgBox_DPfireMS;
osMessageQDef(MsgBox_DPfireMS, 2, &fireMS_MEAS); // 消息队列定义，用于模块进程向显示模块进程
/*---------------------------------------------------------------------------
 * @Description:传感器IO初始化    
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void fireDIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*---------------------------------------------------------------------------
 * @Description:传感器初始化，接口    
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void fireMS_Init(void)
{

	fireDIO_Init();
}
/*---------------------------------------------------------------------------
 * @Description:传感器线程函数，包括定时上传，触发上传    
 * @Param:      创建线程时初始化参数
 * @Return:     无
 *---------------------------------------------------------------------------*/
void fireMS_Thread(const void *argument)
{

	osEvent evt;
	osStatus status;

	const bool UPLOAD_MODE = false; //1：数据变化时才上传 0：周期定时上传

	const uint8_t upldPeriod = 5; //数据上传周期因数（UPLOAD_MODE = false 时有效）

	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;

	const uint8_t dpSize = 30;
	const uint8_t dpPeriod = 40;

	static uint8_t Pcnt = 0;
	char disp[dpSize];

	fireMS_MEAS sensorData;
	static fireMS_MEAS Data_temp = {1};
	static fireMS_MEAS Data_tempDP = {1};

	fireMS_MEAS *mptr = NULL;
	fireMS_MEAS *rptr = NULL;

	for (;;)
	{

		/***********************本地进程数据接收***************************************************/
		//传感器构件数据仅作上传，接收数据功能保留，暂时不用
		evt = osMessageGet(MsgBox_MTfireMS, 100);
		if (evt.status == osEventMessage)
		{ //等待消息指令

			rptr = evt.value.p;
			/*自定义本地进程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/

			do
			{
				status = osPoolFree(fireMS_pool, rptr);
			} while (status != osOK); //内存释放
			rptr = NULL;
		}

		/***********************驱动数据采集*****************************************************/
		sensorData.VAL = !FIRE_DATA; //数据采集

		if (!UPLOAD_MODE)
		{ //选择上传触发模式

			if (UPLDcnt < upldPeriod)
				UPLDcnt++;
			else
			{

				UPLDcnt = 0;
				UPLD_EN = true;
			}
		}
		else
		{

			if (Data_temp.VAL != sensorData.VAL)
			{ //数据推送（数据更替时才触发）

				Data_temp.VAL = sensorData.VAL;
				UPLD_EN = true;
			}
		}

		/***********************进程数据推送*****************************************************/
		if (UPLD_EN)
		{

			UPLD_EN = false;
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;

			osDelay(10);
		}

		if (Data_tempDP.VAL != sensorData.VAL)
		{ //数据推送（数据更替时才触发）

			Data_tempDP.VAL = sensorData.VAL;
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;
			do
			{
				mptr = (fireMS_MEAS *)osPoolCAlloc(fireMS_pool);
			} while (mptr == NULL); //1.44寸液晶显示消息推送
			mptr->VAL = sensorData.VAL;
			osMessagePut(MsgBox_DPfireMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
		/***********************Debug_log*********************************************************/
		if (Pcnt < dpPeriod)
		{
			osDelay(10);
			Pcnt++;
		}
		else
		{

			Pcnt = 0;
			memset(disp, 0, dpSize * sizeof(char));
			sprintf(disp, "\n\ris firing now? : %d\n\r", Data_temp.VAL);
			Driver_USART1.Send(disp, strlen(disp));
			osDelay(20);
		}
		osDelay(10);
	}
}
/*---------------------------------------------------------------------------
 * @Description:传感器线程，接口    
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void fireMSThread_Active(void)
{

	static bool memInit_flg = false;

	if (!memInit_flg)
	{

		fireMS_pool = osPoolCreate(osPool(fireMS_pool));		      //创建内存池
		MsgBox_fireMS = osMessageCreate(osMessageQ(MsgBox_fireMS), NULL);     //创建消息队列
		MsgBox_MTfireMS = osMessageCreate(osMessageQ(MsgBox_MTfireMS), NULL); //创建消息队列
		MsgBox_DPfireMS = osMessageCreate(osMessageQ(MsgBox_DPfireMS), NULL); //创建消息队列

		memInit_flg = true;
	}

	fireMS_Init();
	tid_fireMS_Thread = osThreadCreate(osThread(fireMS_Thread), NULL);
}
