#ifndef __USART_H
#define __USART_H
 
 
	
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
	
	
	
#define    TIMEOUT_SUM     			 100    //��ʱ20MS
#define    RE_SEND_SUM      			 1      //�ط�����

#define    TIMEOUT_SUM_2  		         100    //��ʱ20MS
#define    RE_SEND_SUM_2  			 1      //�ط�����

#define    ALL_ACK_LEN 				 10  	//����ACK���Ȳ�����10
 
#define    UART_SD_LEN  			 80  	// 
#define    UART_RX_LEN  			 80	//  
			
#define    USART_TXRX_PK_NUM  	                 1  	//


 
typedef  struct
{   
   u16   dest_dev_num;     //Ŀ���豸��
	 u8    dest_addr[4];     //Ŀ���ַ
	 u16   source_dev_num;   //Դ�豸��
	 u8    source_addr[4];   //Դ��ַ 
	 u8    TYPE_NUM ;        //���ݸ�ʽ��
	 u8    data_len ;        //���ݳ���	 
}
TRAN_D_struct;  
 


typedef  struct
{ 
	 u8   txtransid;        //���
	 u8   ACK_TIMEOUT_SUM ;//��ʱ����
	 u16  ACK_TIMEOUT_js;//��ʱ����	
   
	 u8   ACK_RX_OK_flg;   //ACK ���ܵ�OK
	 u8   ALL_tc_flg;

	 u8    uart_SD_lun;     //������ѯ
   u8    uart_Rx_lun;    //������ѯ
	
	 
	 u8   SDcun[UART_SD_LEN] ;
   u8   RXcun[UART_RX_LEN] ;
	
	 u8   SDBuff_len[USART_TXRX_PK_NUM]  ;
   u8   RXBuff_len[USART_TXRX_PK_NUM]  ;
	 
   u8   SDBuff[USART_TXRX_PK_NUM][UART_SD_LEN] ;
   u8   RXBuff[USART_TXRX_PK_NUM][UART_RX_LEN] ;
   u8   ACKBuff[USART_TXRX_PK_NUM][ALL_ACK_LEN] ;
	 
}
Uart_struct;

 
 
extern   Uart_struct    UART1_zigbee_st; 
 

extern  u8       Rs485_COMM_CHECK_BUF_VALID(u8 *p);
extern  uint8_t  Rs485_COMM_crc8(uint8_t *buf,uint8_t len);
extern  void    Rs485_COMM_uart_timeout_fution(void) ;
extern  void    Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid);
extern  void    Rs485_COMM_dma_send_buf(void);
extern  void    Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   );
extern  void    Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );
extern  void    Rs485_COMM_uart_fuc(void) ;
extern  u8       Rs485_COMM_msg_process(u8 *tab);  
 

extern  u16    send_urt_time ;



#endif


