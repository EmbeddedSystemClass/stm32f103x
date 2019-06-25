#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"


 
  
void   Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid);
 
void   Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

void   Rs485_COMM_uart_fuc(void) ;

u8     Rs485_COMM_msg_process(char *tab); 

void   Rs485_COMM_dma_send_buf(void); 

void   Rs485_COMM_uart_timeout_fution(void);

u8     Rs485_COMM_CHECK_BUF_VALID(u8 *p) ;


uint8_t Rs485_COMM_crc8(uint8_t *buf,uint8_t len);

void    Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   );


void Android_CANCEL_SD_buf(u8 transid);

void  Android_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);


u8  Rs485_COMM_CHECK_BUF_VALID(u8 *p) 
{  u8  i=0;
	
			for(i=0;i<3;i++)
			{
				if(  p[i] !=0  )
				 {
				 return TRUE;
				 } 
			}
    return FALSE;
}


/*********************************************************************
 *  crc
 */
uint8_t Rs485_COMM_crc8(uint8_t *buf,uint8_t len)
{
	/*
  uint8_t i,l,crc;
  uint16_t init=0;
  for(l=0;l<len;l++)
  {
    init^=(buf[l]*0x100);
    for(i=0;i<8;i++)
    {
      if(init&0x8000)
        init^=0x8380;
      init*=2;
    }
  }
  crc = init/0x100;
  return crc;
	*/
	uint8_t crc;
	uint8_t i;
	
	crc = 0;

	while(len--) 
	{
		crc ^= *buf++;

		for(i = 0; i < 8; i++) 
		{
			if(crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
			else
			crc >>= 1;
		}
	}
	return crc;
}



/*****************ACK ����*************************/
void  Android_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid)
{     
	u8  i=0 ;
	u8  crc=0 ;
	
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.ACKBuff[i])==FALSE)//���û������
		{
			UART1_zigbee_st.ACKBuff[i][0]= hd>>8;
			UART1_zigbee_st.ACKBuff[i][1]= hd ;
			UART1_zigbee_st.ACKBuff[i][2]= transid ;
			UART1_zigbee_st.ACKBuff[i][3]= t_status;
			UART1_zigbee_st.ACKBuff[i][4]= tail>>8 ;
			UART1_zigbee_st.ACKBuff[i][5]= tail  ;

			crc=Rs485_COMM_crc8(&UART1_zigbee_st.ACKBuff[i][0],6); 

			UART1_zigbee_st.ACKBuff[i][6]= crc  ; 

			break;
		}
	}
}

/************************************************* 
*����1�жϳ���
*/
void USART1_IRQHandler(void)                	//����1�жϷ������
{  
	u8	transid = 0,crc = 0;
	u32 i,Data_Len = 0;

	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //�ж��Ƿ��ǿ��б�־
	{ 

		DMA_ClearFlag( DMA1_FLAG_GL5 );           // ��DMA��־λ
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);  //������б�־

		i = USART1->SR;  //������б�־
		i = USART1->DR; //������б�־

		DMA_Cmd(UART1_DMA_RX_CH, DISABLE);// ����  �������
		Data_Len = UART_RX_LEN- DMA_GetCurrDataCounter(UART1_DMA_RX_CH);  

		//LED1=!LED1; 
		send_urt_time = 150;

		if(Data_Len<UART_RX_LEN&&Data_Len >= 21 ) 
		{  
			if( UART1_zigbee_st.RXcun[0] == 0xAA && UART1_zigbee_st.RXcun[1] == 0xAA)
			{
				i = 17+UART1_zigbee_st.RXcun[16];
				if( UART1_zigbee_st.RXcun[ i ] == 0xBB && UART1_zigbee_st.RXcun[i+1] == 0xBB )
				{
					crc=Rs485_COMM_crc8( (u8*) &UART1_zigbee_st.RXcun[ 0 ], i+2);
					if(  UART1_zigbee_st.RXcun[ i+2 ] == crc  )//�ж�CRC
					{
						//������յ�����
						transid =   UART1_zigbee_st.RXcun[ 2 ] ;
						Rs485_COMM_UART_RX_load_buf(UART1_zigbee_st.RXcun,i+3,transid);     //���뵽RX  FIFO   
					}
				}
			}	
		} 
			/*	  else if(Data_Len<UART_RX_LEN&&Data_Len>= 7  )
					{
					        if( UART1_zigbee_st.RXcun[0] ==0xCC&&UART1_zigbee_st.RXcun[1] ==0xCC&&UART1_zigbee_st.RXcun[ 4 ]==0XDD&&UART1_zigbee_st.RXcun[ 5 ]==0XDD )
									{			
											 if(  UART1_zigbee_st.RXcun[ 6 ]  ==Rs485_COMM_crc8(  &UART1_zigbee_st.RXcun[ 0 ], 6)  )//�ж�CRC
											 {
												     //������ܵ�ACK   
												      
														transid =   UART1_zigbee_st.RXcun[ 2 ] ;
														Android_CANCEL_SD_buf(transid);        
											 }	 
									}				
					}
					*/
			  
		memset(UART1_zigbee_st.RXcun, 0,i+2);

		UART1_DMA_RX_CH->CNDTR=UART_RX_LEN; //  ���¸�ֵ����ֵ��������ڵ������

		DMA_Cmd(UART1_DMA_RX_CH, ENABLE);//		
	} 		 
} 
  
 


/************************************************* 
*����1 DMA �ж�
*/  
void  DMA1_Channel4_IRQHandler(void) 
{   
	if(DMA_GetITStatus( DMA1_IT_TC4) != RESET) // 
	{  
		UART1_zigbee_st.ALL_tc_flg = TRUE; 

		DMA_Cmd(UART1_DMA_TX_CH, DISABLE);// ����  ������� ����

		DMA_ClearITPendingBit(DMA1_FLAG_GL4);   //DMA1_FLAG_GL7

		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET){};//�ȴ����ͽ���
	} 
}

 

/************************************************* 
*����1   ��ʱ������
*/
void  Rs485_COMM_uart_timeout_fution(void) 
{  
	UART1_zigbee_st.ACK_TIMEOUT_js++; 
}
 

 /************************************************* 
   �������뻺��
*/ 	 
void   Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
	u8  i=0,cun_rx_lun;

	cun_rx_lun=UART1_zigbee_st.uart_Rx_lun;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[cun_rx_lun])==FALSE)//δʹ�õ�BUFF
		{
			memcpy((u8*)UART1_zigbee_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
	} 
	// test   
	//   Android_ACK_load_buf(0XCCCC,0XDDDD,0,transid);	
}
 
 
 /****************�յ����غ� ����FIFO************************/
void Android_CANCEL_SD_buf(u8 transid) 
{
	u8  i=0 ;
	
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[i]) == TRUE)//���û������
		{   
			if(transid == UART1_zigbee_st.SDBuff[i][2])
			{
				if( i == UART1_zigbee_st.uart_SD_lun )// 
				{
					UART1_zigbee_st.ACK_RX_OK_flg = TRUE;  //����ACK ���ձ�־λ OK
					UART1_zigbee_st.ACK_TIMEOUT_js = 0;

					//fbd
					UART1_zigbee_st.ACK_TIMEOUT_SUM	= 0;//��������ʱ��0 �����ۼƼ��������������ݻ�û����ɾ����

					memset(UART1_zigbee_st.SDBuff[i], FALSE, 10 );//  ���ǰ��λ���ᱻ�ж�Ϊ��Ч	
					//  memset(UART1_zigbee_st.SDBuff[i]+5, 5, 5 );//  ���ǰ��λ���ᱻ�ж�Ϊ��Ч	
				}
				break;
			} 
		} 
	}
}

 
///************************************************* 
//*����3  DMA ���� 
//*/ 
void  Rs485_COMM_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;
      
	if( UART1_zigbee_st.ALL_tc_flg==TRUE )//DMA�������
	{
	/*	 for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{      
			if(Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.ACKBuff[i])==TRUE)//���������
			{   
				memset(UART1_zigbee_st.SDcun, FALSE,7);				

				memcpy((u8*)UART1_zigbee_st.SDcun , (u8*)UART1_zigbee_st.ACKBuff[i], 7 ); //������COPY�� 	 

				memset(UART1_zigbee_st.ACKBuff[i], FALSE,7); 						

				MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_zigbee_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 

				MYDMATX_Enable(UART1_DMA_TX_CH,7);		

				UART1_zigbee_st.ALL_tc_flg=FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 

				return;
			} 
		}
	*/
			  
		if(UART1_zigbee_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART1_zigbee_st.ACK_RX_OK_flg ==FALSE  )//��ʱ��������յ�Ӧ�����
		{ 
			return;
		}			 
		UART1_zigbee_st.ACK_RX_OK_flg=FALSE;			
		UART1_zigbee_st.ACK_TIMEOUT_js=0;		

		USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE);

		//û��ACKʱ ��������
		cun_sd_lun= UART1_zigbee_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun])==TRUE)//���������
			{
				if( cun_sd_lun== UART1_zigbee_st.uart_SD_lun  )
				{
					UART1_zigbee_st.ACK_TIMEOUT_SUM++;
					if(UART1_zigbee_st.ACK_TIMEOUT_SUM> RE_SEND_SUM)
					{  
						UART1_zigbee_st.ACK_TIMEOUT_SUM=0; 
						memset(UART1_zigbee_st.SDBuff[cun_sd_lun], FALSE,10 ); //�ط��ˣ�����û�л�Ӧ�������
						(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);		
						continue;
					}
				}								 
				memset(UART1_zigbee_st.SDcun, FALSE,10);			

				memcpy((u8*)UART1_zigbee_st.SDcun , (u8*)UART1_zigbee_st.SDBuff[cun_sd_lun], UART1_zigbee_st.SDBuff_len[cun_sd_lun] ); //������COPY�� 	 

				MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_zigbee_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 

				MYDMATX_Enable(UART1_DMA_TX_CH, UART1_zigbee_st.SDBuff_len[cun_sd_lun] );		

				UART1_zigbee_st.uart_SD_lun = cun_sd_lun; // �ݴ�
				UART1_zigbee_st.ALL_tc_flg = FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 

				UART1_zigbee_st.ACK_RX_OK_flg = FALSE;			
				UART1_zigbee_st.ACK_TIMEOUT_js = 0;			

				break;
			}
			(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 	 
	} 	 
}

 /*****************��������BUF**************************/
void  Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   )
{     
	u8   i=0,cun_sd_lun;
	
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun=  UART1_zigbee_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun])==FALSE)//���û������
		{ 

			memcpy((u8*)&UART1_zigbee_st.SDBuff[cun_sd_lun][0], urt_buf,len);

			UART1_zigbee_st.SDBuff_len[cun_sd_lun]=len;					
			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM -1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}
 

/*****************��������BUF**************************/
void  Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
	u8  i=0,cun_sd_lun;
	u8  crc=0 ,z_c=0;
	
	if(len >UART_SD_LEN ) 
	{
		return; 
	}
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 
	cun_sd_lun=  UART1_zigbee_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun])==FALSE)//���û������
		{ 
			UART1_zigbee_st.SDBuff[cun_sd_lun][0]= d_head;//��ͷ 
			UART1_zigbee_st.SDBuff[cun_sd_lun][1]= d_head>>8;//��ͷ
			UART1_zigbee_st.SDBuff[cun_sd_lun][2]= UART1_zigbee_st.txtransid;

			memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len]= d_tail;   //��β
			UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1]= d_tail>>8;

			z_c=3+sizeof(TRAN_D_struct)+len+2;
			crc= Rs485_COMM_crc8(&UART1_zigbee_st.SDBuff[cun_sd_lun][0],z_c);

			UART1_zigbee_st.SDBuff[cun_sd_lun][z_c]=crc;

			UART1_zigbee_st.SDBuff_len[cun_sd_lun]= z_c+1    ;

			UART1_zigbee_st.txtransid++;	//���++		

			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM -1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

void Rs485_COMM_uart_fuc(void) 
{
	u8  i=0 ;
	
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 
    
	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{
		if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[i])==TRUE)  
		{
			if( Rs485_COMM_msg_process((char*)UART1_zigbee_st.RXBuff[i]) )
			{ 
				memset(UART1_zigbee_st.RXBuff[i], FALSE,10);
			} 
			break;
		}
	} 
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

u8 Rs485_COMM_msg_process(char *tab)  
{
	TRAN_D_struct   TRAN_info1 ;
	u8    fuc_dat[UART_RX_LEN];
	u8    dat[UART_RX_LEN];
	u16   abc=0;
	u8    i;

	memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  


	TRAN_info1.source_dev_num=(TRAN_info1.source_dev_num<<8|TRAN_info1.source_dev_num>>8);

	TRAN_info1.dest_dev_num=(TRAN_info1.dest_dev_num<<8|TRAN_info1.dest_dev_num>>8);

	if(TRAN_info1.TYPE_NUM==3)
	{
		switch(  fuc_dat[0]<<8|fuc_dat[1]  ) 
		{
			case  notify_net_status:     
				join_flg=fuc_dat[2]; 

				if(join_flg)
				{
					//LED�� ������
					JOIN_LED =0;

					TRAN_info1.dest_dev_num= 0;

					TRAN_info1.dest_addr[0]=0x00;
					TRAN_info1.dest_addr[1]=0x00;
					TRAN_info1.dest_addr[2]=0x00;
					TRAN_info1.dest_addr[3]=0x00;


					TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

					TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
					TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
					TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
					TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

					TRAN_info1.TYPE_NUM=3    ;   //��һ��
					TRAN_info1.data_len= 18   ;

					dat[0]=0; 
					dat[1]= notify_net_status;
					// dat[1]=upload_info; //

					dat[2]=0; // 
					dat[3]=DTN_POWER_SUBAREA; // 
					dat[4]=(u8) MCU_UID[0];
					dat[5]=(u8)(MCU_UID[0]>>8);
					dat[6]=(u8)(MCU_UID[0]>>16);
					dat[7]=(u8)(MCU_UID[0]>>24); 

					memset( dat+8,0,8  );

					dat[8+8]=JDQ_16BIT;  
					dat[8+9]=JDQ_16BIT>>8;

					Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );	

					TRAN_info1.dest_dev_num= 0;

					TRAN_info1.dest_addr[0]=0x00;
					TRAN_info1.dest_addr[1]=0x00;
					TRAN_info1.dest_addr[2]=0x00;
					TRAN_info1.dest_addr[3]=0x00;


					TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

					TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
					TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
					TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
					TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

					TRAN_info1.TYPE_NUM=3    ;   //��һ��
					TRAN_info1.data_len= 18   ;

					dat[0]=0; 
					//dat[1]= notify_net_status;
					dat[1]=upload_info; //

					dat[2]=0; // 
					dat[3]=DTN_POWER_SUBAREA; // 
					dat[4]=(u8) MCU_UID[0];
					dat[5]=(u8)(MCU_UID[0]>>8);
					dat[6]=(u8)(MCU_UID[0]>>16);
					dat[7]=(u8)(MCU_UID[0]>>24); 

					memset( dat+8,0,8  );

					dat[8+8]=JDQ_16BIT;  
					dat[8+9]=JDQ_16BIT>>8;

					Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );	

					Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );	

				}
				else
				{
					//LED�� ������
					JOIN_LED =1;
				}
				break;
				
			case  control_info:    //�����豸  10���̵�������  ��һ��2�ֽڰ�λ��ʾ
			if((fuc_dat[2]<<8|fuc_dat[3])==DTN_POWER_SUBAREA &&   TRAN_info1.data_len==18 )
			{
				abc=fuc_dat[16]|fuc_dat[17]<<8;
				if( abc!=0xffff  )
				{  	
					JDQ_16BIT= abc;
					write_595_JDQ( (u8*)&abc, 2 );
				}
				else 
				{
					abc=0;
					for(i=0;i<10;i++)
					{
						if(!(JDQ_16BIT &(1<<i)) )
						{
							JDQ_16BIT|=(1<<i);

							abc=JDQ_16BIT;
							write_595_JDQ( (u8*)&abc, 2 );
							delay_ms(700);
						}
					}
				}
				/*		 
				TRAN_info1.dest_dev_num= 0;

				TRAN_info1.dest_addr[0]=0x00;
				TRAN_info1.dest_addr[1]=0x00;
				TRAN_info1.dest_addr[2]=0x00;
				TRAN_info1.dest_addr[3]=0x00;


				TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

				TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
				TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM=3    ;   //��һ��
				TRAN_info1.data_len= 18   ;

				dat[0]=0; 
				dat[1]=upload_info; //

				dat[2]=0; // 
				dat[3]=DTN_POWER_SUBAREA; // 
				dat[4]=(u8) MCU_UID[0];
				dat[5]=(u8)(MCU_UID[0]>>8);
				dat[6]=(u8)(MCU_UID[0]>>16);
				dat[7]=(u8)(MCU_UID[0]>>24); 

				memset( dat+8,0,8  );

				dat[8+8]=JDQ_16BIT;  
				dat[8+9]=JDQ_16BIT>>8;

				Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );		 
				*/
			}
			break;
		case  query_dev_info:   //�����һ��  �豸ʶ��� �͵�ַ 
			if( fuc_dat[16]==1  )  //��ѯ��һ���豸��Ϣ  ��ѯ�̵���ͨ��
			{
				TRAN_info1.dest_dev_num = 0;

				TRAN_info1.dest_addr[0]=0x00;
				TRAN_info1.dest_addr[1]=0x00;
				TRAN_info1.dest_addr[2]=0x00;
				TRAN_info1.dest_addr[3]=0x00;

				TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

				TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
				TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM=3    ;   //��һ��
				TRAN_info1.data_len= 18   ;

				dat[0]=0; 
				dat[1]=upload_info; //

				dat[2]=0; // 
				dat[3]=DTN_POWER_SUBAREA; // 
				dat[4]=(u8) MCU_UID[0];
				dat[5]=(u8)(MCU_UID[0]>>8);
				dat[6]=(u8)(MCU_UID[0]>>16);
				dat[7]=(u8)(MCU_UID[0]>>24); 

				memset( dat+8,0,8  );

				dat[8+8]=JDQ_16BIT;  
				dat[8+9]=JDQ_16BIT>>8; 
				Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );									
			}
			else if(  fuc_dat[16]==2   )//��ѯ��ѹ���� ���ʼ�
			{
				TRAN_info1.dest_dev_num= 0;

				TRAN_info1.dest_addr[0]=0x00;
				TRAN_info1.dest_addr[1]=0x00;
				TRAN_info1.dest_addr[2]=0x00;
				TRAN_info1.dest_addr[3]=0x00;

				TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

				TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
				TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM=3    ;   //��һ��
				TRAN_info1.data_len= 50  ;

				dat[0]=0; 
				dat[1]=upload_info; //

				dat[2]=0; // 
				dat[3]=DTN_POWER_SUBAREA; // 
				dat[4]=(u8) MCU_UID[0];
				dat[5]=(u8)(MCU_UID[0]>>8);
				dat[6]=(u8)(MCU_UID[0]>>16);
				dat[7]=(u8)(MCU_UID[0]>>24); 

				memset( dat+8,0,8  );

				memcpy(dat+16,(u8*)&My_AC_frequency ,2);
				memcpy(dat+18,(u8*)&My_three_phase_W_H16 ,2);
				memcpy(dat+20,(u8*)&My_three_phase_W_L16 ,2);
				memcpy(dat+22,(u8*)&My_A_PHASE_I ,2);
				memcpy(dat+24,(u8*)&My_A_PHASE_U ,2);
				memcpy(dat+26,(u8*)&My_A_PHASE_W ,2);
				memcpy(dat+28,(u8*)&My_B_PHASE_I ,2);
				memcpy(dat+30,(u8*)&My_B_PHASE_U ,2);
				memcpy(dat+32,(u8*)&My_B_PHASE_W ,2);
				memcpy(dat+34,(u8*)&My_C_PHASE_I ,2);
				memcpy(dat+36,(u8*)&My_C_PHASE_U ,2);														 
				memcpy(dat+38,(u8*)&My_C_PHASE_W ,2);														 

				memcpy(dat+40,(u8*)&My_vol ,2);
				memcpy(dat+42,(u8*)&My_curr ,2);
				memcpy(dat+44,(u8*)&My_watt ,2);														 
				memcpy(dat+46,(u8*)&My_powr ,4);	 

				Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );

			}
			break;
			default:  break; 
		}
	}
	return  1;
}
