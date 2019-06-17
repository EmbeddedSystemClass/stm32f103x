#ifndef _RELAY_CM_H_
#define _RELAY_CM_H_

#include "stm32f10x.h"

#define Relay_OFF PAout(12) = 0;//�̵�����
#define Relay_ON  PAout(12) = 1;//�̵�����

#define Blue_LED_OFF PBout(9) = 1; //���ϼ��ָʾ��
#define Blue_LED_ON  PBout(9) = 0; //���ϼ��ָʾ��

void Relay_Init(void);
 
#endif

