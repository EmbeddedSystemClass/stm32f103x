#include <dataTrans_USART.h>//���ݴ����������̺�����������wifi��zigbee��

extern ARM_DRIVER_USART Driver_USART2;

extern osThreadId tid_keyMboard_Thread;	//�������尴������ID�����ڴ�����Ϣ����ʹ���ź�

osThreadId tid_USARTWireless_Thread;
osThreadId tid_UsartRx_Thread;
uint8 gb_databuff[50];

static bool online = false;
static uint16 sendcount = 0;
volatile bool gb_Exmod_key=false;
static bool RX_FLG = false; //��Ч���ݻ�ȡ��־
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
/*********************************************************************
 *  crc
 */
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
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);		//ʹ�ܻ���ʧ��APB2����ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;	//�װ�ָʾ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//����������50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	MSGZigbee_rest = 1;
	
}


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
    osSignalSet(tid_UsartRx_Thread, 0x01);//������ɣ��������signal
    osSignalSet(tid_USARTWireless_Thread, 0x01);
    
  }
  if (event & RX_mask) {
    /* Success: Wakeup Thread */
    osSignalSet(tid_UsartRx_Thread, 0x02);//������ɣ��������signal
    osSignalSet(tid_USARTWireless_Thread, 0x02);//������ɣ��������signal
  	}
}

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
		"AT+CIPSTART=\"TCP\",\"10.2.8.54\",8085\r\n",		//̨��һ��31.26		̨�ܶ���31.27
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

/*****************����֡������װ*****************/
//����֡���棬���ģ���ַ�����ݳ��ȣ��������ݰ����������ݰ���
u16 dataTransFrameLoad_TX(u8 bufs[],u8 cmd,u8 Maddr,u8 dats[],u8 datslen){

	u16 memp;
	
	memp = 0;
	
	memcpy(&bufs[memp],dataTransFrameHead,dataTransFrameHead_size); //֡ͷ���
	memp += dataTransFrameHead_size;	//ָ�����
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
		memcpy(recvbuff,recv_b,100);//���뻺��

		if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa  )
		{
			if( recvbuff[18] == 3)//��ѯCMD
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

//�л�zigbee״̬������ <----> ����
LEDstatus zigbee_sw(void)
{
	uint8 sendbuff[100]={0};
	
	
	if(led2_status == led2_b_flash)
		{
			uint8 i = 0;
			for(;i < 80;i++ )
			{
				if (led2_status == led2_b_flash)
				{
					TRAN_D_struct joinTRAN;

					memset(sendbuff,0,100);
					

					memset(&joinTRAN,0,sizeof(joinTRAN));

					memset(sendbuff,0xAA,2);

					memset(sendbuff+17+2,0xBB,2);

					joinTRAN.TYPE_NUM = 0x03;//��ģ��stm32--->·����zigbee

					joinTRAN.data_len = 2;

					sendbuff[2] = sendcount++;

					memcpy(sendbuff+3,&joinTRAN,sizeof(joinTRAN));
					
					if( online )//��ʼ����ѯ���Ƿ�����
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

					if( i == 3 || i == 20)//ֻ������
					{
						Driver_USART2.Send(sendbuff,17+2+2+1);
						osSignalWait(0x01, osWaitForever);
					}

				
				}
				else
					return led2_status;
				
				
				delay_ms(200);
				}
		}
	else
	{
		return led2_status;
	}

	
	return led2_g;
}
uint8 zigbee_updata(uint8 EXmodID)
{
	uint8 sendbuff[100]={0};
	osEvent  evt;
    osStatus status;
	
	TRAN_D_struct updataTRAN;

	memset(sendbuff,0,100);
	memset(&updataTRAN,0,sizeof(TRAN_D_struct));

	memset(sendbuff,0xAA,2);
	updataTRAN.TYPE_NUM = 0x03;//��ģ��stm32--->·����zigbee
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
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 2);//17+updataTRAN.data_len-1
						
					}break;			
			case MID_SENSOR_TEMP:{
						updataTRAN.data_len = 8+16;		
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 8);//17+updataTRAN.data_len-1
						
					}break;
			case MID_SENSOR_SHAKE :{
						updataTRAN.data_len = 1+16;					
						memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
						
						
					}break;
			case MID_SENSOR_LIGHT:{
				
						
					}break;			
			case MID_SENSOR_ANALOG:{
				
						analogMS_MEAS *rptr = NULL;
				
						
						evt = osMessageGet(MsgBox_analogMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							osDelay(20);
							do{status = osPoolFree(analogMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;					
			case MID_EGUARD:{
				
						EGUARD_MEAS *rptr = NULL;
						evt = osMessageGet(MsgBox_EGUD, 100);
				
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->CMD){
							
								case FID_EXERES_SUCCESS:
								case FID_EXERES_FAIL:
								case FID_EXERES_TTIT:
									
										break;
								
								case RFID_EXERES_TTIT:
									
										break;
								
								case PSD_EXERES_TTIT:
									
										break;
								
								case PSD_EXERES_LVMSG_DN:
									
										break;
								
								case PSD_EXERES_LVMSG_UP:
									
										break;
								
								case PSD_EXERES_CALL:
									
										break;
								
								case DLOCK_EXERES_TTIT:
									
										break;
								
								default: 
									;
							}

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(EGUD_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}						
					}break;						
			case MID_EXEC_DEVIFR:{
			
						IFR_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_IFR, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							osDelay(20);
							do{status = osPoolFree(IFR_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;			
			case MID_EXEC_DEVPWM:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			case MID_EXEC_DEVPWM_FAN:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;			
			case MID_EXEC_RELAYS:{
				
						RelaysCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_RelaysCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							
							osDelay(20);
							do{status = osPoolFree(RelaysCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			case MID_EXEC_RELAYSLIGHT:{
				
				
					/*�Զ��巢�����ݴ��������������������������*/
					
							updataTRAN.data_len = 1+16;					
							memcpy(sendbuff+16+updataTRAN.data_len,gb_databuff, 1);//17+updataTRAN.data_len-1
							

							
							
					}break;			
			case MID_EXEC_CURTAIN:{
				
						curtainCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_curtainCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							
							osDelay(20);
							do{status = osPoolFree(curtainCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;			
			case MID_EXEC_SOURCE :{//��Դģ�����ݷ���
				
						sourceCM_MEAS *rptr = NULL;
				
					
				
						evt = osMessageGet(MsgBox_sourceCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							osDelay(20);
							do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;			
			case MID_EXEC_SPEAK:{
			
						speakCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_speakCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
						
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(speakCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
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

void USARTWireless_Thread(const void *argument){
	
	osEvent  evt;
    osStatus status;
	
	
	
	const u8 frameDatatrans_totlen = 100;	//֡�����޳�
	const u8 dats_BUFtemp_len = frameDatatrans_totlen - 20;	//�������ݰ������޳�
	u8 dataTrans_TXBUF[frameDatatrans_totlen] = {0};  //����֡����
	u8 dataTrans_RXBUF[frameDatatrans_totlen] = {0};	//����֡����
	u8 TXdats_BUFtemp[dats_BUFtemp_len] = {0};	//���ͺ������ݰ�����
	u8 RXdats_BUFtemp[dats_BUFtemp_len] = {0};	//���պ������ݰ�����
	u8 memp = 1;
	TRAN_D_struct recv_tranbuff;
	u8 datacount = 0;
	u8 datacount_last = 0;
	
	osSignalWait(WIRLESS_THREAD_EN,osWaitForever);		//�ȴ�����ʹ���ź�
	osSignalClear(tid_USARTWireless_Thread,WIRLESS_THREAD_EN);
	
	switch(Moudle_GTA.Wirless_ID){		//��ʼ�����У��ڽ��̼���ǰ
	
		case MID_TRANS_Zigbee	:	; break;
			
		case MID_TRANS_Wifi		:	 USART2Wireless_wifiESP8266Init(); break;
			
		default:break;
	}
	
//	Moudle_GTA.Extension_ID = MID_EGUARD;   	/****�������*****/
//	Moudle_GTA.Wirless_ID = 0xAA;   			/****�������*****/
	if(memp)
		memp=0;
	for(;;){

		
		led2_status = zigbee_sw();
		
		if( online && gb_Exmod_key)//�����߳�
		{	
			gb_Exmod_key = false;
			zigbee_updata(Moudle_GTA.Extension_ID);
		}
		
		
/*************�������������������������������������������������������������������������ݽ��գ��������������������̡�������������������������������������������������������������������������*********************/
		if(RX_FLG){
			RX_FLG = false;


			//���ҶԱ�
			if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa)
			{
				uint8  crc=0xfe;
				memcpy(&recv_tranbuff,recvbuff+3,17);
				crc = TRAN_crc8(recvbuff,19+recv_tranbuff.data_len);//17+recv_tranbuff.data_len+2

				if(crc == recvbuff[19+recv_tranbuff.data_len])//17+recv_tranbuff.data_len+2
				{
					datacount = dataTrans_RXBUF[2];//��Ű����
					if(datacount != datacount_last)//����ͬ��
					{
						datacount_last = datacount;
						
					}

				}
			}
			
			switch(Moudle_GTA.Extension_ID){	//���ݽ���
			
				case MID_SENSOR_FIRE :{
				
						fireMS_MEAS *mptr = NULL;
						
						do{mptr = (fireMS_MEAS *)osPoolCAlloc(fireMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTfireMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_SENSOR_PYRO :{
				
						pyroMS_MEAS *mptr = NULL;
						
						do{mptr = (pyroMS_MEAS *)osPoolCAlloc(pyroMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTpyroMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_SENSOR_SMOKE :{
				
						smokeMS_MEAS *mptr = NULL;
						
						do{mptr = (smokeMS_MEAS *)osPoolCAlloc(smokeMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTsmokeMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_SENSOR_GAS  :{
				
						gasMS_MEAS *mptr = NULL;
						
						do{mptr = (gasMS_MEAS *)osPoolCAlloc(gasMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTgasMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_SENSOR_TEMP :{
				
						tempMS_MEAS *mptr = NULL;
						
						do{mptr = (tempMS_MEAS *)osPoolCAlloc(tempMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTtempMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;
				case MID_SENSOR_SHAKE :{
				
						shakeMS_MEAS *mptr = NULL;
						
						do{mptr = (shakeMS_MEAS *)osPoolCAlloc(shakeMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTshakeMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;
				case MID_SENSOR_LIGHT:{
				
						lightMS_MEAS *mptr = NULL;
						
						do{mptr = (lightMS_MEAS *)osPoolCAlloc(lightMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTlightMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_SENSOR_ANALOG:{
				
						analogMS_MEAS *mptr = NULL;
						
						do{mptr = (analogMS_MEAS *)osPoolCAlloc(analogMS_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						//�����У����·�����
						
						osMessagePut(MsgBox_MTanalogMS, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_EGUARD:{
					
						EGUARD_MEAS *mptr = NULL;
						u8 cmdDevType = RXdats_BUFtemp[0] & 0xf0;	//ͷ�ֽڸ���λ��������ָ���豸�����룬����λΪʵ��ָ��
						
						/*�Զ������ݴ��������������������������*/
						
						switch(cmdDevType){
						
							case 0x00:{		//ָ��
								
								do{mptr = (EGUARD_MEAS *)osPoolCAlloc(EGUD_pool);}while(mptr == NULL);
								mptr -> CMD = RXdats_BUFtemp[0];  //�����������
								mptr -> DAT = RXdats_BUFtemp[1];  //�������ݼ���
								osMessagePut(MsgBox_MTEGUD_FID, (uint32_t)mptr, osWaitForever);		//ָ��������ģ������
							}break;
								
							case 0x10:{		//����
								
								do{mptr = (EGUARD_MEAS *)osPoolCAlloc(EGUD_pool);}while(mptr == NULL);
								mptr -> CMD = RXdats_BUFtemp[0];  //�����������
								mptr -> DAT = RXdats_BUFtemp[1];  //�������ݼ���
								osMessagePut(MsgBox_MTEGUD_DLOCK, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
							}break;
							
							case 0x20:{		//RFID��������
							
							
							}break;
							
							case 0x30:{		//���̣�������
							
							
							}break;
								
							default:break;
						}
						
						osDelay(100);
					}break;				
				case MID_EXEC_DEVIFR:{
				
						IFR_MEAS *mptr = NULL;
						
						do{mptr = (IFR_MEAS *)osPoolCAlloc(IFR_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->Mod_addr = RXdats_BUFtemp[0];
						mptr->VAL_KEY  = RXdats_BUFtemp[1];
						
						osMessagePut(MsgBox_MTIFR, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_EXEC_DEVPWM:{
					
						pwmCM_MEAS *mptr = NULL;
						
						do{mptr = (pwmCM_MEAS *)osPoolCAlloc(pwmCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->Mod_addr 	= RXdats_BUFtemp[0];
						mptr->Switch 	= RXdats_BUFtemp[1];
						mptr->pwmVAL 	= RXdats_BUFtemp[2];
						
						osMessagePut(MsgBox_MTpwmCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;
				case MID_EXEC_DEVPWM_FAN:{
					
						pwmCM_MEAS *mptr = NULL;
						
						do{mptr = (pwmCM_MEAS *)osPoolCAlloc(pwmCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->Mod_addr 	= RXdats_BUFtemp[0];
						mptr->Switch 	= RXdats_BUFtemp[1];
						mptr->pwmVAL 	= RXdats_BUFtemp[2];
						
						osMessagePut(MsgBox_MTpwmCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_EXEC_RELAYS:{
					
						RelaysCM_MEAS *mptr = NULL;
						
						do{mptr = (RelaysCM_MEAS *)osPoolCAlloc(RelaysCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->relay_con = RXdats_BUFtemp[0];
						
						osMessagePut(MsgBox_MTRelaysCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;					
				case MID_EXEC_RELAYSLIGHT:{
					
						RelaysCM_MEAS *mptr = NULL;
						
						do{mptr = (RelaysCM_MEAS *)osPoolCAlloc(RelayslightCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->relay_con = recvbuff[16+recv_tranbuff.data_len];//17+1+recv_tranbuff.data_len
						
						osMessagePut(MsgBox_MTRelayslightCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;
				case MID_EXEC_RELAYSOUTLET:{
					
//						RelaysoutletCM_MEAS *mptr = NULL;
//						
//						do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);
//						/*�Զ������ݴ��������������������������*/
//						
//						mptr->relay_con = RXdats_BUFtemp[0];
//						
//						osMessagePut(MsgBox_MTRelaysoutletCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
//						osDelay(100);
					}break;				
				case MID_EXEC_CURTAIN:{
					
						curtainCM_MEAS *mptr = NULL;
						
						do{mptr = (curtainCM_MEAS *)osPoolCAlloc(curtainCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						
						mptr->valACT = RXdats_BUFtemp[0];
						
						osMessagePut(MsgBox_MTcurtainCM, (uint32_t)mptr, osWaitForever); //ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_EXEC_SOURCE :{//��Դ����ģ�鴮�ڽ���
				
						sourceCM_MEAS *mptr = NULL;
						
						do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->source_addr 	= RXdats_BUFtemp[0];
						mptr->Switch = RXdats_BUFtemp[1];

						osMessagePut(MsgBox_MTsourceCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;				
				case MID_EXEC_SPEAK:{
				
						speakCM_MEAS *mptr = NULL;

						do{mptr = (speakCM_MEAS *)osPoolCAlloc(speakCM_pool);}while(mptr == NULL);
						/*�Զ������ݴ��������������������������*/
						
						mptr->spk_num = RXdats_BUFtemp[0];
						
						osMessagePut(MsgBox_MTspeakCM, (uint32_t)mptr, osWaitForever);	//ָ��������ģ������
						osDelay(100);
					}break;
				
				default:break;
			}
		}
		
/****************�����������������������������������������������ݽ��գ�������������������ݴ����������������������������������������*************************/
		memset(TXdats_BUFtemp, 0, sizeof(u8) * dats_BUFtemp_len);
		if(0)
		switch(Moudle_GTA.Extension_ID){	//���ݷ���
		
			case MID_SENSOR_FIRE :{
				
						fireMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_fireMS, 100);		
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(fireMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}	
					}break;
			
			case MID_SENSOR_PYRO :{
				
						pyroMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pyroMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
				
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pyroMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_SMOKE :{
				
						smokeMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_smokeMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							//ģ��ͨ��ֵ��ʱ���������ϴ�
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
														
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(smokeMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_GAS:{
				
						gasMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_gasMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							//ģ��ͨ��ֵ��ʱ���������ϴ�
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(gasMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_TEMP:{
				
						tempMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_tempMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							if(rptr->temp >= 0.0)TXdats_BUFtemp[0] = 1;
							else TXdats_BUFtemp[0] = 2; 
							
							TXdats_BUFtemp[1] = (char)rptr->temp;
							TXdats_BUFtemp[2] = (char)((rptr->temp - (float)TXdats_BUFtemp[1])*100);
							
							TXdats_BUFtemp[3] = (char)rptr->hum;
							TXdats_BUFtemp[4] = (char)((rptr->hum - (float)TXdats_BUFtemp[3])*100);
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,5);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(tempMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			case MID_SENSOR_SHAKE :{
				
						shakeMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_shakeMS, 100);		
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							switch(rptr->VAL){
							
								case true:	TXdats_BUFtemp[0] = 0x02;	break;
									
								case false:	TXdats_BUFtemp[0] = 0x01;	break;
								
								default:	TXdats_BUFtemp[0] = ABNORMAL_DAT;	break;
							}
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(shakeMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}	
					}break;
			case MID_SENSOR_LIGHT:{
				
						lightMS_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_lightMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = (char)(rptr->illumination);
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);
							
							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(lightMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_SENSOR_ANALOG:{
				
						analogMS_MEAS *rptr = NULL;
				
						float CUR = 0.0;
						float VOL = 0.0;
				
						evt = osMessageGet(MsgBox_analogMS, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							VOL = 420.0 / 4096.0 * (float)rptr->Ich1;		//��ѹ
							CUR = 6.1 / 4096.0 * (float)rptr->Ich2;			//����
							
							TXdats_BUFtemp[0] = (uint8_t)((uint32_t)VOL % 10000 / 100);
							TXdats_BUFtemp[1] = (uint8_t)((uint32_t)VOL % 100);
							TXdats_BUFtemp[2] = (uint8_t)((uint32_t)((VOL - TXdats_BUFtemp[1]) * 100.0)) % 100;
							
							TXdats_BUFtemp[3] = (uint8_t)((uint32_t)CUR % 100);
							TXdats_BUFtemp[4] = (uint8_t)((uint32_t)((CUR - TXdats_BUFtemp[3]) * 100.0)) % 100;
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,5);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(analogMS_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
					
			case MID_EGUARD:{
				
						EGUARD_MEAS *rptr = NULL;
						evt = osMessageGet(MsgBox_EGUD, 100);
				
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
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
							do{status = osPoolFree(EGUD_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}						
					}break;			
			
			case MID_EXEC_DEVIFR:{
			
						IFR_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_IFR, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->VAL_KEY;
							TXdats_BUFtemp[2] = rptr->STATUS;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,2);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(IFR_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_DEVPWM:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->Switch;
							TXdats_BUFtemp[2] = rptr->pwmVAL;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,3);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			case MID_EXEC_DEVPWM_FAN:{
			
						pwmCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_pwmCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->Mod_addr;
							TXdats_BUFtemp[1] = rptr->Switch;
							TXdats_BUFtemp[2] = rptr->pwmVAL;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,3);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(pwmCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_RELAYS:{
				
						RelaysCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_RelaysCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->relay_con;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(RelaysCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			case MID_EXEC_RELAYSLIGHT:{
				
						RelaysCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_RelayslightCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->relay_con;	
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(RelayslightCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_CURTAIN:{
				
						curtainCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_curtainCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->valACT;
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(curtainCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_SOURCE :{//��Դģ�����ݷ���
				
						sourceCM_MEAS *rptr = NULL;
				
						float CUR;
				
						evt = osMessageGet(MsgBox_sourceCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
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
							do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			case MID_EXEC_SPEAK:{
			
						speakCM_MEAS *rptr = NULL;
				
						evt = osMessageGet(MsgBox_speakCM, 100);
						if (evt.status == osEventMessage) {		//�ȴ���Ϣָ��
							
							rptr = evt.value.p;
							/*�Զ��巢�����ݴ��������������������������*/
							
							TXdats_BUFtemp[0] = rptr->spk_num;
							
							memp = dataTransFrameLoad_TX(dataTrans_TXBUF,datsTransCMD_UPLOAD,Moudle_GTA.Extension_ID,TXdats_BUFtemp,1);

							//Driver_USART2.Send(dataTrans_TXBUF,memp);
							osDelay(20);
							do{status = osPoolFree(speakCM_pool, rptr);}while(status != osOK);	//�ڴ��ͷ�
							rptr = NULL;
						}
					}break;
			
			default:break;
		}
		memset(dataTrans_TXBUF, 0, sizeof(u8) * frameDatatrans_totlen);
		delay_ms(10);
	}
}

void USART_WirelessInit(void){

	USART2Wirless_Init();
	
}

void wirelessThread_Active(void){
	
	USART_WirelessInit();
	tid_USARTWireless_Thread = osThreadCreate(osThread(USARTWireless_Thread),NULL);
	tid_UsartRx_Thread = osThreadCreate(osThread(UsartRx_Thread),NULL);
}
