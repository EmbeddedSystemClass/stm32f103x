#ifndef __USART_H
#define __USART_H
 
 #include  "global.h"
 
 	
	
	
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
	
	
	
#define    TIMEOUT_SUM     			 100   //��ʱ20MS
#define    RE_SEND_SUM      			1   //�ط�����

#define    TIMEOUT_SUM_2  		  100   //��ʱ20MS
#define    RE_SEND_SUM_2  			  1   //�ط�����

#define    ALL_ACK_LEN 				   10  	//����ACK���Ȳ�����10
 
#define    UART_SD_LEN  			  250  	// 
#define    UART_RX_LEN  			  250  	//  
			
#define    USART_TXRX_PK_NUM  	  5  	//


 
#pragma pack(1)
typedef  struct
{ 
	u8   txtransid;        //���
	u8   ACK_TIMEOUT_SUM ;	//��ʱ����
	u16  ACK_TIMEOUT_js;	//��ʱ����	

	u8   ACK_RX_OK_flg;   //ACK ���ܵ�OK
	u8   ALL_tc_flg;

	u8   uart_SD_lun;     //������ѯ
	u8   uart_Rx_lun;    //������ѯ


	u8   SDcun[UART_SD_LEN] ;
	u8   RXcun[UART_RX_LEN] ;

	u8   SDBuff_len[USART_TXRX_PK_NUM]  ;
	u8   RXBuff_len[USART_TXRX_PK_NUM]  ;

	u8   SDBuff[USART_TXRX_PK_NUM][UART_SD_LEN] ;
	u8   RXBuff[USART_TXRX_PK_NUM][UART_RX_LEN] ;
	u8   ACKBuff[USART_TXRX_PK_NUM][ALL_ACK_LEN] ;
	 
}
Uart_struct;

 
extern Uart_struct    UART1_RS485_V_I_st;
extern Uart_struct    UART2_RS485_COMM_st;  
  




#define SYS_MAX_TRAN_data_SIZE  70   // 

  

//����봮���жϽ��գ��벻Ҫע�����º궨��   
extern void   uart1_init (u32 bound);	  
extern void   uart2_init (u32 bound);	 
/*****************************/ 

#endif


