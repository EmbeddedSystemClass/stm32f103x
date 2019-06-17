/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 

#include "stm32f10x_exti.h"// fbd add
#include "drv_interrupt.h"// fbd add


volatile bool uDataReady 							= false;
volatile bool RF_DataExpected 					= false;
volatile bool RF_DataReady 						= false;
volatile bool screenRotated 						= false;
volatile bool disableRotate						= false;

volatile uint32_t nb_ms_elapsed = 0;

extern __IO uint32_t 					SELStatus;
extern bool 									KEYPress;
extern uint8_t 								KEYPressed;
extern bool 									lockKEYUpDown;
extern bool 									refreshMenu;
extern bool										reversed;

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}




void RFTRANS_95HF_IRQ_HANDLER ( void )
{
	if(EXTI_GetITStatus(EXTI_RFTRANS_95HF_LINE) != RESET)
	{
		/* Clear IT flag */
		EXTI_ClearITPendingBit(EXTI_RFTRANS_95HF_LINE);
		/* Disable EXTI Line9 IRQ */
		EXTI->IMR &= ~EXTI_RFTRANS_95HF_LINE;
		
		if(RF_DataExpected)			
			RF_DataReady = true;
		
		/* Answer to command ready*/
		uDataReady = true;		
	}
}

/***************************************************************************************************************
*��������	TIM1_CC_IRQHandler
*������		��ʱ��1�����жϷ�������������������ECHO��������
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��	��
*��д���ڣ�	2019.3.25 10:33
*�汾��¼��	
****************************************************************************************************************/
void TIM1_CC_IRQHandler(void)
{
	if(HcSr04StartF == TRUE)								//�����˲�෢��
	{
		if((Tim1CaptureState&0x80) == 0)						//���λΪ0��û�����һ�����벶��
		{
			if(TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)	//���벶���ж�
			{
				TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);				
				//�����ж�

				if((Tim1CaptureState&0x40) == 0)			//�������벶���е�һ�β���������
				{
					Tim1CaptureState = 0;				//��־�ı䣬��ʼ��ʱ
					Tim1CaptureCount = 0;					//����ֵ����
					TIM_SetCounter(TIM1,0); 				//����������
					Tim1CaptureState |=0X40;
					TIM_OC4PolarityConfig(TIM1,TIM_ICPolarity_Falling);//����Ϊ�½��ز���
				}
				else 
				if(Tim1CaptureState&0x40)				//�ǵ�һ�β��񵽽����أ�˵���˴����½��أ�����ʱ��
				{
					Tim1CaptureState |= 0x80;				//���λ����Ϊ1����־�����һ�����������벶��
					Tim1CaptureCount = TIM_GetCapture4(TIM1);//ȡ�õ�ǰ����ֵ
					HcSr04StartF = FALSE;					//������־��0
					//TIM_GetCapture1(TIM1);					//��ȡ��ǰ�������е���ֵ,������ʱ����Ҫ��main������,�������������ʱ��
					TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Rising);//����Ϊ�����ز���Ϊ��һ�β�����׼��
				}
			}
		}
	}
	//�����ж�
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);
}

/***************************************************************************************************************
*��������	TIM1_IRQHandler
*������		��ʱ��1����жϷ�������������������ECHO��������
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25 10:33
*�汾��¼��	
****************************************************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	if(HcSr04StartF == TRUE)							//�����˲�෢��
	{
		if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//����ж�
		{
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
			//�����ж�
			if(Tim1CaptureState&0x40)					//�Ѿ�����������
			{
				if((Tim1CaptureState&0x3F) == 0x3F)		//�Ѿ������0x3F�Σ�ǿ�ƽ���
				{
					Tim1CaptureState |= 0x80;			//���λ��1						
					HcSr04StartF = FALSE;				//������־��0
					Tim1CaptureCount = 0xFFFF;			//����ֵΪ���
				}
				else
				{
					Tim1CaptureState++; 				//����������û�г�����־λ�ĵ���λ���ܱ�ʾ�ķ�Χ����ô���ñ�־λֱ�Ӽ�1���Դ�����¼����Ĵ������������һֱ����ȥ��ɶʱ�򳬳��˱�־λ�ĵ���λ���ܱ�ʾ�ķ�Χ����ô��ǿ���˳���⣬ֱ����Ϊ�˴μ��ߵ�ƽ�Ѿ�������Ȼ��ֱ�ӿ�����һ�μ��
				}
			}
		}
	}
	//�����ж�
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
}

/***************************************************************************************************************
*��������		TIM3_IRQHandler
*������		��ʱ��3�жϷ�����������תPWM����IO
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*��д���ڣ�	2019.3.25
*�汾��¼��	
****************************************************************************************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{		
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
		if(PwmPowerFlag == TRUE)
		{	
			FAN_PWM_VALUE = 1;
		}
	}

	if(TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET)
	{
		TIM_ClearFlag(TIM3,TIM_IT_CC2);

		FAN_PWM_VALUE = 0;
	}
}

/***************************************************************************************************************
*��������		TIM6_IRQHandler
*������		��ʱ��6�жϷ���������ϵͳʱ�����ֵ����(1ms)
*���ߣ�		hongxiang.liu
*������		��
*����ֵ��		��
*������ڣ�	2019.3.25
*�汾��¼��	
****************************************************************************************************************/
//void TIM6_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearFlag(TIM6,TIM_FLAG_Update);
//		SystemCnt++;			
//	}
//}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
