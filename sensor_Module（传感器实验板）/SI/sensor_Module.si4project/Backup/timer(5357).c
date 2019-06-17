#include "timer.h"
#include "led.h"
#include "key.h"
#include "usart_cfg.h"
#include "stm32f10x_tim.h"
 
 
 
 
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
  	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}


 
void  ENTER_critical_offtim2(void)
{ 

	TIM_Cmd(TIM2, DISABLE); 

}

void  EXTI_critical_offtim2(void)
{ 

	TIM_Cmd(TIM2, ENABLE); 

}



//��ʱ��3�жϷ������
 
void TIM2_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
 
		RS485_V_I_timeout_fution();  //1ms  ONCE 
		Rs485_COMM_uart_timeout_fution();

		alarm_time++;
		SystemCnt++;									//ϵͳ��ʱ++
		if(BeepCnt>=SystemCnt)
		{
			if(SystemCnt%2==0)	BEEP_VALUE() = 1;
			else				BEEP_VALUE() = 0;
		}

		if(SystemCnt%2000==0)	TimeOutFlag=TRUE;		//2s����
		 (send_urt_time==0)?(send_urt_time=send_urt_time):(send_urt_time--);
	}
}

/***************************************************************************************************************
*��������		TIM6_Init
*������		TIM6��ʼ����������ϵͳʱ���ʱ_1ms
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
//void TIM6_Init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;	
//	NVIC_InitTypeDef NVIC_InitStruct;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
//	
//	//��ʱ��TIM6��ʼ��
//	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//Ԥ��Ƶֵ	72��Ƶ��Ƶ��Ϊ1MHz
//	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
//	TIM_BaseInitStruct.TIM_Period = 1000-1;//�Զ���װ��ֵ ���ʱ��Ϊ 1ms(1KHz) 
//	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�Ƶ���� = 1;����Ƶ
//	TIM_TimeBaseInit(TIM6,&TIM_BaseInitStruct);

//	//�жϷ���
//	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;//�ж���
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	
//	//����жϱ�־λ
//	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
//	//TIM1�ж�����
//	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
//	//��������ж�
//	
//	/*4��������ʱ��*/
//	TIM_Cmd(TIM6,ENABLE);//ʹ��TIM6
//}

/***************************************************************************************************************
*��������		TIM1_Init
*������		TIM1��ʼ����������HC_SR04���ģ���DATA���ţ����벶����ͬʱ����HC_SR04Gpio_Init()����
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.22
*�汾��¼��	
****************************************************************************************************************/
void TIM1_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//TIM1ʱ��ʹ��
	
	TIM_DeInit(TIM1);//����Ϊȱʡֵ
		
	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//Ԥ��Ƶֵ	72��Ƶ��Ƶ��Ϊ1MHz
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_BaseInitStruct.TIM_Period = 1000-1;//�Զ���װ��ֵ ���ʱ��Ϊ 1ms(1KHz) 
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�Ƶ���� = 1;����Ƶ
	TIM_TimeBaseInit(TIM1,&TIM_BaseInitStruct);

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;//CH4
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;//�����ز���
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//��ӦIC4
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;//ÿ������ִ��һ��
	TIM_ICInitStruct.TIM_ICFilter = 0x00;//���˲�
	TIM_ICInit(TIM1, &TIM_ICInitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;//�ж���
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;//�ж���
	NVIC_Init(&NVIC_InitStruct);
	
	//����жϱ�־λ
	TIM_ClearFlag(TIM1,TIM_IT_CC4);
	TIM_ClearFlag(TIM1,TIM_IT_Update);
	//TIM1�ж�����
	TIM_ITConfig(TIM1,TIM_IT_CC4,ENABLE);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	//��������жϺͱȽ��ж�
	
	/*4��������ʱ��*/
	TIM_Cmd(TIM1,DISABLE);//ʧ��TIM1
}


