#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "global.h"

/*�ڲ�����*/
static uint64_t key1Count=0, key2Count=0;
 /*********/
/***************************************************************************************************************
*��������	Key_Init
*������		������ʼ��		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	��
*��д���ڣ�	2019.3.29 13:18
*�汾��¼��	
****************************************************************************************************************/
void Key_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStruct;

 	RCC_APB2PeriphClockCmd(KEY_1_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = KEY_1_PIN|KEY_2_PIN;
	GPIO_Init(KEY_1_PORT, &GPIO_InitStruct);

}
void dly_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	

	PAout(15) = 0;
}

/***************************************************************************************************************
*��������		Key1Scan
*������		����1ɨ��		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	bool ����ֵ
*��д���ڣ�	2019.3.29 13:18
*�汾��¼��	
****************************************************************************************************************/
bool Key1Scan(void)
{
	static u8 key1_i=0;                  								//��̬�ֲ����������ڼ���	
	bool Key1Value = FALSE;
	
	if(KEY_1_VALUE() == KEY_VALUE_FALSE)
	{
		Key1Flag = FALSE;
	}
	if(key1_i >= 10)													//����ȷʵ����
	{
		key1_i = 0;
		Key1Flag = TRUE;		    									//ȷ�ϰ����¼���Ч
	}
	else 																//��������
	{
		if(SystemCnt - key1Count >= 10)									//10ms��
		{
			if(KEY_1_VALUE() == KEY_VALUE_TRUE)     					//ȷ�ϼ�ֵ����Ч״̬
			{
				key1_i++;
			}
			else
			{
				key1_i=0;
			}
			key1Count = SystemCnt;										//�����ʱ��
		}	
	}
	Key1Value = Key1Flag;
	
	return Key1Value;
}

/***************************************************************************************************************
*��������		Key2Scan
*������		����2ɨ��		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	bool ����ֵ
*��д���ڣ�	2019.3.29 13:29
*�汾��¼��	
****************************************************************************************************************/
bool Key2Scan(void)
{
	static u8 key2_i=0;                  								//��̬�ֲ����������ڼ���	
	bool Key2Value = FALSE;

	if(KEY_2_VALUE() == KEY_VALUE_FALSE)
	{
		Key2Flag = FALSE;
	}
	if(key2_i >= 10)													//����ȷʵ����
	{
		Key2Flag = TRUE;		    									//ȷ�ϰ����¼���Ч
		key2_i = 0;
	}
	else 																//��������
	{
		if(SystemCnt - key2Count >= 10)									//10ms��
		{
			if(KEY_2_VALUE() == KEY_VALUE_TRUE)     					//ȷ�ϼ�ֵ����Ч״̬
			{
				key2_i++;
			}
			else
			{
				key2_i=0;
			}
			key2Count = SystemCnt;										//�����ʱ��
		}	
	}
	Key2Value = Key2Flag;
	
	return Key2Value;
}

/***************************************************************************************************************
*��������	Beep_Init
*������		������ʼ��		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	��
*��д���ڣ�	2019.3.29 13:50
*�汾��¼��	
****************************************************************************************************************/
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(BEEP_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = BEEP_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(BEEP_PORT ,&GPIO_InitStruct);

	BEEP_VALUE() = 0;
}

/***************************************************************************************************************
*��������	BeepOn
*������		����������	
*���ߣ�		hongxiang.liu
*������		u16 Cnt ����ʱ��(ms)
*����ֵ��	��
*��д���ڣ�	2019.3.29 13:55
*�汾��¼��	
****************************************************************************************************************/
void BeepOn(u16 Cnt)
{
	BeepCnt = SystemCnt + Cnt;
	BEEP_VALUE() = 1;
}

/***************************************************************************************************************
*��������		CodeSwitch_Init
*������		8472���뿪�س�ʼ��		
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��ע��		����(�͵�ƽ��Ч)
*������ڣ�	2019.3.20
*�汾��¼��	
****************************************************************************************************************/
void CodeSwtich_Init(void)
{
	RCC_APB2PeriphClockCmd(TYPE_1_PERIPH, ENABLE);
	//ʱ��ʹ��

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = TYPE_1_PIN|TYPE_2_PIN|TYPE_4_PIN|TYPE_8_PIN;
	GPIO_Init(TYPE_1_PORT, &GPIO_InitStruct);
	//��ʼ�����뿪��4��IO������������
}

/***************************************************************************************************************
*��������		GetSwitchType
*������		��ȡ8472���뿪�صļ�ֵ
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		u8 �����ֵ
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
u8 GetSwitchType(void)
{
	u8 value = 0;
	
	value = (!TYPE_8_VALUE)<<3|(!TYPE_4_VALUE)<<2|(!TYPE_2_VALUE)<<1|(!TYPE_1_VALUE);

	return value;
}

/***************************************************************************************************************
*��������	Fan_Init
*������		PWM���ٷ�������
*���ߣ�		hongxiang.liu
*������		u16 speed ת��(0~3800)
*����ֵ��		��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
void Fan_Init(u16 speed)
{
	if(speed>3800) speed=3800;		//��ֵ�ж�
	u16 TIM3Cnt = (u16)speed*5/19;
	//speed/3800 * 1000
	
	GPIO_InitTypeDef GPIO_InitStruct;

	/*1.IO������*/
	RCC_APB2PeriphClockCmd(FAN_PWM_PERIPH, ENABLE);
	//ʱ��ʹ��
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = FAN_PWM_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(FAN_PWM_PORT, &GPIO_InitStruct);
	//��ʼ�����뿪��4��IO������������

	/*2.TIM3����*/	
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//TIM3ʱ��ʹ��
	
	TIM_DeInit(TIM3);//����Ϊȱʡֵ
	
//	//��ȡAPB1ʱ��Ƶ��
//	RCC_ClocksTypeDef RCC_Clocks;
//	RCC_GetClocksFreq(&RCC_Clocks);
	
	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//Ԥ��Ƶֵ	72��Ƶ��Ƶ��Ϊ1MHz
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_BaseInitStruct.TIM_Period = 1000-1;//�Զ���װ��ֵ ���ʱ��Ϊ 1ms(1KHz) 
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�Ƶ���� = 1;����Ƶ
	TIM_TimeBaseInit(TIM3,&TIM_BaseInitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);//��Ϊȱʡֵ
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;//PWM1�Ƚ����ģʽ
	TIM_OCInitStruct.TIM_Pulse = TIM3Cnt;//ռ�ձ� 
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽΪ�ߵ�ƽ
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//���״̬ʹ��
	TIM_OC2Init(TIM3,&TIM_OCInitStruct);//��ʼ��TIM5��ͨ��2
	TIM_ARRPreloadConfig(TIM3,ENABLE);//ʹ��TIM5ARR�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��TIM5��CCR2�ϵ�Ԥװ�ؼĴ���
	
	/*3�������ж�*/
	//�жϷ���
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;//�ж���
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//����жϱ�־λ
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
	//TIM1�ж�����
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
	//��������жϺͱȽ��ж�
	
	/*4��������ʱ��*/
	TIM_Cmd(TIM3,ENABLE);//ʹ��TIM3
//	TIM_CtrlPWMOutputs(TIM3,ENABLE);//TIM3�����(PWM)ʹ��
}

/***************************************************************************************************************
*��������	FanPowerOn
*������		��PWM���ٷ���
*���ߣ�		hongxiang.liu
*������		u16 speed ת��(0~3800) ���� ��Ϊ0���򲻸ı䶨ʱ���Ƚ�ֵ
*����ֵ��	��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
void FanPowerOn(u16 speed)
{
	if(speed!=0)			//����β�Ϊ0���򲻸ı�ת��
	{
		//if(speed>3800) speed=3800;		//��ֵ�ж�
		u16 TIM3Cnt = (u16)speed;//*5/19
		//speed/3800 * 1000
		TIM3Cnt = TIM3Cnt < 999 ? TIM3Cnt : 999;
		TIM_SetCompare2(TIM3, TIM3Cnt);
	}
	PwmPowerFlag = TRUE;
	//���ȴ򿪱�־��λ
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
	TIM_Cmd(TIM3,ENABLE);//ʹ��TIM3
//	TIM_CtrlPWMOutputs(TIM3,ENABLE);//TIM3�����(PWM)ʹ��
}

/***************************************************************************************************************
*��������	ChangeFanSpeed
*������		PWM���ٷ���ת��
*���ߣ�		hongxiang.liu
*������		u16 speed ת��(0~3800)
*����ֵ��	��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
void ChangeFanSpeed(u16 speed)
{
	//if(speed>3800) speed=3800;		//��ֵ�ж�
	u16 TIM3Cnt = (u16)speed;//*5/19
	//speed/3800 * 1000
	TIM3Cnt = TIM3Cnt < 999 ? TIM3Cnt : 999;
	TIM_SetCompare2(TIM3, TIM3Cnt);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
}

/***************************************************************************************************************
*��������	FanPowerOff
*������		�ر�PWM���ٷ���
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
void FanPowerOff(void)
{
	PwmPowerFlag = FALSE;
	//���ȴ򿪱�־��0
	
	TIM_Cmd(TIM3,DISABLE);//ʧ��TIM3
//	TIM_CtrlPWMOutputs(TIM3,DISABLE);//TIM3�����(PWM)ʹ��

	FAN_PWM_OFF //ע��˴�Ϊ���������
}

