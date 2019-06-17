#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"

Uart_struct    UART3_android_st; 
Uart_struct    UART4_st;  
Uart_struct    UART2_rs485_st;

/*********************************************************************
 *  LOCAL  FUNC
 */ 
 void   uart3_init(u32 bound);
 void   uart4_init(u32 bound);	// 
 void   uart2_init(u32 bound);	// 
 void   disable_all_uart_interupt( void  );
 void   enable_all_uart_interupt( void  );


void uart4_init(u32 bound)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA1���� 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4   ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC   ,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); //��ʼ������
   
	  
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
  NVIC_InitStructure.NVIC_IRQChannel =  DMA2_Channel4_5_IRQn; //һ��Ҫ�����ж�����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
	 
	 
  USART_ITConfig(UART4, USART_IT_TXE,  DISABLE);// 
  USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//  
  USART_ITConfig(UART4, USART_IT_TC,   DISABLE);  //���� ����ж�
  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);  //����1�����ж�
  
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���4��DMA���� ����  
  USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE); 
	
  MYDMA_Config( ZIG1_DMA_TX_CH,(u32)&UART4->DR,(u32)UART4_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
  DMA_Cmd(ZIG1_DMA_TX_CH, DISABLE);// û���ݷ���Ҫ����������һ�����ͻᷢ������
  MYDMA_Config( ZIG1_DMA_RX_CH,(u32)&UART4->DR,(u32)UART4_st.RXcun,UART_RX_LEN ,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
  DMA_Cmd(ZIG1_DMA_RX_CH, ENABLE);// 
		
  DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,  ENABLE); //��DMA�������ж�
	   
		 
  DMA_ClearFlag(DMA2_FLAG_GL3); 
  DMA_ClearFlag(DMA2_FLAG_GL5);     
	
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 

	memset((u8*)&UART4_st, FALSE,sizeof(UART4_st));//��־λ  ȫ��Ϊ FALSE
   
  UART4_st.ALL_tc_flg=TRUE;
	 
}


void uart3_init(u32 bound)
{   
 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA1���� 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3   ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO   ,ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//GPIO_Mode_IN_FLOATING

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	

  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������
   
	  
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel2_IRQn; //һ��Ҫ�����ж�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 


	USART_ITConfig(USART3, USART_IT_TXE,  DISABLE);// 
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//  
	USART_ITConfig(USART3, USART_IT_TC,   DISABLE);  //���� ����ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  //����1�����ж�

	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���4��DMA���� ����  
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE); 
	
	
  MYDMA_Config( Android_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_android_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
  DMA_Cmd(Android_DMA_TX_CH, DISABLE);// û���ݷ���Ҫ����������һ�����ͻᷢ������
  MYDMA_Config( Android_DMA_RX_CH,(u32)&USART3->DR,(u32)UART3_android_st.RXcun,UART_RX_LEN ,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
  DMA_Cmd(Android_DMA_RX_CH, ENABLE);// 
		
  DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,  ENABLE); //��DMA�������ж�
	   
		 
  DMA_ClearFlag(DMA1_FLAG_GL2); 
  DMA_ClearFlag(DMA1_FLAG_GL3);     
	
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 


	memset((u8*)&UART3_android_st, FALSE,sizeof(UART3_android_st));//��־λ  ȫ��Ϊ FALSE
   
  UART3_android_st.ALL_tc_flg = TRUE;
  
}



void uart2_init(u32 bound)
{   
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA1���� 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA   ,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������
   
	  
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
  NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel7_IRQn; //һ��Ҫ�����ж�����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
	 
	 
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);// 
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//  
  USART_ITConfig(USART2,USART_IT_TC,DISABLE);  //���� ����ж�
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //����1�����ж�
  
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���4��DMA���� ����  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 

	
	MYDMA_Config( RS485_DMA_TX_CH,(u32)&USART2->DR,(u32)UART2_rs485_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
	DMA_Cmd(RS485_DMA_TX_CH, DISABLE);// û���ݷ���Ҫ����������һ�����ͻᷢ������
	MYDMA_Config( RS485_DMA_RX_CH,(u32)&USART2->DR,(u32)UART2_rs485_st.RXcun,UART_RX_LEN ,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1ͨ��4,����Ϊ����1,�洢��ΪUART1_SDBuff,����USART1_SD_LEN. 
	DMA_Cmd(RS485_DMA_RX_CH, ENABLE);// 

	DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,  ENABLE); //��DMA�������ж�


	DMA_ClearFlag(DMA1_FLAG_GL6); 
	DMA_ClearFlag(DMA1_FLAG_GL7);     

	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

	memset((u8*)&UART2_rs485_st, FALSE,sizeof(UART2_rs485_st));//��־λ  ȫ��Ϊ FALSE
   
  UART2_rs485_st.ALL_tc_flg=TRUE;	 
}


void disable_all_uart_interupt( void  )
{
	USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,DISABLE); 
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,DISABLE);      
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE);	     
}

void enable_all_uart_interupt( void  )
{
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);  
	DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  
	DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,ENABLE);   
	DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 
}
