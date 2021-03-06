/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     dataTrans_USART
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 15:41:53
 * @Description: 
 * ————数据传输模块，收发线程，数据处理等
 * ---------------------------------------------------------------------------*/

#include <dataTrans_USART.h>//数据传输驱动进程函数，包括有wifi及zigbee；

extern ARM_DRIVER_USART Driver_USART2;

extern osThreadId tid_keyMboard_Thread;	//声明主板按键任务ID，便于传递信息调试使能信号

osThreadId tid_USARTWireless_Thread;
osThreadId tid_UsartRx_Thread;
uint8 gb_databuff[50];

static bool online = false;
static uint16 sendcount = 0;
volatile bool gb_Exmod_key=false;//全局，扩展模块，数据改变标志位
static bool RX_FLG = false; //有效数据获取标志
static uint8 recvbuff[100]={0};

osThreadDef(USARTWireless_Thread,osPriorityNormal,1,1024);
osThreadDef(UsartRx_Thread,osPriorityNormal,1,512);

const u8 dataTransFrameHead_size = 1;
const u8 dataTransFrameHead[dataTransFrameHead_size + 1] = {

	0x7f
};

const u8 dataTransFrameTail_size = 2;
const u8 dataTransFrameTail[dataTransFrameTail_size + 1] = {

	0x0D,0x0A
};
/*---------------------------------------------------------------------------
 * @Description: 8bit  crc  校验
 * @Param:      uint8_t *buf 数据,uint8_t len数据长度
 * @Return:     校验值
 *---------------------------------------------------------------------------*/
uint8_t TRAN_crc8(uint8_t *buf,uint8_t len)
{
	uint8_t crc;
	uint8_t i;

	crc = 0;

	while(len--) 
	{
		crc ^= *buf++;

		for(i = 0; i < 8; i++) 
		{
			if(crc & 0x01)
				crc = (crc >> 1) ^ 0x8C;
			else
				crc >>= 1;
		}
	}
	return crc;
}
/*---------------------------------------------------------------------------
 * @Description:数据校验，未使用
 * @Param:      void *start数据指针, 
 * 		unsigned int s_len 数据长度, 
 * 		void *find查找指针,
 * 		unsigned int f_len查找长度
 * @Return:     无
 *---------------------------------------------------------------------------*/
void *memmem(void *start, unsigned int s_len, void *find,unsigned int f_len){
	
	char *p, *q;
	unsigned int len;
	p = start, q = find;
	len = 0;
	while((p - (char *)start + f_len) <= s_len){
			while(*p++ == *q++){
					len++;
					if(len == f_len)
							return(p - f_len);
			};
			q = find;
			len = 0;
	};
	return(NULL);
}
/*---------------------------------------------------------------------------
 * @Description:串口初始化，以及io初始化
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void USART2Wirless_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/*Initialize the USART driver */
	Driver_USART2.Initialize(myUSART2_callback);
	/*Power up the USART peripheral */
	Driver_USART2.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART2.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART2.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART2.Control (ARM_USART_CONTROL_RX, 1);

//	Driver_USART2.Send("i'm usart2 for wireless datstransfor\r\n", 38);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);		//使能或者失能APB2外设时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;	//底板指示灯
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//最高输出速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	MSGZigbee_rest = 1;
	
}

/*---------------------------------------------------------------------------
 * @Description:串口回调，收发事件触发
 * @Param:      标准的回调函数事件接收，uint32_t event
 * @Return:     无
 *---------------------------------------------------------------------------*/
void myUSART2_callback(uint32_t event){

	uint32_t TX_mask;
	uint32_t RX_mask;
  	TX_mask =ARM_USART_EVENT_TRANSFER_COMPLETE |
	         ARM_USART_EVENT_SEND_COMPLETE     |
	         ARM_USART_EVENT_TX_COMPLETE; 
         	
     RX_mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
			   ARM_USART_EVENT_TRANSFER_COMPLETE |
			   ARM_USART_EVENT_RX_TIMEOUT;
  if (event & TX_mask) {
    /* Success: Wakeup Thread |
			   ARM_USART_EVENT_RX_TIMEOUT*/
    osSignalSet(tid_UsartRx_Thread, 0x01);//接收完成，发送完成signal
    osSignalSet(tid_USARTWireless_Thread, 0x01);
    
  }
  if (event & RX_mask) {
    /* Success: Wakeup Thread */
    osSignalSet(tid_UsartRx_Thread, 0x02);//接收完成，发送完成signal
    osSignalSet(tid_USARTWireless_Thread, 0x02);//接收完成，发送完成signal
  	}
}
/*---------------------------------------------------------------------------
 * @Description:未使用
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
bool ATCMD_INPUT(char *CMD,char *REPLY[2],u8 REPLY_LEN[2],u8 times,u16 timeDelay){
	
	const u8 dataLen = 100;
	u8 dataRXBUF[dataLen];
	u8 loop;
	u8 loopa;
	
	for(loop = 0;loop < times;loop ++){
	
		Driver_USART2.Send(CMD,strlen(CMD));
		osDelay(20);
		Driver_USART2.Receive(dataRXBUF,dataLen);
		osDelay(timeDelay);
		osDelay(20);
		
		for(loopa = 0;loopa < 2;loopa ++)if(memmem(dataRXBUF,dataLen,REPLY[loopa],REPLY_LEN[loopa]))return true;
	}return false;
}
/*---------------------------------------------------------------------------
 * @Description:无线模块初始化，ESP8266-AT指令初始化，
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void USART2Wireless_wifiESP8266Init(void){

	const u8 	InitCMDLen = 9;
	const u16 	timeTab_waitAnsr[InitCMDLen] = {
		
		100,
		100,
		100,
		100,
		100,
		5000,
		3000,
		200,
		200,
	};
	const char *wifiInit_reqCMD[InitCMDLen] = {
		"ATE0\r\n",
		"ATE0\r\n",
		"AT\r\n",
		"AT+CWMODE_DEF=1\r\n",
		"AT+CWDHCP_DEF=1,1\r\n",
		"AT+CWJAP_DEF=\"GTA2017\",\"88888888\"\r\n",
		"AT+CIPSTART=\"TCP\",\"10.2.8.54\",8085\r\n",		//台架一：31.26		台架二：31.27
		"AT+CIPMODE=1\r\n",
		"AT+CIPSEND\r\n"
	};
	const u8 REPLY_DATLENTAB[InitCMDLen][2] = {
		
		{2,2},
		{2,2},
		{2,2},
		{2,2},
		{2,2},
		{2,14},
		{2,6},
		{2,2},
		{1,1},
	};
	const char *wifiInit_REPLY[InitCMDLen][2] = {
		
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","WIFI CONNECTED"},
		{"OK","CONNECT"},
		{"OK","OK"},
		{">",">"}
	};
	
	u8 loop;
	
	Driver_USART2.Send("+++",3);osDelay(100);
	Driver_USART2.Send("+++\r\n",5);osDelay(100);
	Driver_USART2.Send("+++",3);osDelay(100);
	Driver_USART2.Send("+++\r\n",5);osDelay(100);
	
	for(loop = 0;loop < InitCMDLen;loop ++)
		if(false == ATCMD_INPUT((char *)wifiInit_reqCMD[loop],
								(char **)wifiInit_REPLY[loop],
								(u8 *)REPLY_DATLENTAB[loop],
								3,
								timeTab_waitAnsr[loop])
								)loop = 0;
	
	beeps(6);
}
/*---------------------------------------------------------------------------
 * @Description:地址映射
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
u8 Extension_IDCHG(u8 Addr_in){
	
	switch(Addr_in){
	
		case MID_EXEC_DEVPWM:	return 0x32;
			
		case MID_EXEC_CURTAIN:	return 0x31;
		
		case MID_EXEC_RELAYS:	return 0x30;
			
		case MID_EXEC_SPEAK:	return 0x24;
			
		case MID_SENSOR_FIRE:	return 0x11;
			
		case MID_SENSOR_PYRO:	return 0x12;
			
		case MID_SENSOR_SMOKE:	return 0x13;
			
		case MID_SENSOR_GAS:	return 0x14;
			
		case MID_SENSOR_TEMP:	return 0x22;
			
		case MID_SENSOR_LIGHT:	return 0x23;
			
		case MID_SENSOR_ANALOG:	return 0x21;
			
		case MID_EXEC_DEVIFR:	return 0x34;
			
		case MID_EXEC_SOURCE:	return 0x33;
			
		case MID_EGUARD:		return 0x0A;
		
		default:return 0;
	}
}

/*****************发送帧数据填装*****************/
//发送帧缓存，命令，模块地址，数据长度，核心数据包，核心数据包长
u16 dataTransFrameLoad_TX(u8 bufs[],u8 cmd,u8 Maddr,u8 dats[],u8 datslen){

	u16 memp;
	
	memp = 0;
	
	memcpy(&bufs[memp],dataTransFrameHead,dataTransFrameHead_size); //帧头填充
	memp += dataTransFrameHead_size;	//指针后推
	bufs[memp ++] = Moudle_GTA.Wirless_ID;
	bufs[memp ++] = cmd;
	bufs[memp ++] = Extension_IDCHG(Moudle_GTA.Extension_ID);
	bufs[memp ++] = datslen;
	memcpy(&bufs[memp],dats,datslen);
	memp += datslen;
	memcpy(&bufs[memp],dataTransFrameTail,dataTransFrameTail_size);
	memp += dataTransFrameTail_size;
	
	return memp;
}
/*---------------------------------------------------------------------------
 * @Description:串口接收线程--->zigbee无线模块
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void UsartRx_Thread(const void *argument	)
{
	uint8 recv_b[100]={0};

	LED_MSGZigbee_OK = 0;
	MSGZigbee_rest = 0;
	delay_ms(10);
	MSGZigbee_rest = 1;
	delay_ms(20);
	
	
	while(1)
	{
		
		

		memset(recv_b,0,100);
		//osSignalWait(0x02, osWaitForever);
		
		Driver_USART2.Receive(recv_b,64);
		osSignalWait(0x02, osWaitForever);
		memcpy(recvbuff,recv_b,100);//放入缓存

		if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa  )
		{
			if( recvbuff[18] == 3)//查询CMD
			{
				if(recvbuff[19] == 0)
					online = false;
				else if(recvbuff[19] == 1)
					online = true;
			}
			else if( recvbuff[18] == 5)
			{
				RX_FLG = true;
				
			}

			
		}
		
		if(online)
				led2_status = led2_b;
			else
				led2_status = led2_g;
		
	}
	
}
/*---------------------------------------------------------------------------
 * @Description:切换zigbee状态：入网 <----> 退网
 * @Param:      无
 * @Return:     返回当前网络状态
 *---------------------------------------------------------------------------*/
LEDstatus zigbee_sw(void)
{
	uint8 sendbuff[100]={0};
	
	
	if(led2_status == led2_b_flash)
		{
			uint8 i = 0;
			for(;i < 200;i++ )
			{
				if (led2_status == led2_b_flash)
				{
					TRAN_D_struct joinTRAN;

					memset(sendbuff,0,100);
					

					memset(&joinTRAN,0,sizeof(joinTRAN));

					memset(sendbuff,0xAA,2);

					memset(sendbuff+17+2,0xBB,2);

					joinTRAN.TYPE_NUM = 0x03;//子模块stm32--->路由器zigbee

					joinTRAN.data_len = 2;

					sendbuff[2] = sendcount++;

					memcpy(sendbuff+3,&joinTRAN,sizeof(joinTRAN));
					
					if( online )//初始化查询，是否入网
					{
						sendbuff[17] = 0;
						sendbuff[18] = 2;
						
					}
					else 
					{
						sendbuff[17] = 0;
						sendbuff[18] = 1;
					}
					
					sendbuff[17+2+2] = TRAN_crc8(sendbuff,17+2+2);

					if( i == 3 || i == 20)//只发两次
					{
						Driver_USART2.Send(sendbuff,17+2+2+1);
						osSignalWait(0x01, osWaitForever);
					}

				
				}
				else if(led2_status == led2_b)
					return led2_status;
				else
					return led2_g;
				
				
				delay_ms(200);
				}
		}
	else
	{
		return led2_status;
	}

	
	return led2_g;
}
/*---------------------------------------------------------------------------
 * @Description:zigbee模块，无线透传， 数据上传
 * @Param:      模块ID
 * @Return:     函数状态
 *---------------------------------------------------------------------------*/
uint8 zigbee_updata(uint8 EXmodID)
{
	uint8 sendbuff[100]={0};
	osEvent  evt;
    osStatus status;
	
	TRAN_D_struct updataTRAN;

	memset(sendbuff,0,100);
	memset(&updataTRAN,0,sizeof(TRAN_D_struct));

	memset(sendbuff,0xAA,2);
	updataTRAN.TYPE_NUM = 0x03;//子模块stm32--->路由器zigbee
	sendbuff[2] = sendcount++;
	sendbuff[18] = 0x04;//CMD----->updata
	sendbuff[20] = EXmodID;
	switch (EXmodID)
		{
			case MID_SENSOR_FIRE :{
				
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							
							
					}break;			
			case MID_SENSOR_PYRO :{
				
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							
						
					}break;			
			case MID_SENSOR_SMOKE :{
				
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
						
					}break;			
			case MID_SENSOR_GAS:{
						updataTRAN.data_len = 2+16;					
						memcpy(sendbuff+15+updataTRAN.data_len,gb_databuff, 2);//17+updataTRAN.data_len-2
						
					}break;			
			case MID_SENSOR_TEMP:{

						updataTRAN.data_len = 8+16;	
						memcpy(sendbuff+9+updataTRAN.data_len,gb_databuff, 8);//17+updataTRAN.data_len-8
						
					}break; 
			case MID_SENSOR_SHAKE :{
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
						
						
					}break;
			case MID_SENSOR_LIGHT:{
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
									
						
					}break;			
			case MID_SENSOR_ANALOG:{
					//暂无数据
					updataTRAN.data_len = 16+16;					
					memcpy(sendbuff+1+updataTRAN.data_len,gb_databuff, 16);//17+updataTRAN.data_len-1
						
					}break;					
			case MID_EGUARD:{
						EGUARD_MEAS *rptr = NULL;
						evt = osMessageGet(MsgBox_EGUD, 100);
				
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						switch(gb_databuff[0]){
							
								case FID_EXERES_SUCCESS:
								case FID_EXERES_FAIL:
								case FID_EXERES_TTIT:
										gb_databuff[0] = rptr->CMD;
										gb_databuff[1] = rptr->DAT;	
										updataTRAN.data_len = 2+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 2);
										break;
								case RFID_EXERES_TTIT:
										gb_databuff[0] = rptr->CMD;
										updataTRAN.data_len = 5+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 5);
										break;
								case PSD_EXERES_TTIT:
										gb_databuff[0] = rptr->CMD;
										memcpy(gb_databuff+1,rptr->Psd,8);
										updataTRAN.data_len = 9+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 9);
										break;
								case PSD_EXERES_LVMSG_DN:
										gb_databuff[0] = rptr->CMD;
										updataTRAN.data_len = 1+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);
										break;								
								case PSD_EXERES_LVMSG_UP:
										gb_databuff[0] = rptr->CMD;
										updataTRAN.data_len = 1+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);
										break;
								case PSD_EXERES_CALL:
										gb_databuff[0] = rptr->CMD;
										updataTRAN.data_len = 1+16;					
										memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);
										break;
								case DLOCK_EXERES_TTIT:
										gb_databuff[0] = rptr->CMD;
										gb_databuff[1] = rptr->DAT;
										updataTRAN.data_len = 2+16;					
										memcpy(sendbuff+15+updataTRAN.data_len,gb_databuff, 2);
										break;
								default: 
									break;
								}
							do{status = osPoolFree(EGUD_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
							}
					} break;						
			case MID_EXEC_DEVIFR:{

						updataTRAN.data_len = 3+16;					
						memcpy(sendbuff+14+updataTRAN.data_len,gb_databuff, 3);//17+updataTRAN.data_len-1
						
//						IFR_MEAS *rptr = NULL;
//				
//						evt = osMessageGet(MsgBox_IFR, 100);
//						if (evt.status == osEventMessage) {		//等待消息指令
//							
//							rptr = evt.value.p;
//							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
//							
//							osDelay(20);
//							do{status = osPoolFree(IFR_pool, rptr);}while(status != osOK);	//内存释放
//							rptr = NULL;
//						}
					}break;			
			case MID_EXEC_DEVPWM:{
			
						updataTRAN.data_len = 2+16;					
						memcpy(sendbuff+15+updataTRAN.data_len,gb_databuff, 2);//17+updataTRAN.data_len-1
						
					}break;
			case MID_EXEC_DEVPWM_FAN:{
			
						updataTRAN.data_len = 2+16;					
						memcpy(sendbuff+15+updataTRAN.data_len,gb_databuff, 2);//17+updataTRAN.data_len-1
						
					}break;			
			case MID_EXEC_RELAYS:{
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							
					}break;
			case MID_EXEC_RELAYSLIGHT:{
				
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
					
							updataTRAN.data_len = 1+16;					
							memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							
					}break;	
			case MID_EXEC_RELAYSOUTLET:{
				
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
					
							updataTRAN.data_len = 5+16;					
							memcpy(sendbuff+12+updataTRAN.data_len,gb_databuff, 5);//17+updataTRAN.data_len-1
							
					}break;	
			case MID_EXEC_CURTAIN:{
							updataTRAN.data_len = 1+16;					
							memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							
						
					}break;			
			case MID_EXEC_SOURCE :{//电源模块数据发送
							updataTRAN.data_len = 12+16;					
							memcpy(sendbuff+5+updataTRAN.data_len,gb_databuff, 12);//17+updataTRAN.data_len-1
							
						
					}break;			
			case MID_EXEC_SPEAK:{
			
						speakCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_speakCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
						
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(speakCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			default:break;
		}

	
	//beeps(1);
	memcpy(sendbuff+3,&updataTRAN,sizeof(TRAN_D_struct));
	
	memset(sendbuff+17+updataTRAN.data_len,0xBB,2);

	sendbuff[19+updataTRAN.data_len] = TRAN_crc8(sendbuff,19+updataTRAN.data_len);//17+data_len+2
	Driver_USART2.Send(sendbuff,20+updataTRAN.data_len);//sendbuff,17+2+2+1
	osSignalWait(0x01, osWaitForever);
	
	
	return 0;
}
/*---------------------------------------------------------------------------
 * @Description:无线模块主线程，负责数据发送，网络状态切换
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void USARTWireless_Thread(const void *argument){
	
	osEvent  evt;
    osStatus status;
	
	
	
	const u8 frameDatatrans_totlen = 100;	//帧缓存限长
	const u8 dats_BUFtemp_len = frameDatatrans_totlen - 20;	//核心数据包缓存限长
	u8 dataTrans_TXBUF[frameDatatrans_totlen] = {0};  //发送帧缓存
	u8 dataTrans_RXBUF[frameDatatrans_totlen] = {0};	//接收帧缓存
	u8 TXdats_BUFtemp[dats_BUFtemp_len] = {0};	//发送核心数据包缓存
	u8 RXdats_BUFtemp[dats_BUFtemp_len] = {0};	//接收核心数据包缓存
	u8 memp = 1;
	TRAN_D_struct recv_tranbuff;
	u8 datacount = 0;
	u8 datacount_last = 0;
	
	osSignalWait(WIRLESS_THREAD_EN,osWaitForever);		//等待进程使能信号
	osSignalClear(tid_USARTWireless_Thread,WIRLESS_THREAD_EN);
	
	switch(Moudle_GTA.Wirless_ID){		//初始化先行，在进程激活前
	
		case MID_TRANS_Zigbee	:	; break;
			
		case MID_TRANS_Wifi		:	 USART2Wireless_wifiESP8266Init(); break;
			
		default:break;
	}
	
//	Moudle_GTA.Extension_ID = MID_EGUARD;   	/****调试语句*****/
//	Moudle_GTA.Wirless_ID = 0xAA;   			/****调试语句*****/
	if(memp)
		memp=0;
	for(;;){

		
		led2_status = zigbee_sw();
		
		if( online && gb_Exmod_key)//更新线程
		{	
			gb_Exmod_key = false;
			zigbee_updata(Moudle_GTA.Extension_ID);
		}
		
		
/*************↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓无线数据接收，处理推送至驱动级进程↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*********************/
		if(RX_FLG){
			RX_FLG = false;


			//查找对比
			if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa)
			{
				uint8  crc=0xfe;
				memcpy(&recv_tranbuff,recvbuff+3,17);
				crc = TRAN_crc8(recvbuff,19+recv_tranbuff.data_len);//17+recv_tranbuff.data_len+2

				if(crc == recvbuff[19+recv_tranbuff.data_len])//17+recv_tranbuff.data_len+2
				{
					datacount = dataTrans_RXBUF[2];//存放包序号
					if(datacount != datacount_last)//过滤同包
					{
						datacount_last = datacount;
						
					}

				}
			}
			
			switch(Moudle_GTA.Extension_ID){	//数据接收
			
				case MID_SENSOR_FIRE :{
				
						fireMS_MEAS *mptr = NULL;
						
						do{mptr = (fireMS_MEAS *)osPoolCAlloc(fireMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTfireMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_SENSOR_PYRO :{
				
						pyroMS_MEAS *mptr = NULL;
						
						do{mptr = (pyroMS_MEAS *)osPoolCAlloc(pyroMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTpyroMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_SENSOR_SMOKE :{
				
						smokeMS_MEAS *mptr = NULL;
						
						do{mptr = (smokeMS_MEAS *)osPoolCAlloc(smokeMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTsmokeMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_SENSOR_GAS  :{
				
						gasMS_MEAS *mptr = NULL;
						
						do{mptr = (gasMS_MEAS *)osPoolCAlloc(gasMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTgasMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_SENSOR_TEMP :{
				
						tempMS_MEAS *mptr = NULL;
						
						do{mptr = (tempMS_MEAS *)osPoolCAlloc(tempMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTtempMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;
				case MID_SENSOR_SHAKE :{
				
						shakeMS_MEAS *mptr = NULL;
						
						do{mptr = (shakeMS_MEAS *)osPoolCAlloc(shakeMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTshakeMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;
				case MID_SENSOR_LIGHT:{
				
						lightMS_MEAS *mptr = NULL;
						
						do{mptr = (lightMS_MEAS *)osPoolCAlloc(lightMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTlightMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_SENSOR_ANALOG:{
				
						analogMS_MEAS *mptr = NULL;
						
						do{mptr = (analogMS_MEAS *)osPoolCAlloc(analogMS_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						//仅上行，无下发数据
						
						osMessagePut(MsgBox_MTanalogMS, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_EGUARD:{
					
						EGUARD_MEAS *mptr = NULL;
						u8 cmdDevType = recvbuff[15+recv_tranbuff.data_len] & 0xf0;	//头字节高四位代表下行指令设备区分码，低四位为实际指令
						
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						switch(cmdDevType){
						
							case 0x00:{		//指纹
								
								do{mptr = (EGUARD_MEAS *)osPoolCAlloc(EGUD_pool);}while(mptr == NULL);
								mptr -> CMD = recvbuff[15+recv_tranbuff.data_len];  //下行命令加载
								mptr -> DAT = recvbuff[16+recv_tranbuff.data_len];  //下行数据加载
								osMessagePut(MsgBox_MTEGUD_FID, (uint32_t)mptr, osWaitForever);		//指令推送至模块驱动
							}break;
								
							case 0x10:{		//门锁
								
								do{mptr = (EGUARD_MEAS *)osPoolCAlloc(EGUD_pool);}while(mptr == NULL);
								mptr -> CMD = recvbuff[15+recv_tranbuff.data_len];  //下行命令加载
								mptr -> DAT = recvbuff[16+recv_tranbuff.data_len];  //下行数据加载
								osMessagePut(MsgBox_MTEGUD_DLOCK, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
							}break;
							
							case 0x20:{		//RFID（保留）
							
							
							}break;
							
							case 0x30:{		//键盘（保留）
							
							
							}break;
								
							default:break;
						}
						
						osDelay(100);
					}break;				
				case MID_EXEC_DEVIFR:{
				
						IFR_MEAS *mptr = NULL;
						
						do{mptr = (IFR_MEAS *)osPoolCAlloc(IFR_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->Mod_addr = recvbuff[15+recv_tranbuff.data_len];
						mptr->VAL_KEY  = recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTIFR, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_EXEC_DEVPWM:{
					
						pwmCM_MEAS *mptr = NULL;
						
						do{mptr = (pwmCM_MEAS *)osPoolCAlloc(pwmCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						
						mptr->Switch 	=  recvbuff[15+recv_tranbuff.data_len];
						mptr->pwmVAL 	=  recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTpwmCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;
				case MID_EXEC_DEVPWM_FAN:{
					
						pwmCM_MEAS *mptr = NULL;
						
						do{mptr = (pwmCM_MEAS *)osPoolCAlloc(pwmCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->Mod_addr 	= recvbuff[14+recv_tranbuff.data_len];
						mptr->Switch 	= recvbuff[15+recv_tranbuff.data_len];
						mptr->pwmVAL 	= recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTpwmCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_EXEC_RELAYS:{
					
						RelaysCM_MEAS *mptr = NULL;
						
						do{mptr = (RelaysCM_MEAS *)osPoolCAlloc(RelaysCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->relay_con = recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTRelaysCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;					
				case MID_EXEC_RELAYSLIGHT:{
					
						RelaysCM_MEAS *mptr = NULL;
						
						do{mptr = (RelaysCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->relay_con = recvbuff[16+recv_tranbuff.data_len];//17+1+recv_tranbuff.data_len
						
						osMessagePut(MsgBox_MTRelayslightCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;
				case MID_EXEC_RELAYSOUTLET:{
					
						RelaysoutletCM_MEAS *mptr = NULL;
						
						do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->relay_con = recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTRelaysoutletCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_EXEC_CURTAIN:{
					
						curtainCM_MEAS *mptr = NULL;
						
						do{mptr = (curtainCM_MEAS *)osPoolCAlloc(curtainCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						
						mptr->valACT = recvbuff[16+recv_tranbuff.data_len];
						
						osMessagePut(MsgBox_MTcurtainCM, (uint32_t)mptr, osWaitForever); //指令推送至模块驱动
						osDelay(100);
					}break;				
				case MID_EXEC_SOURCE :{//电源控制模块串口接收
						//仅上行，无下发数据
//						sourceCM_MEAS *mptr = NULL;
//						
//						do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);
//						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
//						
//						mptr->source_addr 	= RXdats_BUFtemp[0];
//						mptr->Switch = RXdats_BUFtemp[1];
//
//						osMessagePut(MsgBox_MTsourceCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
//						osDelay(100);
					}break;				
				case MID_EXEC_SPEAK:{
				
						speakCM_MEAS *mptr = NULL;

						do{mptr = (speakCM_MEAS *)osPoolCAlloc(speakCM_pool);}while(mptr == NULL);
						/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
						
						mptr->spk_num = RXdats_BUFtemp[0];
						
						osMessagePut(MsgBox_MTspeakCM, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
						osDelay(100);
					}break;
				
				default:break;
			}
		}
		
/****************↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑驱动级进程数据接收，处理后推送至无线数据传输↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*************************/
		memset(TXdats_BUFtemp, 0, sizeof(u8) * dats_BUFtemp_len);
		if(0)
		switch(Moudle_GTA.Extension_ID){	//数据发送
		
			case MID_SENSOR_FIRE :{
				
						fireMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_fireMS, 100);		
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(fireMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}	
					}break;
			
			case MID_SENSOR_PYRO :{
				
						pyroMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pyroMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
				
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pyroMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_SMOKE :{
				
						smokeMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_smokeMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							//模拟通道值暂时保留，不上传
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
														
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(smokeMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_GAS:{
				
						gasMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_gasMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							//模拟通道值暂时保留，不上传
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(gasMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_TEMP:{
				
						tempMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_tempMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							if(rptr->temp >= 0.0)TXdats_BUFtemp[0] = 1;
							else TXdats_BUFtemp[0] = 2; 
							
							TXdats_BUFtemp[1] = (char)rptr->temp;
							TXdats_BUFtemp[2] = (char)((rptr->temp - (float)TXdats_BUFtemp[1])*100);
							
							TXdats_BUFtemp[3] = (char)rptr->hum;
							TXdats_BUFtemp[4] = (char)((rptr->hum - (float)TXdats_BUFtemp[3])*100);
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,5);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(tempMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			case MID_SENSOR_SHAKE :{
				
						shakeMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_shakeMS, 100);		
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(shakeMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}	
					}break;
			case MID_SENSOR_LIGHT:{
				
						lightMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_lightMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = (char)(rptr->illumination);
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(lightMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_ANALOG:{
				
						analogMS_MEAS *rptr = NULL;
				
						float CUR = 0.0;
						float VOL = 0.0;
				
						evt = osMessageGet(MsgBox_analogMS, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							VOL = 420.0 / 4096.0 * (float)rptr->Ich1;		//电压
							CUR = 6.1 / 4096.0 * (float)rptr->Ich2;			//电流
							
							TXdats_BUFtemp[0] = (uint8_t)((uint32_t)VOL % 10000 / 100);
							TXdats_BUFtemp[1] = (uint8_t)((uint32_t)VOL % 100);
							TXdats_BUFtemp[2] = (uint8_t)((uint32_t)((VOL - TXdats_BUFtemp[1]) * 100.0)) % 100;
							
							TXdats_BUFtemp[3] = (uint8_t)((uint32_t)CUR % 100);
							TXdats_BUFtemp[4] = (uint8_t)((uint32_t)((CUR - TXdats_BUFtemp[3]) * 100.0)) % 100;
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,5);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(analogMS_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
					
			case MID_EGUARD:{
				
						EGUARD_MEAS *rptr = NULL;
						evt = osMessageGet(MsgBox_EGUD, 100);
				
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							switch(rptr->CMD){
							
								case FID_EXERES_SUCCESS:
								case FID_EXERES_FAIL:
								case FID_EXERES_TTIT:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										TXdats_BUFtemp[1] = rptr->DAT;	
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,2);
										break;
								
								case RFID_EXERES_TTIT:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										memcpy(&TXdats_BUFtemp[1],rptr->rfidDAT,4);
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,5);
										break;
								
								case PSD_EXERES_TTIT:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										memcpy(&TXdats_BUFtemp[1],rptr->Psd,8);
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,9);
										break;
								
								case PSD_EXERES_LVMSG_DN:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
										break;
								
								case PSD_EXERES_LVMSG_UP:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
										break;
								
								case PSD_EXERES_CALL:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
										break;
								
								case DLOCK_EXERES_TTIT:
									
										TXdats_BUFtemp[0] = rptr->CMD;
										TXdats_BUFtemp[1] = rptr->DAT;
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,2);
										break;
								
								default: 
									
										TXdats_BUFtemp[0] = ABNORMAL_DAT;
										memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							}

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(EGUD_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}						
					}break;			
			
			case MID_EXEC_DEVIFR:{
			
						IFR_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_IFR, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->VAL_KEY;
							TXdats_BUFtemp[2] = rptr->STATUS;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,2);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(IFR_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_DEVPWM:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->Switch;
							TXdats_BUFtemp[2] = rptr->pwmVAL;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,3);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			case MID_EXEC_DEVPWM_FAN:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->Switch;
							TXdats_BUFtemp[2] = rptr->pwmVAL;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,3);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_RELAYS:{
				
						RelaysCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_RelaysCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->relay_con;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(RelaysCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			case MID_EXEC_RELAYSLIGHT:{
				
						RelaysCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_RelayslightCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->relay_con;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(RelayslightCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_CURTAIN:{
				
						curtainCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_curtainCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->valACT;
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(curtainCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_SOURCE :{//电源模块数据发送
				
						sourceCM_MEAS *rptr = NULL;
				
						float CUR;
				
						evt = osMessageGet(MsgBox_sourceCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							if((rptr->anaVal * (4.5 / 5586)) < 2.5)CUR = 0.0;
							else{
							
								CUR = (rptr->anaVal * (4.5 / 5586) - 2.5) * (10.0 / 2.0);
							}
							TXdats_BUFtemp[0] = rptr->source_addr;
							TXdats_BUFtemp[1] = rptr->Switch;
							TXdats_BUFtemp[2] = (uint8_t)((uint32_t)CUR % 100);
							TXdats_BUFtemp[3] = (uint8_t)((uint32_t)((CUR - TXdats_BUFtemp[2]) * 100.0)) % 100;
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,4);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_SPEAK:{
			
						speakCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_speakCM, 100);
						if (evt.status == osEventMessage) {		//等待消息指令
							
							rptr = evt.value.p;
							/*自定义发送数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
							
							TXdats_BUFtemp[0] = rptr->spk_num;
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(speakCM_pool, rptr);}while(status != osOK);	//内存释放
							rptr = NULL;
						}
					}break;
			
			default:break;
		}
		memset(dataTrans_TXBUF, 0, sizeof(u8) * frameDatatrans_totlen);
		delay_ms(10);
	}
}
/*---------------------------------------------------------------------------
 * @Description:模块初始化
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void USART_WirelessInit(void){

	USART2Wirless_Init();
	
}
/*---------------------------------------------------------------------------
 * @Description:无线模块启动
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void wirelessThread_Active(uint8 status){
	
	if(status && led2_status == led2_r)
	{	
		led2_status = led2_g;
		USART_WirelessInit();
		tid_USARTWireless_Thread = osThreadCreate(osThread(USARTWireless_Thread),NULL);
		tid_UsartRx_Thread = osThreadCreate(osThread(UsartRx_Thread),NULL);
	}
	else if(status == 0)
	{
		led2_status = led2_r;
		osThreadTerminate(tid_USARTWireless_Thread);
		osThreadTerminate(tid_UsartRx_Thread);
	}
	
}
