#include "Moudle_DEC.h"
#include "Tips.h"
//�ļ��ڰ����װ���չģ���ۼ�ͨѶģ���۵�PCB���������
//����������⵽ģ�����ʱ���ݲ�ͬPCB���뼤���Ӧ���������̣�
//����鵽ģ��γ�ʱ���ս��Ӧ���̣�
Moudle_attr Moudle_GTA;
bool Exmod_rest = false;

osThreadId tid_MBDEC_Thread;
osThreadDef(MBDEC_Thread, osPriorityNormal, 1, 512);

extern ARM_DRIVER_USART Driver_USART1; //�豸�����⴮��һ�豸����

void stdDeInit(void)
{

	TIM_BDTRInitTypeDef TIM_BDTRStruct;

	TIM_BDTRStructInit(&TIM_BDTRStruct);

	EXTI_DeInit();

	ADC_DeInit(ADC1);
}

void MoudleDEC_ioInit(void)
{ //ģ����ų�ʼ��

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE); //ʹ��ADC1ͨ��ʱ��

	GPIO_InitStructure.GPIO_Pin |= 0xfff8; //Ӳ��ID������ų�ʼ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //Moudle_Check���ų�ʼ��
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //ExtMOD���ų�ʼ��
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void MBDEC_Thread(const void *argument)
{ //ѭ�����
	u16 ID_temp;

	bool M_CHG = true;

	//led2_status = led2_r;
	ID_temp = MID_EXEC_raley8_power;
	Moudle_GTA.Extension_ID = (u8)ID_temp;
	for (;;)
	{
		if (M_CHG)
		{
			//keyIFRActive();
			sourceCMThread_Active();
			M_CHG = false;
		}
		if (Exmod_rest)
		{
			M_CHG = true;

			Exmod_rest = false;

			osThreadTerminate(tid_sourceCM_Thread);
		}
		delay_ms(50);
	}
}

void MoudleDEC_Init(void)
{ //ģ������̼���

	//MoudleDEC_ioInit();
	tid_MBDEC_Thread = osThreadCreate(osThread(MBDEC_Thread), NULL);
}
