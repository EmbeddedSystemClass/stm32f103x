/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     Main
 * @Author:     RanHongLiang
 * @Date:       2019-06-26 10:11:40
 * @Description: 
 *������������ʵѵ̨�װ����
 *	ʹ��CMSIS-RTOS��һ�����������̣߳�
 *	��һ����wirelessThread���������ߴ����̣߳���������ͽ������ݡ�
 * 	�ڶ�����LCD144Disp_Thread������1.44��LCD��ʾ������ʾ��չ�����ͺ�ID��������չ�����ͺ�ID���Լ��������ݡ�
 *	������MoudleDEC_Thread��������չ����л��̡߳�
 *	ע��keyMboard_Thread��tipsLED_Thread�����ڰ����߳��Լ�LED�ͷ������̡߳�
 *---------------------------------------------------------------------------*/
#define osObjectsPublic   // define objects in main module
#include "includes.h"//�ļ��ڰ��������ں˼���Լ�Bsp��ʼ���Ͳ����жϽ���������

/*---------------------------------------------------------------------------
 *
 * @Description:�������������������ڡ�
 * @Param:      void*,�����ⲿ����
 * @Return:     int �����߳��˳�����
 *---------------------------------------------------------------------------*/
int main (void const *argument)
{
	
	osKernelInitialize ();                    	// initialize CMSIS-RTOS
	
	// initialize peripherals here

	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);
	
	BSP_Init();////�ļ��ڰ�����ʱ������ʼ����Debug���������ʼ����
	
	wirelessThread_Active(1);
	
	MoudleDEC_Init();//ģ�������
	
	LCD144Disp_Active();//1.44������ʾ�������̺�����
	
	keyMboardActive();//�װ尴�������̺������䰴���ص�������
	
	tipsLEDActive();//led��ʾ�Ƽ���Դ������������������

	osKernelStart ();                         	// start thread execution 
}
