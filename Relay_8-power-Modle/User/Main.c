/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.21 
 * @Module:     Main
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 17:48:35
 * @Description: 
 *————CMSIS-RTOS,添加全通和全断的处理函数（延时通电，避免电流浪涌）
 *---------------------------------------------------------------------------*/

#define osObjectsPublic // define objects in main module
#include "includes.h"   //文件内包括用于内核激活，以及Bsp初始化和不可中断进程启动；

int main(void const *argument)
{

	osKernelInitialize(); // initialize CMSIS-RTOS

	// initialize peripherals here

	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);

	BSP_Init(); ////文件内包括延时函数初始化及Debug串口输出初始化；

	wirelessThread_Active(1);

	MoudleDEC_Init(); //模块检测进程

	// tipsLEDActive();//led提示灯及无源蜂鸣器的驱动函数；

	keyMboardActive(); //底板按键检测进程函数及其按键回调函数；

	osKernelStart(); // start thread execution
}
