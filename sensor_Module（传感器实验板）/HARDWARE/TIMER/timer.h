#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "global.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   


extern void TIM2_Int_Init(u16 arr,u16 psc);
 
 void  ENTER_critical_offtim2(void);
 void  EXTI_critical_offtim2(void);

 
void TIM6_Init(void);
void TIM1_Init(void);
 
#endif
