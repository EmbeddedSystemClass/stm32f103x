#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"



Uart_struct    UART3_rs485_st; 
/*********************************************************************
 *  LOCAL  FUNC
 */ 
void   uart3_init(u32 bound);   
void   disable_all_uart_interupt(void);
void   enable_all_uart_interupt(void);

void uart3_init(u32 bound)
{   
 
   GPIO_InitTypeDef GPIO_InitStructure;
	 USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	 

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA1传输 

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3   ,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB   ,ENABLE);


		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB,&GPIO_InitStructure);

  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口
   
	  
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
  NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel2_IRQn; //一定要开启中断向量
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器 
	 
	 
  USART_ITConfig(USART3, USART_IT_TXE,  DISABLE);// 
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//  
  USART_ITConfig(USART3, USART_IT_TC,   DISABLE);  //传送 完成中断
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  //串口1空闲中断
  
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //使能串口4的DMA发送 接收  
  USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE); 
	
	
	
	
  MYDMA_Config( RS485_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
  DMA_Cmd(RS485_DMA_TX_CH, DISABLE);// 没数据发不要开启这条，一开启就会发出数据
  MYDMA_Config( RS485_DMA_RX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.RXcun,UART_RX_LEN ,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
  DMA_Cmd(RS485_DMA_RX_CH, ENABLE);// 
		
  DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,  ENABLE); //开DMA传送完中断
	   
		 
  DMA_ClearFlag(DMA1_FLAG_GL2); 
  DMA_ClearFlag(DMA1_FLAG_GL3);     
	
  USART_Cmd(USART3, ENABLE);                    //使能串口 



	memset((u8*)&UART3_rs485_st, FALSE,sizeof(UART3_rs485_st));//标志位  全部为 FALSE
   
  UART3_rs485_st.ALL_tc_flg=TRUE;
   
 
	 
}



  void disable_all_uart_interupt( void  )
{

         USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
         DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE);      
          
}
void enable_all_uart_interupt( void  )
{

        USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  
        DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE);   
         
}
