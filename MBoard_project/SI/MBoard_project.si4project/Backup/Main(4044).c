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
		
		wirelessThread_Active();////���ݴ����������̺�����������wifi��zigbee��
		
		MoudleDEC_Init();//ģ�������
		
		LCD144Disp_Active();//1.44������ʾ�������̺�����
		
		keyMboardActive();//�װ尴�������̺������䰴���ص�������
		
		tipsLEDActive();//led��ʾ�Ƽ���Դ������������������
	
	osKernelStart ();                         	// start thread execution 
}
