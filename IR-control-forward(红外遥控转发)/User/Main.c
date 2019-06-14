/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#define osObjectsPublic                    		 // define objects in main module
#include "includes.h"//�ļ��ڰ��������ں˼���Լ�Bsp��ʼ���Ͳ����жϽ���������

int main (void const *argument)
{
	
	osKernelInitialize ();                    	// initialize CMSIS-RTOS
	
	// initialize peripherals here

	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);
	
		BSP_Init();////�ļ��ڰ�����ʱ������ʼ����Debug���������ʼ����
		
		wirelessThread_Active(1);
		
		MoudleDEC_Init();//ģ�������
		
	
		
		
		tipsLEDActive();//led��ʾ�Ƽ���Դ������������������

		keyMboardActive();//�װ尴�������̺������䰴���ص�������
	
	osKernelStart ();                         	// start thread execution 
}
