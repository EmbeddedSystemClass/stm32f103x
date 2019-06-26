#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"



Uart_struct    UART1_RS485_V_I_st; 
    
/*********************************************************************
 *  LOCAL  FUNC
 */
 void   uart1_init(u32 bound);	  //��ӡ������  


//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA1����

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA   ,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;									//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;					//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); 									//��ʼ������

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);															//����ָ���Ĳ�����ʼ��VIC�Ĵ���


	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel4_IRQn; 	//һ��Ҫ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;		//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);															//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	 
 
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	// 
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	// 
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);  		//���� ����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //����1�����ж� 
	 
	 
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); 		//ʹ�ܴ���1��DMA���� ����  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 


	MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
	DMA_Cmd(UART1_DMA_TX_CH, DISABLE);		// û���ݷ���Ҫ����������һ�����ͻᷢ������
	MYDMA_Config( UART1_DMA_RX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.RXcun,UART_RX_LEN,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
	DMA_Cmd(UART1_DMA_RX_CH, ENABLE);			// 
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC, ENABLE); //��DMA�������ж� 

	DMA_ClearFlag(DMA1_FLAG_GL4); 
	DMA_ClearFlag(DMA1_FLAG_GL5);    

	USART_Cmd(USART1, ENABLE);			//ʹ�ܴ��� 

	memset((u8*)&UART1_RS485_V_I_st,FALSE,sizeof(UART1_RS485_V_I_st));	//��־λ  ȫ��Ϊ FALSE

	UART1_RS485_V_I_st.ALL_tc_flg = TRUE;
}
 
