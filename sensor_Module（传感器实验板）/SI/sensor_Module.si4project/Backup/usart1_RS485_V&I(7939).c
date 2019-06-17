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
{  
	u8  i = 0;

	for(i=0;i<3;i++)
	{
		if(  p[i] != 0  )
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

	while(len--) 
	{
		crc ^= *buf++;

		for(i = 0; i < 8; i++) 
		{
			if(crc & 0x01)
			{
				crc = (crc >> 1) ^ 0x8C;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}

//ע�ͣ�Ϊ�Ժ����Ӵӻ����ǣ������ڰѴ˺����ṹ��Ϊswitch-caseǶ��
//���ݵ�ַ�����ֵ�һ��case���ٸ��ݹ��������ֵڶ���case
//����hongxiang.liu 2019.3.26
 void USART1_IRQHandler(void)           	     	//����3�жϷ������
 {  
	u32 i=0,Data_Len = 0;

	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //�ж��Ƿ��ǿ��б�־
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL5 );           	// ��DMA��־λ
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);  //������б�־
		i = USART1->SR;  //������б�־
		i = USART1->DR; //������б�־

		DMA_Cmd(UART1_DMA_RX_CH, DISABLE);			// ����  �������
		Data_Len = UART_RX_LEN- DMA_GetCurrDataCounter(UART1_DMA_RX_CH);  
        
		if(Data_Len==15   ) 
		{  
			if(UART1_RS485_V_I_st.RXcun[0] == 0x01 && UART1_RS485_V_I_st.RXcun[1] == 0x03 && UART1_RS485_V_I_st.RXcun[2] == 0x0a )
			{
				My_vol = UART1_RS485_V_I_st.RXcun[3] << 8 | UART1_RS485_V_I_st.RXcun[4];   //��ѹֵ
				My_curr = UART1_RS485_V_I_st.RXcun[5] << 8 | UART1_RS485_V_I_st.RXcun[6];     //����ֵ
				My_watt = UART1_RS485_V_I_st.RXcun[7] << 8 | UART1_RS485_V_I_st.RXcun[8];    //����
				My_powr = UART1_RS485_V_I_st.RXcun[9] << 24 | UART1_RS485_V_I_st.RXcun[10] << 16 | UART1_RS485_V_I_st.RXcun[11] << 8 | UART1_RS485_V_I_st.RXcun[12] ;//����
				if( My_vol > 100 )
				{
					POWER_220V_FLG = 1; 
				}
				//	LED1=~LED1; 
			}
		} 
		else if(Data_Len == 7)						//���ټƴ������ش����ٲ�ѯӦ��֡�ֽ���
		{
			if(UART1_RS485_V_I_st.RXcun[0]==Anemograph_Addr && UART1_RS485_V_I_st.RXcun[1]==0x03 && UART1_RS485_V_I_st.RXcun[2]==2)	//��ַΪ���ټƵ�ַ�ҹ�����Ϊ0x03���ֽ���Ϊ2
			{
				WindSpeed = (UART1_RS485_V_I_st.RXcun[3]<<8|UART1_RS485_V_I_st.RXcun[4])/10.0;		//�������ֵ
			}
		}
		else if(Data_Len == 9)						//������ʪ�ȴ������ش��¶�+ʪ��Ӧ��֡��������stm32Ĭ��ֻ�����¶�+ʪ�ȵ�2��Ч������ѯ֡
		{
			if(UART1_RS485_V_I_st.RXcun[0]==Soil_Detector_Addr && UART1_RS485_V_I_st.RXcun[1]==0x03 && UART1_RS485_V_I_st.RXcun[2]==0x04)	//��ַΪ������������ַ�ҹ�����Ϊ0x03���ֽ���Ϊ4
			{
				SoilHum  = (UART1_RS485_V_I_st.RXcun[3]<<8|UART1_RS485_V_I_st.RXcun[4])/10;			//��������ʪ��ֵ
				SoilTemp = (UART1_RS485_V_I_st.RXcun[5]<<8|UART1_RS485_V_I_st.RXcun[6])/10;			//���������¶�ֵ
				///////////****************��ע����������¶��ǲ�����ʽ����������ԭ��ת����ʽ����������������������hongxiang.liu****************///////////////
			}
		}

		memset(UART1_RS485_V_I_st.RXcun, 0,15);		//��ս�������

		UART1_DMA_RX_CH -> CNDTR = UART_RX_LEN; 	//���¸�ֵ����ֵ��������ڵ������

		DMA_Cmd(UART1_DMA_RX_CH, ENABLE);			//���¿���DMA����			
	}  			 
}


/*****************DMA �������ж�************************/
void  DMA1_Channel4_IRQHandler(void) 
{    
	if(DMA_GetITStatus( DMA1_IT_TC4) != RESET) // 
	{   
		UART1_RS485_V_I_st.ALL_tc_flg = TRUE;   
		DMA_Cmd(UART1_DMA_TX_CH, DISABLE);// ����  ������� ����
		DMA_ClearITPendingBit(DMA1_FLAG_GL4);   //DMA1_FLAG_GL2

		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET){};//�ȴ����ͽ���
		RS485_CON1 = 0;	//����
		RS485_CON1 = 0;	//����
	}
}
 
/************************************************* 
*����3   ��ʱ������
*/
void   RS485_V_I_timeout_fution(void) 
{  
	UART1_RS485_V_I_st.ACK_TIMEOUT_js++;
}
 
 
  

///************************************************* 
//*����3  DMA ���� 
//*/ 
void  RS485_V_I_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;
	
	if( UART1_RS485_V_I_st.ALL_tc_flg == TRUE )//DMA�������
	{
		if(UART1_RS485_V_I_st.ACK_TIMEOUT_js < TIMEOUT_SUM_2 && UART1_RS485_V_I_st.ACK_RX_OK_flg == FALSE  )//  ��ʱδ��ʱ�䣬���� Ӧ���־δ�յ�OK   �����ȴ�
		{ 
			return;
		}			 
		UART1_RS485_V_I_st.ACK_RX_OK_flg = FALSE;			
		UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;	

		USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE);  
				 	 
		//û��ACKʱ ��������
		cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun;
		for(i=0; i<USART_TXRX_PK_NUM ; i++)
		{  
			if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == TRUE)//���������
			{
				if( cun_sd_lun == UART1_RS485_V_I_st.uart_SD_lun  )
				{
					UART1_RS485_V_I_st.ACK_TIMEOUT_SUM ++;
					if(UART1_RS485_V_I_st.ACK_TIMEOUT_SUM > RE_SEND_SUM_2)
					{  
						UART1_RS485_V_I_st.ACK_TIMEOUT_SUM = 0; 

						memset(UART1_RS485_V_I_st.SDBuff[cun_sd_lun], FALSE,10 ); //�ط��ˣ�����û�л�Ӧ�������
						(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun = 0):(cun_sd_lun++);	
						continue;
					}
				}								 
				memset(UART1_RS485_V_I_st.SDcun, FALSE,10);			

				memcpy((u8*)UART1_RS485_V_I_st.SDcun , (u8*)UART1_RS485_V_I_st.SDBuff[cun_sd_lun], UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] ); //������COPY�� 	 


				RS485_CON1 = 1;//485  ����ʹ��
				RS485_CON1 = 1;//485  ����ʹ��
				MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 

				MYDMATX_Enable(UART1_DMA_TX_CH, UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] );		

				UART1_RS485_V_I_st.uart_SD_lun = cun_sd_lun; // �ݴ�
				UART1_RS485_V_I_st.ALL_tc_flg = FALSE;//��������� ��Ҫ���͵� ����FLASE  ��Ȼֱ����ʾTRUE  Ȼ��uart_dma_send_buf����Կ��� 				 


				UART1_RS485_V_I_st.ACK_RX_OK_flg = FALSE;			
				UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;			
									
				break;
			}
			(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
	} 
}


/*****************��������BUF**************************/
//ע�ͣ�Uart1����modbusЭ�飬ֱ�ӵ��ô˺������ɣ��Դ����桪��hongxiang.liu
void  RS485_V_I_SD_load_buf2( u8 * urt_buf ,u8 len )
{     
	u8 i=0, cun_sd_lun;

	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //�ٽ��
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun ;
	for(i=0; i<USART_TXRX_PK_NUM; i++)
	{  
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == FALSE)//���û������
		{ 
			memcpy((u8*)&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][0], urt_buf,len);

			UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] = len;

			break;
		}
		(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
	}
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE);  //�ٽ��
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

