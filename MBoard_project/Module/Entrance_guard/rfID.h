/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ����ʵѵ̨��Ŀ
 * @Version:    V 0.2 
 * @Module:     rfID
 * @Author:     RanHongLiang
 * @Date:       2019-06-27 12:02:37
 * @Description: 
 *��������
 *---------------------------------------------------------------------------*/



#ifndef _RFID_H_
#define _RFID_H_

#include "Eguard.h"
#include "rc522_config.h"
#include <dataTrans_USART.h>

#define	  RFID_EXERES_TTIT		0xDD

#define   macRC522_DELAY()  delay_us(100)

#define   macDummy_Data  0x00

void             PcdReset                   ( void );                       //��λ
void             M500PcdConfigISOType       ( u8 type );                    //������ʽ
char             PcdRequest                 ( u8 req_code, u8 * pTagType ); //Ѱ��
char             PcdAnticoll                ( u8 * pSnr);                   //������

void rfID_Thread(const void *argument);
void rfIDThread_Active(void);

#endif

