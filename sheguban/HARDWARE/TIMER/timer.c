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
	 
		rand_seed++;
		//������
		(ACCIDENTAL_time[0]==0)?(ACCIDENTAL_time[0]=ACCIDENTAL_time[0]):(ACCIDENTAL_time[0]--);
		(ACCIDENTAL_time[1]==0)?(ACCIDENTAL_time[1]=ACCIDENTAL_time[1]):(ACCIDENTAL_time[1]--);
		(ACCIDENTAL_time[2]==0)?(ACCIDENTAL_time[2]=ACCIDENTAL_time[2]):(ACCIDENTAL_time[2]--);
		(ACCIDENTAL_time[3]==0)?(ACCIDENTAL_time[3]=ACCIDENTAL_time[3]):(ACCIDENTAL_time[3]--);			
		(ACCIDENTAL_time[4]==0)?(ACCIDENTAL_time[4]=ACCIDENTAL_time[4]):(ACCIDENTAL_time[4]--);
		(ACCIDENTAL_time[5]==0)?(ACCIDENTAL_time[5]=ACCIDENTAL_time[5]):(ACCIDENTAL_time[5]--);
		(ACCIDENTAL_time[6]==0)?(ACCIDENTAL_time[6]=ACCIDENTAL_time[6]):(ACCIDENTAL_time[6]--);
		(ACCIDENTAL_time[7]==0)?(ACCIDENTAL_time[7]=ACCIDENTAL_time[7]):(ACCIDENTAL_time[7]--);
		(ACCIDENTAL_time[8]==0)?(ACCIDENTAL_time[8]=ACCIDENTAL_time[8]):(ACCIDENTAL_time[8]--);
		(ACCIDENTAL_time[9]==0)?(ACCIDENTAL_time[9]=ACCIDENTAL_time[9]):(ACCIDENTAL_time[9]--);
		(ACCIDENTAL_time[10]==0)?(ACCIDENTAL_time[10]=ACCIDENTAL_time[10]):(ACCIDENTAL_time[10]--);
		(ACCIDENTAL_time[11]==0)?(ACCIDENTAL_time[11]=ACCIDENTAL_time[11]):(ACCIDENTAL_time[11]--);
		(ACCIDENTAL_time[12]==0)?(ACCIDENTAL_time[12]=ACCIDENTAL_time[12]):(ACCIDENTAL_time[12]--);
		(ACCIDENTAL_time[13]==0)?(ACCIDENTAL_time[13]=ACCIDENTAL_time[13]):(ACCIDENTAL_time[13]--);			
		(ACCIDENTAL_time[14]==0)?(ACCIDENTAL_time[14]=ACCIDENTAL_time[14]):(ACCIDENTAL_time[14]--);
		(ACCIDENTAL_time[15]==0)?(ACCIDENTAL_time[15]=ACCIDENTAL_time[15]):(ACCIDENTAL_time[15]--);

		(send_urt_time==0)?(send_urt_time=send_urt_time):(send_urt_time--);

		alarm_time++;
	}
}





