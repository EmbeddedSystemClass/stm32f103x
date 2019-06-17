#ifndef  __USART1_RS485_V_I_H
#define  __USART1_RS485_V_I_H
 
#include "global.h"	
 

 

extern void  RS485_V_I_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);  //Ӧ�����뻺��

extern void  RS485_V_I_RX_load_buf(u8* buf,u8 len,u8  transid); //�������뻺��

extern void  RS485_V_I_CANCEL_SD_buf(u8 transid);           //ɾ�����ͻ���
 

extern void  RS485_V_I_dma_send_buf(void);             //DMA���ͺ���

extern void   RS485_V_I_timeout_fution(void) ;         //��ʱ����


extern void RS485_V_I_uart_fuc(void);    //���յ����ݺ�����

extern u8  RS485_V_I__msg_process(char *tab);  



extern u8  RS485_V_I_CHECK_BUF_VALID(u8 *p) ;    //������ݲ�Ϊ��

extern void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
extern void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

 
extern uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len);       //У�� 




#pragma pack(1)
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


extern TRAN_D_struct   RS485_V_I_TRAN_info  ;

 extern void  RS485_V_I_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //�������뻺��
 
void  RS485_V_I_SD_load_buf2(  u8 * urt_buf ,u8  len   );

#endif 

