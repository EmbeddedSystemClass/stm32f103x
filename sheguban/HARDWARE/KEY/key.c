#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "global.h"
 
 
 
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��WIFI_RESET-->GPIOD.12   ��������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTDʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD 12 13

}

 
