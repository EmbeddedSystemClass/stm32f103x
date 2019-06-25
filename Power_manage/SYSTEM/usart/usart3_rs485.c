 
 #include "global.h"


TRAN_D_struct   RS485_TRAN_info  ;


void  RS485_ACK_load_buf(u8* hd,u8 transid);

void  RS485_RX_load_buf(u8* buf,u8 len,u8  transid);

void  RS485_CANCEL_SD_buf(u8 transid);
 
void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );
 
void  RS485_dma_send_buf(void);

void   RS485_timeout_fution(void) ;



u8  RS485_CHECK_BUF_VALID(u8 *p) ;

void RS485_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
void RS485_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

u8      RS485_rcvd_crc(char *buf, u8 *transid);
uint8_t RS485_crc8(uint8_t *buf,uint8_t len);

void RS485_uart_fuc(void); 

u8  RS485__msg_process(char *tab);  
 
void RS485_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
{
	char h1,h2;
	u8  s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
	h1 = pbSrc[2*i];
	h2 = pbSrc[2*i+1];

	s1 = toupper(h1) - 0x30;
	if (s1 > 9) 
	s1 -= 7;

	s2 = toupper(h2) - 0x30;
	if (s2 > 9) 
	s2 -= 7;

	pbDest[i] = s1*16 + s2;
	}
}

 
void RS485_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
{
char	ddl,ddh;
int i;

for (i=0; i<nLen; i++)
{
ddh = 48 + pbSrc[i] / 16;
ddl = 48 + pbSrc[i] % 16;
if (ddh > 57) ddh = ddh + 7;
if (ddl > 57) ddl = ddl + 7;
pbDest[i*2] = ddh;
pbDest[i*2+1] = ddl;
}

pbDest[nLen*2] = '\0';
}




u8  RS485_CHECK_BUF_VALID(u8 *p)//��ȫ�յĻ� ��Ϊ��Ч
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
uint8_t RS485_crc8(uint8_t *buf,uint8_t len)
{
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
}


u8 RS485_rcvd_crc(char *buf, u8 *transid)
{
  u8  crc;
  u16 len;
	
	len=strlen(buf);
  if(buf[len-2]=='\r'&&buf[len-1]=='\n')
	{
   buf[len-2] = '\0';
  
		RS485_StrToHex(&crc,(u8*)&buf[len-4] , 1);
		RS485_StrToHex( transid,(u8*)&buf[1] , 1);
  
 
		if(crc == RS485_crc8((u8 *)buf, len-4))
		{  
			 buf[len-4] = '\0';
		   return SUCCESS;
		}
  }
	return FAIL;
}




 void USART3_IRQHandler(void)                	//����3�жϷ������
 {  
  	 u8	 transid=0;
     u32 i,Data_Len=0;
		
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //�ж��Ƿ��ǿ��б�־
    { 
		   USART_ClearITPendingBit(USART3, USART_IT_IDLE);  //������б�־
         i = USART3->SR;  //������б�־
         i = USART3->DR; //������б�־
			   i=i;
        DMA_Cmd(RS485_DMA_RX_CH, DISABLE);// ����  �������
        Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(RS485_DMA_RX_CH);  
        
  		 
						if(Data_Len<UART_RX_LEN&&Data_Len>=2&&UART3_rs485_st.RXcun[Data_Len-2] =='\r'&&UART3_rs485_st.RXcun[Data_Len-1] =='\n' ) 
					{  
						 UART3_rs485_st.RXcun[Data_Len]=0;//��β
					 if( RS485_rcvd_crc((char*)UART3_rs485_st.RXcun, &transid) ==SUCCESS )//�ж�CRC
					 {
						 if(strncmp("D",	(char*)UART3_rs485_st.RXcun, strlen("D")) ==0||strncmp("B",	(char*)UART3_rs485_st.RXcun, strlen("B")) ==0)//������յ�����
						 {	   
							  RS485_RX_load_buf(UART3_rs485_st.RXcun,Data_Len,transid);     //���뵽RX  FIFO   
						 }
						 else if(strncmp("A",	(char*)UART3_rs485_st.RXcun, strlen("A")) ==0)//������ܵ�ACK   A+TRANSID+CRC+/R/N
						 {    
                
							  RS485_CANCEL_SD_buf(transid);  
					 
						 }		 
						 else
						 {
							 
						 }
				   }
					} 
			  
          memset(UART3_rs485_st.RXcun, 0,strlen((char*)UART3_rs485_st.RXcun));
				
  MYDMA_Config(RS485_DMA_RX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.RXcun,UART_RX_LEN,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
   
			   	DMA_Cmd(RS485_DMA_RX_CH, ENABLE);//
    }  		
		 
 }


/*****************DMA �������ж�************************/
void  DMA1_Channel2_IRQHandler(void) 
{   
	   
	 if(DMA_GetITStatus( DMA1_IT_TC2) != RESET) // 
    {   
			   UART3_rs485_st.ALL_tc_flg=TRUE;   

				 DMA_ClearITPendingBit(DMA1_IT_TC2);
		}

}
 
/************************************************* 
*����3   ��ʱ������
*/
void   RS485_timeout_fution(void) 
{  
     UART3_rs485_st.ACK_TIMEOUT_js++;
}
 
 
 /************************************************* 
   �������뻺��
*/ 	 
void   RS485_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
      u8  i=0,cun_rx_lun;
									 
	     cun_rx_lun=UART3_rs485_st.uart_Rx_lun;
			for(i=0;i<USART_TXRX_PK_NUM ;i++)
			{
		    if( RS485_CHECK_BUF_VALID(UART3_rs485_st.RXBuff[cun_rx_lun])==FALSE)//δʹ�õ�BUFF
					{
				   memcpy((u8*)UART3_rs485_st.RXBuff[cun_rx_lun], (u8*)buf,strlen((char*)buf)); 
				    
			 
         RS485_ACK_load_buf((u8*)"A",transid);					
						return;
					}
			 (cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
			} 
			 
			
	   /*********test**********/  
      RS485_ACK_load_buf((u8*)"A",transid);		
    /******************/ 
}
 

/*****************ACK ����*************************/
void  RS485_ACK_load_buf(u8* hd,u8 transid)
{     u8  i=0 ;
      u8  crc=0 ;
	      
      
	      USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 
          
          for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
							  if( RS485_CHECK_BUF_VALID(UART3_rs485_st.ACKBuff[i])==FALSE)//���û������
								{
						 	UART3_rs485_st.ACKBuff[i][0]= hd[0];
						  RS485_HexToStr(&UART3_rs485_st.ACKBuff[i][1],&transid , 1);
						  crc=RS485_crc8(&UART3_rs485_st.ACKBuff[i][0],3);
              RS485_HexToStr(&UART3_rs485_st.ACKBuff[i][3],&crc , 1);	
            
				 							
						  UART3_rs485_st.ACKBuff[i][5]='\r';
						  UART3_rs485_st.ACKBuff[i][6]='\n';
					    UART3_rs485_st.ACKBuff[i][7]=0;
					  
									 break;
								}
							 
			   }
				 
     DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
     USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);   
}



/****************�յ����غ� ����FIFO************************/
void RS485_CANCEL_SD_buf(u8 transid) 
{
   u8  i=0 ;
	 u8  t_id;
	     
        USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 
	
	       for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
							  if( RS485_CHECK_BUF_VALID(UART3_rs485_st.SDBuff[i])==TRUE)//���û������
								{   
					      RS485_StrToHex(&t_id,&UART3_rs485_st.SDBuff[i][1], 1);
									if(transid==t_id)
									{
										 if( i== UART3_rs485_st.uart_SD_lun )
										 {
										 UART3_rs485_st.ACK_RX_OK_flg=TRUE;
									   UART3_rs485_st.ACK_TIMEOUT_js=0;
										 }
									  
							memset(UART3_rs485_st.SDBuff[i], FALSE,strlen((char*)UART3_rs485_st.SDBuff[i]));//��־λ  ȫ��Ϊ FALSE			
									
									 break;
									} 
								} 
			   }
	 
     DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
     USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);   
}

 
///************************************************* 
//*����3  DMA ���� 
//*/ 
void  RS485_dma_send_buf(void)
{   u8  i=0,cun_sd_lun;
      
	
	  if( UART3_rs485_st.ALL_tc_flg==TRUE )//DMA�������
		 {
        USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 
		     
           //����ACK�������Ƿ�������   ACK ���ȷ���
				 for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {      
					  if(RS485_CHECK_BUF_VALID(UART3_rs485_st.ACKBuff[i])==TRUE)//���������
						{   
			  DMA_Cmd(RS485_DMA_TX_CH, DISABLE);// ����  �������			
	 
			  memset(UART3_rs485_st.SDcun, FALSE,strlen((char*)UART3_rs485_st.SDcun));				
							
        memcpy((u8*)UART3_rs485_st.SDcun , (u8*)UART3_rs485_st.ACKBuff[i],strlen((char*)UART3_rs485_st.ACKBuff[i]) ); //������COPY�� 	 
 					  
				 memset(UART3_rs485_st.ACKBuff[i], FALSE,strlen((char*)UART3_rs485_st.ACKBuff[i])); 						
							
		   	MYDMA_Config( RS485_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
        
				MYDMATX_Enable(RS485_DMA_TX_CH,strlen((char*)UART3_rs485_st.SDcun));	
				delay_us(100);
				
				USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���4��DMA���� ����  
					
			  UART3_rs485_st.ALL_tc_flg=FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 
			 			
			 DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
			 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  
							return;
						} 
				 }

				 
				 
     	 if(UART3_rs485_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART3_rs485_st.ACK_RX_OK_flg ==FALSE  )//��ʱ��������յ�Ӧ�����
    		 {
					 
			 DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
			 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  
   		    return;
  			 }			 
				 		  UART3_rs485_st.ACK_RX_OK_flg=FALSE;			
							UART3_rs485_st.ACK_TIMEOUT_js=0;		
				 
          //û��ACKʱ ��������
    			 cun_sd_lun= UART3_rs485_st.uart_SD_lun;
          for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
						  if( RS485_CHECK_BUF_VALID(UART3_rs485_st.SDBuff[cun_sd_lun])==TRUE)//���������
							 {
									if( cun_sd_lun== UART3_rs485_st.uart_SD_lun  )
									{
												 UART3_rs485_st.ACK_TIMEOUT_SUM++;
											if(UART3_rs485_st.ACK_TIMEOUT_SUM>=RE_SEND_SUM)
											{  UART3_rs485_st.ACK_TIMEOUT_SUM=0; 
												 
											memset(UART3_rs485_st.SDBuff[cun_sd_lun], FALSE,strlen((char*)UART3_rs485_st.SDBuff[cun_sd_lun])); 
                      (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
												continue;
											}
									}		
									
									
						  DMA_Cmd(RS485_DMA_TX_CH, DISABLE);// ����  �������	
									
						  memset(UART3_rs485_st.SDcun, FALSE,strlen((char*)UART3_rs485_st.SDcun));			
									
              memcpy((u8*)UART3_rs485_st.SDcun , (u8*)UART3_rs485_st.SDBuff[cun_sd_lun],strlen((char*)UART3_rs485_st.SDBuff[cun_sd_lun]) ); //������COPY�� 	 
					   	MYDMA_Config( RS485_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
         	 
						 
			      	MYDMATX_Enable(RS485_DMA_TX_CH,strlen((char*)UART3_rs485_st.SDcun));	
					delay_us(100);
					 USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���4��DMA���� ����  
									
							UART3_rs485_st.uart_SD_lun =cun_sd_lun; // �ݴ�
							UART3_rs485_st.ALL_tc_flg=FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 

							UART3_rs485_st.ACK_RX_OK_flg=FALSE;			
							UART3_rs485_st.ACK_TIMEOUT_js=0;			
										
           					
									
						 DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
						 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  
							return;
								}
							 (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
			   }

       DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
			 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
				 
		 } 
}



/*****************��������BUF**************************/
void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   )
{     u8  i=0,cun_sd_lun;
 
	 
	
	      if( len >UART_SD_LEN) 
				 {
				   return; 
				 }
        USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  //�ٽ��
        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 
		
				  
   				 cun_sd_lun=  UART3_rs485_st.uart_SD_lun ;
          for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
							  if( RS485_CHECK_BUF_VALID(UART3_rs485_st.SDBuff[cun_sd_lun])==FALSE)//���û������
								{ 
										UART3_rs485_st.SDBuff[cun_sd_lun][0]= d_head;//��ͷ
								  	UART3_rs485_st.SDBuff[cun_sd_lun][1]= d_head>>8;//��ͷ
									  UART3_rs485_st.SDBuff[cun_sd_lun][2]= UART3_rs485_st.txtransid;
										 
									 	memcpy(&UART3_rs485_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
									  memcpy(&UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);
									
									UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len]= d_tail;   //��β
									UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1]= d_tail>>8;
									
 UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+2]=RS485_crc8(&UART3_rs485_st.SDBuff[cun_sd_lun][0],3+sizeof(TRAN_D_struct)+len+1);
										     
									  UART3_rs485_st.txtransid++;	//���++	
									 break;
								}
							 (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
			   }
				  
				 
				 DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE);  //�ٽ��
				 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
 
}


void RS485_uart_fuc(void) 
{

   u8  i=0 ;
	
   USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
   DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 
   
    
      for(i=0;i<USART_TXRX_PK_NUM ;i++)
      {
                     if( RS485_CHECK_BUF_VALID(UART3_rs485_st.RXBuff[i])==TRUE)  
                      {
                        
                        
                      if( RS485__msg_process((char*)UART3_rs485_st.RXBuff[i]) )
                      { 
                        memset(UART3_rs485_st.RXBuff[i], FALSE,strlen((char*)UART3_rs485_st.RXBuff[i]));
                      } 
                      
                        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	                      USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);    // Re-enable interrupts.      
                              return;
                      }
 
      } 
	
     DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	   USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 

}




 u8  RS485__msg_process(char *tab)  
 {
   
   
   

     
    switch(tab[0]) 
    {
        case  'L':  
         
        
          break;
        case  'D':  
         
          break;
        case  'B':  
         
          break;  
        case  'R':  
         
         
          break;
        default:  break; 
    }
   
    return  1;
 }



