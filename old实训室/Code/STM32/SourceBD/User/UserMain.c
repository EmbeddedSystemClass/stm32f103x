#include "System_init.h"
#include "USART_Communication.h"

int main (void) 
{
		System_init();//����ϵͳ��ʼ������
		USART_Communication_485();	//485ͨѶ������
}
