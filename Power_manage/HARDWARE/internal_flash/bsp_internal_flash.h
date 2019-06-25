#ifndef __INTERNAL_FLASH_H
#define	__INTERNAL_FLASH_H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"

/* STM32��������Ʒÿҳ��С2KByte���С�С������Ʒÿҳ��С1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)	//2048
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//д�����ʼ��ַ�������ַ
#define WRITE_START_ADDR  ((uint32_t)0x08008000)
#define WRITE_END_ADDR    ((uint32_t)0x0800C000)



typedef enum 
{
	FAILED = 0, 
  PASSED = !FAILED
} TestStatus;


int InternalFlash_Test(void);

void Write_Flash_J_status(char status);

char Read_Flash_J_status(void);




#endif /* __INTERNAL_FLASH_H */

