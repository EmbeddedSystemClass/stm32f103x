#include "ALL_Includes.h"


void LED_ALL_OFF(void);



void LED_ALL_OFF(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(0);      //���

  KEY_MID_BLUE(0);   
  KEY_MID_RED(0);       //�м�

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(0);     //�Ҽ�
}

/******* ������ *******/
/* 
* LED   ��    ��     ��
*       ��    ��     ��
*/
void LED_UP(void)
{
  KEY_LEFT_BLUE(1); 
  KEY_LEFT_RED(0);      //���

  KEY_MID_BLUE(0);   
  KEY_MID_RED(1);       //�м�

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(1);     //�Ҽ�
}

/******* ֹͣ�� *******/
/* 
* LED   ��    ��     ��
*       ��    ��     ��
*/
void LED_PAUSE(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(1);      //���

  KEY_MID_BLUE(1);   
  KEY_MID_RED(0);       //�м�

  KEY_RIGHT_BLUE(0);   
  KEY_RIGHT_RED(1);     //�Ҽ�
}

/******* �½��� *******/
/* 
* LED   ��    ��     ��
*       ��    ��     ��
*/
void LED_DOWN(void)
{
  KEY_LEFT_BLUE(0); 
  KEY_LEFT_RED(1);      //���

  KEY_MID_BLUE(0);   
  KEY_MID_RED(1);       //�м�

  KEY_RIGHT_BLUE(1);   
  KEY_RIGHT_RED(0);     //�Ҽ�
}

/******* ���� *******/
/* 
* LED   �м�������5��
*      
*/
void LED_JOIN(void)
{
  u16 LED_time = 0;
  u8 i;
  for(i=0;i<5;i++)
  { 
    KEY_MID_RED(1);
    KEY_MID_BLUE(0);

    LED_time = 300;
    while( LED_time != 0 ); 

    KEY_MID_RED(0);
    KEY_MID_BLUE(1);

    LED_time = 300;
    while( LED_time != 0 );
  }
}

