 #include "global.h"


TRAN_D_struct   RS485_V_I_TRAN_info  ;



void  RS485_V_I_SD_load_buf2(  u8 * urt_buf ,u8  len   );


void  RS485_V_I_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);  //Ӧ�����뻺��

void  RS485_V_I_RX_load_buf(u8* buf,u8 len,u8  transid); //�������뻺��

void  RS485_V_I_CANCEL_SD_buf(u8 transid);           //ɾ�����ͻ���
 
void  RS485_V_I_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //�������뻺��
 
void  RS485_V_I_dma_send_buf(void);             //DMA���ͺ���

void   RS485_V_I_timeout_fution(void) ;         //��ʱ����


void RS485_V_I_uart_fuc(void);    //���յ����ݺ�����

u8  RS485_V_I__msg_process(char *tab);  



u8  RS485_V_I_CHECK_BUF_VALID(u8 *p) ;    //������ݲ�Ϊ��

void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

 
uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len);       //У��


void  UART3_RS485_V_I_st_msg_process(u8* tabb,u8 len);


 
void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
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

 
void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
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




u8  RS485_V_I_CHECK_BUF_VALID(u8 *p)//��ȫ�յĻ� ��Ϊ��Ч
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
uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len)
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

while(len--) {

crc ^= *buf++;

for(i = 0; i < 8; i++) {

if(crc & 0x01)

crc = (crc >> 1) ^ 0x8C;

else

crc >>= 1;

}

}

return crc;
	
}


 



 void USART3_IRQHandler(void)                	//����3�жϷ������
 {  
  	 TRAN_D_struct   TRAN_info1,TRAN_info2;
     u32 i,Data_Len=0;
	   u8    dat[UART_RX_LEN];
	 
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //�ж��Ƿ��ǿ��б�־
    { 
			
			 DMA_ClearFlag( DMA1_FLAG_GL3 );           // ��DMA��־λ
		   USART_ClearITPendingBit(USART3, USART_IT_IDLE);  //������б�־
         i = USART3->SR;  //������б�־
         i = USART3->DR; //������б�־
			  
         DMA_Cmd(RS485_COMM_DMA_RX_CH, DISABLE);// ����  �������
         Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(RS485_COMM_DMA_RX_CH);  
        
  		 
						if(Data_Len==15) 
					{   
									if(UART3_RS485_V_I_st.RXcun[0] ==0x01&& UART3_RS485_V_I_st.RXcun[1 ]==0X03&&UART3_RS485_V_I_st.RXcun[ 2 ]==0X0a )
									{
										
										
										My_vol= UART3_RS485_V_I_st.RXcun[3]<<8|  UART3_RS485_V_I_st.RXcun[4];   //��ѹֵ
										My_curr= UART3_RS485_V_I_st.RXcun[5]<<8|  UART3_RS485_V_I_st.RXcun[6];     //����ֵ
										My_watt= UART3_RS485_V_I_st.RXcun[7]<<8|  UART3_RS485_V_I_st.RXcun[8];    //����
										My_powr= UART3_RS485_V_I_st.RXcun[9]<<24|UART3_RS485_V_I_st.RXcun[10]<<16|UART3_RS485_V_I_st.RXcun[11]<<8|UART3_RS485_V_I_st.RXcun[12]  ;    //����
										
									//	 LED1=~LED1; 
										
										/*
										         TRAN_info1.dest_dev_num= 0;
									
														 TRAN_info1.dest_addr[0]=0x00;
														 TRAN_info1.dest_addr[1]=0x00;
														 TRAN_info1.dest_addr[2]=0x00;
														 TRAN_info1.dest_addr[3]=0x00;
													 
														 
														 TRAN_info1.source_dev_num= (DTN_curtain<<8|DTN_curtain>>8);
														 
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
										*/
									}
						     
					}
          if(Data_Len==0x9F)
					{
								  if( UART3_RS485_V_I_st.RXcun[0] ==0x01&& UART3_RS485_V_I_st.RXcun[1 ]==0X03&&UART3_RS485_V_I_st.RXcun[ 2 ]==154     )
									{
										     i= crc16_MODBUS( UART3_RS485_V_I_st.RXcun, 0x9F-2  );
											if( (UART3_RS485_V_I_st.RXcun[ 0x9D ] | UART3_RS485_V_I_st.RXcun[ 0x9E ]<<8) ==  i )
											{
													 
													  	My_AC_frequency= UART3_RS485_V_I_st.RXcun[9]<<8|  UART3_RS485_V_I_st.RXcun[10];   
															My_three_phase_W_H16= UART3_RS485_V_I_st.RXcun[19]<<8|  UART3_RS485_V_I_st.RXcun[20];  
													  	My_three_phase_W_L16= UART3_RS485_V_I_st.RXcun[21]<<8|  UART3_RS485_V_I_st.RXcun[22];   
															My_A_PHASE_I= UART3_RS485_V_I_st.RXcun[45]<<8|  UART3_RS485_V_I_st.RXcun[46];  
													  	My_A_PHASE_U= UART3_RS485_V_I_st.RXcun[47]<<8|  UART3_RS485_V_I_st.RXcun[48];   
															My_A_PHASE_W= UART3_RS485_V_I_st.RXcun[53]<<8|  UART3_RS485_V_I_st.RXcun[54];  
													  	My_B_PHASE_I= UART3_RS485_V_I_st.RXcun[93]<<8|  UART3_RS485_V_I_st.RXcun[94];   
															My_B_PHASE_U= UART3_RS485_V_I_st.RXcun[95]<<8|  UART3_RS485_V_I_st.RXcun[96];  
													  	My_B_PHASE_W= UART3_RS485_V_I_st.RXcun[101]<<8|  UART3_RS485_V_I_st.RXcun[102];   
															My_C_PHASE_I= UART3_RS485_V_I_st.RXcun[141]<<8|  UART3_RS485_V_I_st.RXcun[142];  
									  			  	My_C_PHASE_U= UART3_RS485_V_I_st.RXcun[143]<<8|  UART3_RS485_V_I_st.RXcun[144];   
															My_C_PHASE_W= UART3_RS485_V_I_st.RXcun[149]<<8|  UART3_RS485_V_I_st.RXcun[150];  
													 			
												     //���͸�ZIGBEE  �ϴ���Щ
												/*
												     TRAN_info1.dest_dev_num= 0;
									
														 TRAN_info1.dest_addr[0]=0x00;
														 TRAN_info1.dest_addr[1]=0x00;
														 TRAN_info1.dest_addr[2]=0x00;
														 TRAN_info1.dest_addr[3]=0x00;
													 
														 
														 TRAN_info1.source_dev_num= (DTN_curtain<<8|DTN_curtain>>8);
														 
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
												*/
												 
											}
									
									} 
					}

					
					
					memset(UART3_RS485_V_I_st.RXcun, 0,15);
					  
					 RS485_COMM_DMA_RX_CH->CNDTR=UART_RX_LEN; //  ���¸�ֵ����ֵ��������ڵ������
					
			   	DMA_Cmd(RS485_COMM_DMA_RX_CH, ENABLE);//
					
    }  		
		 
 }


/*****************DMA �������ж�************************/
void  DMA1_Channel2_IRQHandler(void) 
{   
	   
	 if(DMA_GetITStatus( DMA1_IT_TC2) != RESET) // 
    {   
			   UART3_RS485_V_I_st.ALL_tc_flg=TRUE;   
         DMA_Cmd(RS485_COMM_DMA_TX_CH, DISABLE);// ����  ������� ����
				 DMA_ClearITPendingBit(DMA1_FLAG_GL2);   //DMA1_FLAG_GL2
			   
			 	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET){};//�ȴ����ͽ���
				RS485_CON1=0;	//����
			  RS485_CON1=0;	//����
		}

}
 
/************************************************* 
*����3   ��ʱ������
*/
void   RS485_V_I_timeout_fution(void) 
{  
     UART3_RS485_V_I_st.ACK_TIMEOUT_js++;
}
 
 
  
/****************�յ����غ� ����FIFO************************/
void RS485_V_I_CANCEL_SD_buf(u8 transid) 
{
   u8  i=0 ;
	  
	
	       for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
							  if( RS485_V_I_CHECK_BUF_VALID(UART3_RS485_V_I_st.SDBuff[i])==TRUE)//���û������
								{   
					     
									if(transid==UART3_RS485_V_I_st.SDBuff[i][2])
									{
										  if( i== UART3_RS485_V_I_st.uart_SD_lun )// 
										  {
										  UART3_RS485_V_I_st.ACK_RX_OK_flg=TRUE;  //����ACK ���ձ�־λ OK
									    UART3_RS485_V_I_st.ACK_TIMEOUT_js=0;
										  }
									  
							memset(UART3_RS485_V_I_st.SDBuff[i], FALSE, 10 );//  ���ǰ��λ���ᱻ�ж�Ϊ��Ч	
									
									 break;
									} 
								} 
			   }
	 
 
}

 
///************************************************* 
//*����3  DMA ���� 
//*/ 
void  RS485_V_I_dma_send_buf(void)
{   u8  i=0,cun_sd_lun;
      
	
	  if( UART3_RS485_V_I_st.ALL_tc_flg==TRUE )//DMA�������
		 {
        
     	 if(UART3_RS485_V_I_st.ACK_TIMEOUT_js<TIMEOUT_SUM_2 && UART3_RS485_V_I_st.ACK_RX_OK_flg ==FALSE  )//  ��ʱδ��ʱ�䣬���� Ӧ���־δ�յ�OK   �����ȴ�
    		 { 
   		    return;
  			 }			 
				 		  UART3_RS485_V_I_st.ACK_RX_OK_flg=FALSE;			
							UART3_RS485_V_I_st.ACK_TIMEOUT_js=0;	

				 
          USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
          DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE);  
				 
				 
          //û��ACKʱ ��������
    			 cun_sd_lun= UART3_RS485_V_I_st.uart_SD_lun;
          for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
						  if( RS485_V_I_CHECK_BUF_VALID(UART3_RS485_V_I_st.SDBuff[cun_sd_lun])==TRUE)//���������
							 {
									if( cun_sd_lun== UART3_RS485_V_I_st.uart_SD_lun  )
									{
												 UART3_RS485_V_I_st.ACK_TIMEOUT_SUM++;
											if(UART3_RS485_V_I_st.ACK_TIMEOUT_SUM> RE_SEND_SUM_2)
											{  UART3_RS485_V_I_st.ACK_TIMEOUT_SUM=0; 
												 
												 
											memset(UART3_RS485_V_I_st.SDBuff[cun_sd_lun], FALSE,10 ); //�ط��ˣ�����û�л�Ӧ�������
                      (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
												continue;
											}
									}		
									
									 
						  memset(UART3_RS485_V_I_st.SDcun, FALSE,10);			
									
              memcpy((u8*)UART3_RS485_V_I_st.SDcun , (u8*)UART3_RS485_V_I_st.SDBuff[cun_sd_lun], UART3_RS485_V_I_st.SDBuff_len[cun_sd_lun] ); //������COPY�� 	 
					    
			 
			        RS485_CON1=1;//485  ����ʹ��
						  RS485_CON1=1;//485  ����ʹ��
						  MYDMA_Config( RS485_COMM_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART3_SDBuff,����USART3_SD_LEN. 
  			
					  	MYDMATX_Enable(RS485_COMM_DMA_TX_CH, UART3_RS485_V_I_st.SDBuff_len[cun_sd_lun] );		
									
							UART3_RS485_V_I_st.uart_SD_lun =cun_sd_lun; // �ݴ�
							UART3_RS485_V_I_st.ALL_tc_flg=FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 

					
						 	UART3_RS485_V_I_st.ACK_RX_OK_flg=FALSE;			
							 UART3_RS485_V_I_st.ACK_TIMEOUT_js=0;			
										
           					
							       break;
								}
							 (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
			   }

       DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE); 
			 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
				 
		 } 
}


/*****************��������BUF**************************/
void  RS485_V_I_SD_load_buf2(  u8 * urt_buf ,u8  len   )
{     u8   i=0,cun_sd_lun;
 
	 
	
        USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  //�ٽ��
        DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE); 
		
				  
   				 cun_sd_lun=  UART3_RS485_V_I_st.uart_SD_lun ;
          for(i=0;i<USART_TXRX_PK_NUM ;i++)
				 {  
							  if( RS485_V_I_CHECK_BUF_VALID(UART3_RS485_V_I_st.SDBuff[cun_sd_lun])==FALSE)//���û������
								{ 
									 
									 	memcpy((u8*)&UART3_RS485_V_I_st.SDBuff[cun_sd_lun][0], urt_buf,len);
									
									UART3_RS485_V_I_st.SDBuff_len[cun_sd_lun]=len;
									
									 break;
								}
							 (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
			   }
				  
				 
				 DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE);  //�ٽ��
				 USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
 
}
 


void  UART3_RS485_V_I_st_msg_process(u8* tabb,u8 len) 
{

   

	
	
	
	


}

 


