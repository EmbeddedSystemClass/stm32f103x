#include <Bsp.h> //�ļ��ڰ�����ʱ������ʼ����Debug���������ʼ����
/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     Bsp
 * @Author:     RanHongLiang
 * @Date:       2019-06-26 13:16:27
 * @Description: 
 *��������BSP_Init()���弶֧�����ó�ʼ��
 *---------------------------------------------------------------------------*/

void BSP_Init(void)
{
    bsp_delayinit();
    debugThread_Active();
}
