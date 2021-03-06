#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include <ctype.h>  
 	
	
	
#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif	
	
#ifndef  FAIL
#define  FAIL     1
#endif

#ifndef  SUCCESS
#define  SUCCESS  0
#endif	
	
	
	
#define    TIMEOUT_SUM   30   //超时20MS
#define    RE_SEND_SUM   2  //重发两次
	
	
#define    TIMEOUT_SUM2   30   //超时20MS
#define    RE_SEND_SUM2   1   //重发两次
	
#define ALL_ACK_LEN 			  7  	//所有ACK长度不超过7   头 CC CC 序号 01 状态 00 尾巴 DD DD CRC8 52
	 
#define UART_SD_LEN  			  200  	// 
#define UART_RX_LEN  			  200  	//  
	  	  
#define USART_TXRX_PK_NUM  			5  	//
 
 


 

#pragma pack(1)
typedef  struct
{ 
	 u8   txtransid;        //序号
	 u8   ACK_TIMEOUT_SUM ;//超时次数
	 u16  ACK_TIMEOUT_js;//超时计数	
   
	 u8   ACK_RX_OK_flg;   //ACK 接受到OK
	 u8   ALL_tc_flg;

	 u8    uart_SD_lun;     //发送轮询
   u8    uart_Rx_lun;    //接收轮询
	
	 
	 u8   SDcun[UART_SD_LEN] ;
   u8   RXcun[UART_RX_LEN] ;
	
	 u8   SDBuff_len[USART_TXRX_PK_NUM]  ;
   u8   RXBuff_len[USART_TXRX_PK_NUM]  ;
	 
   u8   SDBuff[USART_TXRX_PK_NUM][UART_SD_LEN] ;
   u8   RXBuff[USART_TXRX_PK_NUM][UART_RX_LEN] ;
   u8   ACKBuff[USART_TXRX_PK_NUM][ALL_ACK_LEN] ;
	 
}
Uart_struct;

  
extern    Uart_struct    UART3_rs485_st;
 

#define SYS_MAX_TRAN_data_SIZE  70   // 

  
  
//如果想串口中断接收，请不要注释以下宏定义   
extern void   uart3_init(u32 bound);
/*****************************/ 
extern void disable_all_uart_interupt( void  );
extern void enable_all_uart_interupt( void  );



#endif


