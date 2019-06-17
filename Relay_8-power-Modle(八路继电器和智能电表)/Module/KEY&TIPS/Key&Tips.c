#include <Key&Tips.h>//�װ尴�������̺������䰴���ص�������
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern ARM_DRIVER_USART Driver_USART1;								//�豸�����⴮��һ�豸����

osThreadId tid_keyMboard_Thread;										//�������������ID
osThreadDef(keyMboard_Thread,osPriorityAboveNormal,1,2048);	//������������̶���

typedef void (* funkeyThread)(funKeyInit key_Init,Obj_keyStatus *orgKeyStatus,funKeyScan key_Scan,Obj_eventKey keyEvent,const char *Tips_head);

/***���������ʼ��***/
void keyInit(void){	

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);		//ʱ�ӷ���
	
	

	//���� KEY 1~8����
	GPIO_InitStructure.GPIO_Pin = 
			GPIO_Pin_0|
			GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
			GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
			GPIO_Pin_7; //�˿����Է���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

	//�̵��� 1 ����	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;	//�˿����Է���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	//LED 7��8���� �̵��� 6~8 ����	
	GPIO_InitStructure.GPIO_Pin = 
			GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_13|
			GPIO_Pin_14|GPIO_Pin_15;	//�˿����Է���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

	//LED 1~6���� �̵��� 2~5 ����
	GPIO_InitStructure.GPIO_Pin = 
		GPIO_Pin_0|
		GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
		GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
		GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	//�˿����Է���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	LED1 = LED2 = LED3=LED4=LED5=LED6=LED7=LED8 = 1;
	Relay1 = Relay2 = Relay3=Relay4=Relay5=Relay6=Relay7=Relay8 = 0;

	
	GPIO_InitStructure.GPIO_Pin = 
		GPIO_Pin_4 ; 				//�װ�ָʾ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//����������50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	PBout(4) = 0;
	 
	
}

/***������ֵ��ȡ***/
 uint16_t keyScan(void){

	if(!K1)return KEY_VALUE_1;		//��ֵ1
	if(!K2)return KEY_VALUE_2;		//��ֵ2
	return KEY_NULL;				//�ް���
}

/***�������״̬��***/
static uint16_t getKey(Obj_keyStatus *orgKeyStatus,funKeyScan keyScan){

	static	uint16_t s_u16KeyState 		= KEY_STATE_INIT;		//״̬�����״̬����ʼ��״̬
	static	uint16_t	s_u16LastKey		= KEY_NULL;			//������ʷ������ֵ	
	static	uint8_t	KeyKeepComfirm		= 0;					//������ȷ�ϱ��� ȷ������ʱ����ʱ
	static	uint16_t	s_u16KeyTimeCount	= 0;				//����ʱ�����壨������KEY_TICK���м����������������ֵ��ȷ���Ƿ����ڳ�����
				uint16_t keyTemp				= KEY_NULL;		/*ʮ�����Ƶ�һλ������״̬��	�ڶ�λ�����ּ���ֵ��		����λ����ֵ��		����λ����������ֵ*/
	
	static	uint32_t osTick_last			= 0xffff0000;		//��osTick���м�¼����������һ��osTick���жԱȻ�ȡ������˼�������жϰ����Ƿ������������£�
	
	keyTemp = keyScan();		//��ȡ��ֵ
	
	switch(s_u16KeyState){	//��ȡ״̬��״̬
	
		case KEY_STATE_INIT:	//��ʼ��״̬
			
				if(orgKeyStatus->keyCTflg){	//�����һ���Ƿ�Ϊ����
				
					if((osKernelSysTick() - osTick_last) > KEY_CONTINUE_PERIOD){	//��һ�����������Ȿ���Ƿ��������
					
						keyTemp	= s_u16LastKey & 0x00f0;	//���β���������������ֵΪ��������״̬��ͬʱ������־λ����
						keyTemp |= KEY_CTOVER;
						orgKeyStatus->keyCTflg = 0;	
					}
				}
		
				if(KEY_NULL != keyTemp)s_u16KeyState = KEY_STATE_WOBBLE;	//��⵽�а������л�״̬���������
				break;
		
		case KEY_STATE_WOBBLE:	//����״̬���
			
				s_u16KeyState = KEY_STATE_PRESS;	//ȷ�ϰ������л�״̬���̰����
				break;
		
		case KEY_STATE_PRESS:	//�̰�״̬���
		
				if(KEY_NULL != keyTemp){		//�����Ƿ���
				
					s_u16LastKey 	= keyTemp;	//�洢������ֵ
					keyTemp 		  |= KEY_DOWN;	//����״̬ȷ��Ϊ����
					s_u16KeyState	= KEY_STATE_LONG;	//������Ȼδ�����л�״̬���������
				}else{
				
					s_u16KeyState	= KEY_STATE_INIT;	//���Ϊ�������������״̬�����س�ʼ��״̬
				}
				break;
				
		case KEY_STATE_LONG:		//����״̬���
				
				if(KEY_NULL != keyTemp){	//�����Ƿ���
					
					if(++s_u16KeyTimeCount > KEY_LONG_PERIOD){	//��������Ȼδ��������ݳ���ʱ�����м�������ȷ���Ƿ�Ϊ����
					
						s_u16KeyTimeCount	= 0;			//ȷ�ϳ�������������ֵ����
						orgKeyStatus->sKeyKeep				= 0;			//���״̬���л�״̬ǰ���Ա���ȷ��״̬�������ֵ��ǰ���㣬������ֵΪ�����󱣳ּ�������
						KeyKeepComfirm		= 0;			//���״̬���л�״̬ǰ���Ա���ȷ��״̬�������ֵ��ǰ���㣬����ֵ���ڶ��峤�����ý��б��ּ���
						keyTemp			  |= KEY_LONG;	//����״̬ȷ��Ϊ����
						s_u16KeyState		= KEY_STATE_KEEP;	//������Ȼδ�����л�״̬���������ּ��
						
						orgKeyStatus->keyOverFlg			= KEY_OVER_LONG;	//����
					}else	orgKeyStatus->keyOverFlg	= KEY_OVER_SHORT;		//�̰�
				}else{
				
					s_u16KeyState	= KEY_STATE_RELEASE;	//���ȷ��Ϊ�����󰴼������л�״̬��������
				}
				break;
				
		case KEY_STATE_KEEP:		//�����󱣳�״̬���
			
				if(KEY_NULL != keyTemp){		//�����Ƿ���
				
					if(++s_u16KeyTimeCount > KEY_KEEP_PERIOD){	//��������Ȼδ��������ݳ���ʱ�����м�������ȷ���Ƿ�Ϊ�������������
						
						s_u16KeyTimeCount	= 0;			//ȷ�ϳ�����������֣���������ֵ����
						if(KeyKeepComfirm < (KEY_COMFIRM + 3))KeyKeepComfirm++;			//����Ƿ��������ʱ��
						if(orgKeyStatus->sKeyKeep < 15 && KeyKeepComfirm > KEY_COMFIRM)orgKeyStatus->sKeyKeep++; 	//��⵽���������ִ�г����󱣳ּ���
						if(orgKeyStatus->sKeyKeep){		//��⵽�����󱣳ּ���ֵ��Ϊ�㣬��ȷ�ϰ���״̬Ϊ������������֣��Է���ֵ������Ӧȷ�ϴ���
						
							orgKeyStatus->keyOverFlg	 = KEY_OVER_KEEP;	//״̬ȷ��Ϊ������Ϊ����
							keyTemp	|= orgKeyStatus->sKeyKeep << 8;		//���ּ�����������8λ����ʮ������keyTemp�ڶ�λ
							keyTemp	|= KEY_KEEP;			//����״̬ȷ��Ϊ�������������
						}		
					}
				}else{
				
					s_u16KeyState	= KEY_STATE_RELEASE;	//����״̬ȷ��Ϊ�������������֮�����л�״̬��������
				}
				break;
				
		case KEY_STATE_RELEASE:	//����״̬���
				
				s_u16LastKey |= KEY_UP;	//�洢����״̬
				keyTemp		  = s_u16LastKey;	//����״̬ȷ��Ϊ����
				s_u16KeyState = KEY_STATE_RECORD;	//�л�״̬������������¼
				break;
		
		case KEY_STATE_RECORD:	//����������¼״̬���

				if((osKernelSysTick() - osTick_last) < KEY_CONTINUE_PERIOD){	//�����ΰ�������ʱ����С�ڹ涨ֵ�����ж�Ϊ����
					
					orgKeyStatus->sKeyCount++;	//��������		
				}else{
					
					orgKeyStatus->sKeyCount = 0;	//�����Ͽ�����������
				}
				
				if(orgKeyStatus->sKeyCount){		//������������Ϊ�㣬��ȷ��Ϊ�����������Է���ֵ������Ӧ����
													
					orgKeyStatus->keyCTflg	= 1;	//��������־
					keyTemp	= s_u16LastKey & 0x00f0;	//��ȡ��ֵ
					keyTemp	|=	KEY_CONTINUE;				//ȷ��Ϊ��������
					if(orgKeyStatus->sKeyCount < 15)keyTemp += orgKeyStatus->sKeyCount;	//�����������ݷ���ʮ������keyTemp����λ�����λ��
				}
				
				s_u16KeyState	= KEY_STATE_INIT;		//���״̬�����س�ʼ״̬
				osTick_last	 	= osKernelSysTick();	//��¼osTick�������´��������Ա�
				break;
		
		default:break;
	}
	return keyTemp;	//���ذ���״̬�ͼ�ֵ
}

/*������ʼ������������״̬����ṹ�壬����ɨ�躯�������������¼�������������ʾ��Ϣͷ*/
void key_Thread(	funKeyInit 		key_Init,		
					Obj_keyStatus 	*orgKeyStatus,
					funKeyScan 		key_Scan,		
					Obj_eventKey 	keyEvent,		
					const char 		*Tips_head	){
	
/***�������ԣ�����1����������Ϣ��****/
	static uint16_t keyVal;						//����״̬�¼�
	static uint8_t	key_temp;					//������ֵ����
	static uint8_t	kCount;						//��������ֵ�������������ּ�������������ʹ��ͬһ����������Ϊ����״̬����ͬʱ����
	static uint8_t	kCount_rec;			//��ʷ����ֵ����
	
	static osThreadId ID_Temp;					//��ǰ����ID����
	static osEvent evt;
	static uint8_t KEY_DEBUG_FLG = 0;

	const	 uint8_t	tipsLen = 80;		//Tips��ӡ�ַ�������
	static char	key_tempDisp;
	static char	kCountDisp;
	static char	kCount_recDisp;
	static char	tips[tipsLen];					//Tips�ַ���
	
	key_Init();

	for(;;){
		
		keyVal = getKey(orgKeyStatus,key_Scan);    //��ȡ��ֵ
		
		ID_Temp = osThreadGetId();
		evt = osSignalWait (KEY_DEBUG_OFF, 1);		 //��ȡDebug_log���Ȩ���ź�
		if (evt.value.signals == KEY_DEBUG_OFF){
		
			KEY_DEBUG_FLG = 0;
			osSignalClear(ID_Temp ,KEY_DEBUG_OFF);
		}else{
		
			evt = osSignalWait (KEY_DEBUG_ON, 1);
			if (evt.value.signals == KEY_DEBUG_ON){
			
				KEY_DEBUG_FLG = 1;
				osSignalClear(ID_Temp ,KEY_DEBUG_ON);
			}		
		}  
		
if(KEY_DEBUG_FLG){
	
		memset(tips,0,tipsLen*sizeof(char));	//ÿ��Tips��ӡ�����
		strcat(tips,"Tips:");						//Tips��ʶ
		strcat(tips,Tips_head);
		strcat(tips,"-");
}
/*------------------------------------------------------------------------------------------------------------------------------*/		
		switch(keyVal & 0xf000){
		
			case KEY_LONG		:	
				
					key_temp = (uint8_t)((keyVal & 0x00f0) >> 4);
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"����\r\n");	
					Driver_USART1.Send(tips,strlen(tips));	
}/***/
					break;
					
			case KEY_KEEP		:
				
					kCount		= (uint8_t)((keyVal & 0x0f00) >> 8);  //��ȡ����ֵ
					kCount_rec	= kCount;
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/					
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"�������֣����ּ�����");
					kCountDisp = kCount + '0';
					strcat(tips,(const char*)&kCountDisp);	
					strcat(tips,"\r\n");	
					Driver_USART1.Send(tips,strlen(tips));	
}/***/		
					break;
					
			case KEY_DOWN		:
				
					key_temp = (uint8_t)((keyVal & 0x00f0) >> 4);
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/				
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
					strcat(tips,"����\r\n");	
					Driver_USART1.Send(tips,strlen(tips));
}/***/			
					break;
					
			case KEY_UP			:
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/			
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
}/***/						
					switch(orgKeyStatus->keyOverFlg){

							case KEY_OVER_SHORT		:	
								
								   if(keyEvent.funKeySHORT[key_temp])keyEvent.funKeySHORT[key_temp]();		//�����¼��������ȼ�ⴥ���¼��Ƿ񴴽���û�����򲻽��д���
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/								
									strcat(tips,"�̰�����\r\n");	
									Driver_USART1.Send(tips,strlen(tips));
									orgKeyStatus->keyOverFlg = 0;
}/***/							
									break;

							case KEY_OVER_LONG		:
								
									if(keyEvent.funKeyLONG[key_temp])keyEvent.funKeyLONG[key_temp]();
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/								
									strcat(tips,"��������\r\n");	
									Driver_USART1.Send(tips,strlen(tips));
									orgKeyStatus->keyOverFlg = 0;
}/***/							
									break;

							case KEY_OVER_KEEP		:	
								
									if(keyEvent.funKeyKEEP[key_temp][kCount_rec])keyEvent.funKeyKEEP[key_temp][kCount_rec]();
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/									
									strcat(tips,"�����󱣳�");
									kCount_recDisp = kCount_rec + '0';
									strcat(tips,(const char*)&kCount_recDisp);
									strcat(tips,"�μ��������\r\n");
									Driver_USART1.Send(tips,strlen(tips));
									kCount_rec = 0;
}/***/							
									break;			
							default:break;
						}
						break;
					
			case KEY_CONTINUE	:
				
					kCount 		= (uint8_t)((keyVal & 0x000f) >> 0);	//��ȡ����ֵ
					kCount_rec	= kCount + 1;
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/	
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
					strcat(tips,"����������������");	
					kCountDisp = kCount + '0';
					strcat(tips,(const char*)&kCountDisp);	
					strcat(tips,"\r\n");	
					Driver_USART1.Send(tips,strlen(tips));		
}/***/			
					break;
					
			case KEY_CTOVER	:
				
					if(keyEvent.funKeyCONTINUE[key_temp][kCount_rec])keyEvent.funKeyCONTINUE[key_temp][kCount_rec]();
if(KEY_DEBUG_FLG){/*Debug_log���ʹ��*/					
					strcat(tips,"����");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"����");
					kCount_recDisp = kCount_rec + '0';
					strcat(tips,(const char*)&kCount_recDisp);
					strcat(tips,"�κ����\r\n");
					Driver_USART1.Send(tips,strlen(tips));
}/***/	
					kCount_rec = 0;				
					break;
					
			default:break;
		}
		osDelay(KEY_TICK);
	}
}

void eventK23(void){
	
	;
}

void eventK24(void){

	;
}

void eventK25(void){

	switch(Moudle_GTA.Extension_ID){
	
		case MID_EXEC_DEVPWM:{
		
		}break;
		
		default:break;
	}
}
u8 key_scan(void)
{
	u16 i = 0;
	u8 buff=0;
		
		if(KEY1 == 0)
		{
			delay_ms(10);
			while(KEY1 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			
			if(i > 1 && i < 20)//���ΰ���
			{

				(LED1 = !LED1);
				Relay1 = !LED1;
				

			}
			else if(i > 145 && KEY2 == 0)//���� 3S
			{
			
					led2_status = led2_b_flash;
					osSignalSet(tid_USARTWireless_Thread, 0x03);
			}
		}
		else if(KEY2 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY2 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 10)//���ΰ���
			{

				LED2 = !LED2;
				 Relay2 = !Relay2;

			}
			else if(i > 145 && KEY1 == 0)//���� 3S
			{
			
					led2_status = led2_b_flash;
					osSignalSet(tid_USARTWireless_Thread, 0x03);
			}
		}
		
		else if(KEY3 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY3 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//���ΰ���
			{

				LED3 = !LED3;
				 Relay3 = !Relay3;

			}
			
		}
		
		else if(KEY4 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY4 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//���ΰ���
			{

				LED4 = !LED4;
				 Relay4 = !Relay4;

			}
			
		}
		else if(KEY5 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY5 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//���ΰ���
			{

				LED5 = !LED5;
				 Relay5 = !Relay5;

			}
			
		}
		else if(KEY6 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY6 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//���ΰ���
			{

				LED6 = !LED6;
				Relay6 = !LED6;

			}
			
		}
		else if(KEY7 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY7 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//���ΰ���
			{

				LED7 = !LED7;
				Relay7 = !LED7;

			}
			
		}
		else if(KEY8 == 0)
		{
				i = 0;
				delay_ms(10);
				while(KEY8 == 0 && i < 150 )
				{
					delay_ms(20);
					i++;
					}
				if(i > 1 && i < 80)//���ΰ���
				{

					LED8 = !LED8;
					Relay8 = !LED8;
				}
			
		}
		buff =  Relay1;
		buff = (buff<<1)+Relay2;
		buff = (buff<<1)+Relay3;
 		buff = (buff<<1)+Relay4;
 		buff = (buff<<1)+Relay5;
 		buff = (buff<<1)+Relay6;
 		buff = (buff<<1)+Relay7;
 		buff = (buff<<1)+Relay8;
		
	 return buff;
}
/***�������������***/
void keyMboard_Thread(const void *argument){
	u8 relay_status=0;
	u8 relay_status_pre=0xff;
	unsigned int time_out_count = 0xffff;
	//��ʱʹ�ð�������
	while(1)
	{
		relay_status = key_scan();
		gb_databuff[0] = relay_status;

		
		if(relay_status_pre == relay_status)
		{
			
			;
		}
		else
		{
			gb_Exmod_key = true;
			
			osSignalSet(tid_USARTWireless_Thread, 0x03);
			relay_status_pre = relay_status;
		}

		if(!time_out_count--)
		{
			time_out_count = 0xffff;
			gb_Exmod_key = true;
			
			osSignalSet(tid_USARTWireless_Thread, 0x03);
		}

		delay_ms(20);
	}

	

}
	
void keyMboardActive(void){
	keyInit();
	tid_keyMboard_Thread = osThreadCreate(osThread(keyMboard_Thread),NULL);
}



