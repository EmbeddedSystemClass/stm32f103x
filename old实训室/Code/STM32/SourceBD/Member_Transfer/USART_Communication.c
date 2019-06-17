#include "USART_Communication.h"

void USART_Communication_485(void)//485ͨѶ������
{
			const u8 frameDatatrans_totle1 = 100;	//֡�����޳�
		u8 dataTrans_RXBU1[frameDatatrans_totle1] = {0};	//����֡����
		
			const u8 frameDatatrans_totlen2 = 100;	//֡�����޳�
		u8 dataTrans_RXBU2[frameDatatrans_totlen2] = {0};	//����֡����
		for(;;){

				DIR_485_1_Receive();
				DIR_485_2_Receive();				
				datsDev485_1_RX(dataTrans_RXBU1,frameDatatrans_totle1);	//����ָ������
				datsDev485_2_RX(dataTrans_RXBU2,frameDatatrans_totlen2);	//����ָ������
			  
				if(dataTrans_RXBU1[0]==0x0A)
				{
						DIR_485_1_Send();
						datsDev485_1_TX("OK\r\n", 4);
						delay_us(50000);
						dataTrans_RXBU1[0]=0;
						Relay_ON;
						Blue_LED_ON;					
				}				

				if(dataTrans_RXBU2[0]==0x0b)
				{
						DIR_485_2_Send();
						datsDev485_2_TX("OK\r\n", 4);
						delay_us(50000);
						dataTrans_RXBU2[0]=0;
						Relay_OFF;
						Blue_LED_OFF;					
				}		
		}
}
