/**
  ******************************************************************************
  * @file    GPIO_LED\main.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    25-February-2011
  * @brief   This file contains the main function for GPIO  example.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  * ����
  * �������������м��5S����
  *        
  * �ƹ����
  *     
  * LED        ��   ��    ��
  * 
  * ֹͣ       ��   ��    ��
  * ����       ��   ��    ��
  * �½�       ��   ��    ��
  * ����       �м�������5��
  * ����       ����������5��
  * �����ɹ�   ���к����5��
  * �����ɹ�   ����������5��
  */ 
    

    
/* Includes ------------------------------------------------------------------*/

#include "ALL_Includes.h"
    
//����CPU�ڲ�ʱ��
#define  SYS_CLOCK        16
#define  TIM4_PERIOD      124


u8 Data_Len = 0;

u8 uart_rx_flg = 0;

u8 curtain_status = 2;

u16 motor_ck_time = 0;

u16 LED_time = 0;

u16 alarm_time = 0;

u8 join_flg = 0;  //����״̬
 
uint32_t ad_ck_0_num = 0;

uint32_t MCU_UID[3];
 
u8 key_status = 2;
 
u8 sampling_in_flg = 0;   
    
u16 UART_NO_DAT_TIME = 0;
    
u16 sampling_time = 0;
u16 sampling_status = 0;
 
u8 program_num = 0;
 
u8 LIGHT_1_STATUS = 0;  
u8 LIGHT_2_STATUS = 0;  
u8 LIGHT_3_STATUS = 0; 

//
u8 maxcnt =0;

TRAN_D_struct TRAN_info1;


void Delay (uint16_t nCount);
 
void All_Config(void);
void IO_Init(void);
 
void TIM4_Config(void);

void jiance(void);
void jiance_light_key(void);

void  JDQ_WINDOW_PAUSE(void);
void  JDQ_WINDOW_UP(void);
void  JDQ_WINDOW_DOWN(void);    
    
u8  chcek_run_status(u8); 
u16 get_adc(u8 chanel,u8 times);

u8 chcek_status_pre = 0;

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/



/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{ 
  All_Config();

  TRAN_info1.dest_dev_num = 0;          //Ŀ���豸��

  TRAN_info1.dest_addr[0] = 0x00;       //Ŀ���ַ
  TRAN_info1.dest_addr[1] = 0x00;
  TRAN_info1.dest_addr[2] = 0x00;
  TRAN_info1.dest_addr[3] = 0x00;

  TRAN_info1.source_addr[0] = (u8) MCU_UID[0];          //Դ��ַ 
  TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
  TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
  TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24); 

  TRAN_info1.TYPE_NUM = 3    ;   //���ݸ�ʽ��: ���������ݸ�ʽ  ����ZIGBEE���Ǳ�׼��  �豸�� 
  
    
  while(1)
  {  
    if( program_num == 1 )
    {
      jiance(); 
    }
    else if( program_num == 2 || program_num == 3 )
    {
      jiance_light_key();
    }

    Rs485_COMM_uart_fuc();   
  }
}

void jiance_light_key(void)
{
	  u8 i = 0, dat[40];

	  dat[0] = 0; 
	  dat[2] = 0; //

	  dat[4] = (u8) MCU_UID[0];
	  dat[5] = (u8)( MCU_UID[0] >> 8 );
	  dat[6] = (u8)( MCU_UID[0] >> 16 );
	  dat[7] = (u8)( MCU_UID[0] >> 24 ); 

	  memset(  dat+8,0,8 ); 

	  if( LIGHT_1_STATUS == 0 )
	  {
		    KEY_LEFT_BLUE(1); 
		    //KEY_LEFT_RED(0);
		    KEY_LEFT_RED(0);  
		    //KEY_LEFT_BLUE(1);
	  }
	  else 
	  {
		    KEY_LEFT_BLUE(0); 
		    KEY_LEFT_RED(1);  
	  }
	  
	  if( program_num != 2 )
	  {
		    if( LIGHT_2_STATUS == 0 )
		    {
			      KEY_MID_BLUE(1); 
			      KEY_MID_RED(0); 
		    }
		    else 
		    {
			      KEY_MID_BLUE(0); 
			      KEY_MID_RED(1); 
		    }
	  }
	  else
	  {
		    KEY_MID_BLUE(0); 
		    KEY_MID_RED(0); 
	  }

	  if( LIGHT_3_STATUS == 0 )
	  {
		    KEY_RIGHT_BLUE(1); 
		    KEY_RIGHT_RED(0); 
		  }
	  else 
	  {
		    KEY_RIGHT_BLUE(0); 
		    KEY_RIGHT_RED(1);  
	  }
        
 /////////////////////////////////       
  if( KEY_1L == 0 )
  {  
	    alarm_time = 0;
	    while( KEY_1L == 0)
	    {
		      if(  KEY_3L == 0  )
		      {
		        //�ж�ʱ��//��ִ������ ����
		        if( alarm_time >= 3500 )
		        {   
			          //����5S�󣬴���һ������������

			          TRAN_info1.data_len = 2 ;

			          dat[0] = 0; 
				      if( join_flg == 0 )  //�������5S  �������ؿ����м�ĵ���5��
				          {   
					            join_flg = 1;
					            dat[1] = join_sw; // //��������
					            KEY_MID_BLUE(0); 
					            KEY_MID_RED(0); 
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 ); 
						              
						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(0); 
						              
						              LED_time = 300;
						              while( LED_time != 0 );
					            }
			          }
			          else    //�������5S  �������عأ��������ߵĵ���5��
			          {  
				            join_flg = 0;
				            dat[1] = leave_net; //

				            KEY_MID_BLUE(0); 
				            KEY_MID_RED(0);
				            
				            for(i=0;i<5;i++)
				            { 
					              KEY_LEFT_BLUE(1); 
					              KEY_LEFT_RED(1); 
					              
					              KEY_RIGHT_BLUE(1); 
					              KEY_RIGHT_RED(1); 
					              
					              
					              LED_time = 300;
					              while( LED_time != 0 ); 

					              KEY_LEFT_BLUE(0); 
					              KEY_LEFT_RED(0); 
					              
					              KEY_RIGHT_BLUE(0); 
					              KEY_RIGHT_RED(0);
					              

					              LED_time = 300;
					              while( LED_time != 0 );
				            }
				            }
				      Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
				      return;
				      }
		      }
		    }
	                           
	    if( LIGHT_1_STATUS != 0 )
	    {
		      LIGHT_1_STATUS = 0;
		      
		      KEY_LEFT_BLUE(1); 
		      KEY_LEFT_RED(0);  

		      RLY1_OUT(RLY_OFF) ;
	    }
	    else 
	    {  
		      LIGHT_1_STATUS = 1;
		      
		      KEY_LEFT_BLUE(0); 
		      KEY_LEFT_RED(1); 
		      
		      RLY1_OUT(RLY_ON) ;
	    }

	    dat[1] = upload_info;                

	    if( program_num == 2)
	    {

		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );

		      TRAN_info1.data_len = 16 + 3  ;

		      dat[3] = DTN_86_LIGHT_2; // 

		      dat[8+8] = 2; //1 2 3  ��ʾ·��

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_3_STATUS;  

	    } 
	    if( program_num == 3)
	    {

		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );

		      TRAN_info1.data_len = 16 + 4  ;

		      dat[3] = DTN_86_LIGHT_3; // 

		      dat[8+8] = 3; //1 2 3  ��ʾ·��

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_2_STATUS;  
		      dat[8+8+3] = LIGHT_3_STATUS; 
	    } 

	    Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	
  }		 
            
  if( KEY_2L == 0 && program_num == 3)
  { 
	    while( KEY_2L == 0);

	    if( LIGHT_2_STATUS != 0 )
	    {  
		      LIGHT_2_STATUS = 0;
		      
		      KEY_MID_BLUE(1); 
		      KEY_MID_RED(0);   
		      
		      RLY2_OUT(RLY_OFF);
	    }
	    else 
	    {  
		      LIGHT_2_STATUS = 1;
		      
		      KEY_MID_BLUE(0); 
		      KEY_MID_RED(1);
		      
		      RLY2_OUT(RLY_ON);
	    }
	    dat[1] = upload_info;    

	    TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );
	    TRAN_info1.data_len = 16 + 4  ;

	    dat[3] = DTN_86_LIGHT_3; // 

	    dat[8+8] = 3; //1 2 3  ��ʾ·��

	    dat[8+8+1] = LIGHT_1_STATUS;  
	    dat[8+8+2] = LIGHT_2_STATUS;  
	    dat[8+8+3] = LIGHT_3_STATUS; 

	    Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	
  }
	
  if( KEY_3L == 0 )
  {
	    while( KEY_3L == 0 )
	    {
		      if( KEY_1L == 0 )
		      {
		        	//�ж�ʱ��  ��ִ������ ����

			        if( alarm_time >= 3500 )
			        {   

				          //����5S�󣬴���һ������������

				          TRAN_info1.data_len = 2;

				          dat[0] = 0; 

				          if( join_flg == 0 )  //�������5S  �������ؿ����м�ĵ���5��
				          {   
					            join_flg = 1;

					            dat[1] = join_sw; //���� 

					            KEY_MID_BLUE(0); 
					            KEY_MID_RED(0);
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 ); 
						              
						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(1); 
						              
						              LED_time = 300;
						              while( LED_time != 0 );
				            	}  
				          }
				          else    //�������5S  �������عأ��������ߵĵ���5��
				          {   
					            join_flg = 0;
					            dat[1] = leave_net; //����

					            KEY_MID_BLUE(1); 
					            KEY_MID_RED(1);
					            
					            for(i=0;i<5;i++)
					            { 
						              KEY_LEFT_BLUE(1); 
						              KEY_LEFT_RED(1); 
						              
						              KEY_RIGHT_BLUE(1); 
						              KEY_RIGHT_RED(1);  
						              
						              LED_time = 300;
						              while( LED_time != 0 ); 

						              KEY_LEFT_BLUE(0); 
						              KEY_LEFT_RED(0); 
						              
						              KEY_RIGHT_BLUE(0); 
						              KEY_RIGHT_RED(0); 

						              LED_time = 300;
						              while( LED_time != 0 );
					            }
				          }
				          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
				          return; 
			        }  
		      }  
	    }        
	               
	    if( LIGHT_3_STATUS != 0 )
	    {  
		      LIGHT_3_STATUS = 0;
		      
		      KEY_RIGHT_BLUE(1); 
		      KEY_RIGHT_RED(0);   //�ұ�������
		      
		      RLY3_OUT(RLY_OFF);
	    }
	    else 
	    {  
		      LIGHT_3_STATUS = 1;
		      
		      KEY_RIGHT_BLUE(0); 
		      KEY_RIGHT_RED(1);  //�ұߺ����
		      
		      RLY3_OUT(RLY_ON);
	    } 
	    
	    dat[1] = upload_info;  

	    if(program_num == 2)
	    {
		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );
		      TRAN_info1.data_len = 16 + 3  ;

		      dat[3] = DTN_86_LIGHT_2; // 

		      dat[8+8] = 2; //1 2 3  ��ʾ·��

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_3_STATUS;  
	    } 
	    if( program_num == 3 )
	    {
		      TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );
		      TRAN_info1.data_len = 16 + 4;

		      dat[3] = DTN_86_LIGHT_3; // 

		      dat[8+8] = 3; //1 2 3  ��ʾ·��

		      dat[8+8+1] = LIGHT_1_STATUS;  
		      dat[8+8+2] = LIGHT_2_STATUS;  
		      dat[8+8+3] = LIGHT_3_STATUS; 
	    } 
	  
	  	Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );	  
  } 
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
 void TIM4_Config(void)
{

}


void All_Config(void)
{
 
  CLK->CKDIVR &= ~( BIT(4) | BIT(3) );


  IO_Init();  
  UART1_DeInit();
  UART1_Init( (u32)115200,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE );
  UART1_ITConfig( UART1_IT_RXNE_OR, ENABLE );
  UART1_Cmd( ENABLE );

  //TIM4_Config( );       //��һ����ʱ��
  /* TIM4 configuration:
  - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
  clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
  max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
  min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
  so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */



  TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
  /* Set the Autoreload value */
  TIM4->ARR = (uint8_t)(TIM4_PERIOD);
  TIM4->SR1 = (uint8_t)(~TIM4_FLAG_UPDATE);
  TIM4->IER |= (uint8_t)TIM4_IT_UPDATE;
  TIM4->CR1 |= TIM4_CR1_CEN;

  /* Time base configuration */
  // TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  // TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  //  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* enable interrupts */
  //enableInterrupts();

  /* Enable TIM4 */
  //TIM4_Cmd(ENABLE);

  enableInterrupts();
}


void  JDQ_WINDOW_UP(void)
{  
  RLY1_OUT(RLY_OFF);   
  RLY2_OUT(RLY_ON);    
}

void  JDQ_WINDOW_DOWN(void)
{   
  RLY1_OUT(RLY_ON); 
  RLY2_OUT(RLY_OFF);    
}

void  JDQ_WINDOW_PAUSE(void)
{ 
  RLY1_OUT(RLY_OFF);
  RLY2_OUT(RLY_OFF);    
}


void jiance(void)
{
	  u8 i = 0;
	  u8 re = 0;
	  u8 dat[50];

	  dat[0] = 0; 
	  dat[1] = upload_info; //

	  dat[2] = 0;  // 
	  dat[3] = 51; //   �豸���ͺ� ����ȥ��
	  
	  dat[4] = (u8) MCU_UID[0];     
	  dat[5] = (u8)( MCU_UID[0] >> 8 );
	  dat[6] = (u8)( MCU_UID[0] >> 16 );
	  dat[7] = (u8)( MCU_UID[0] >> 24 ); 

	  memset( dat + 8,0,8 );

	  TRAN_info1.source_dev_num = ( DTN_curtain << 8 | DTN_curtain >> 8 );
		

		//check run status
	
	  re =chcek_run_status(curtain_status);
		
			

		if( re == 2 )
		{
			      if(curtain_status != 2)
			      {
				        TRAN_info1.data_len = 17 ;
				        
				        dat[1] = upload_info; //
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   �豸��

				        dat[8+8] = 2; // 1���ֽڣ�1�� 0�£�2��ͣ
				        
				        curtain_status = 2;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  //��������
			        	// delay_ms(1000);
			      }
			           
			      LED_PAUSE();

			      JDQ_WINDOW_PAUSE();
				key_status = 2;
				
			      if( curtain_status != 2 )
			      {
				        TRAN_info1.data_len = 17   ;
				        dat[8+8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ
				        curtain_status = 2;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 
				        // delay_ms(1000);
			      }
		}

	  
//	  if( SAMPING_PD2_Pin_RD != 0 )  //�������50MS  ˵����ֹͣ���е�
//	  { 
//		    if( sampling_time > 300 ) 
//		    {   
//			      sampling_time = 300;   
//			      sampling_in_flg = 0;  //ֹͣ  
//		    } 
//	  }
//	  else
//	  {
//		    sampling_time = 0;
//		    sampling_in_flg = 1;
//		    motor_ck_time = 500;
//	  }  
//
//	  if( motor_ck_time == 0 )
//	  {   
//		    if( sampling_in_flg == 0 )
//		    {
//			      if(curtain_status != 2)
//			      {
//				        TRAN_info1.data_len = 17 ;
//				        
//				        dat[1] = upload_info; //
//				        dat[2] = 0;  // 
//				        dat[3] = DTN_curtain; //   �豸��
//
//				        dat[8+8] = 2; // 1���ֽڣ�1�� 0�£�2��ͣ
//				        
//				        curtain_status = 2;
//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  //��������
//			        	// delay_ms(1000);
//			      }
//			           
//			      LED_PAUSE();
//
//			      JDQ_WINDOW_PAUSE();
//
//			      if( curtain_status != 2 )
//			      {
//				        TRAN_info1.data_len = 17   ;
//				        dat[8+8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ
//				        curtain_status = 2;
//				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 
//				        // delay_ms(1000);
//			      }
//		    }
//	  }              
//	        
	  if( DOWN_IN == 0 )
	  {      
		    Delay(5000);
		    
		    if( DOWN_IN == 0 )
		    {
			      alarm_time = 0;
			      motor_ck_time = 1500;
			 
			      sampling_time = 0;
			     
			      LED_DOWN();

			      JDQ_WINDOW_DOWN();

			      if( key_status != 0 )
			      {
				        TRAN_info1.data_len = 17   ;
				        
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   �豸��
				        
				        dat[8+8] = 0; //1���ֽڣ�1�� 0�£�2��ͣ
				        curtain_status = 0;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 		
			      }
			      key_status = 0; //down
	    	}
	  }		 
	  else if( PAUSE_IN == 0 )
	  { 
	    	Delay(5000);

		    if( PAUSE_IN == 0 )
		    {
		      
			      LED_PAUSE();
			      
			      JDQ_WINDOW_PAUSE(); 

			      if( alarm_time >= 3500 )   //����5S�󣬴���һ������������
			      {   
				        TRAN_info1.data_len = 2   ;

				        dat[0] = 0; 
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   �豸��

				        if( join_flg == 0 )  //�������5S  �������ؿ����м�ĵ���5��
				        {   
					          join_flg = 1;

					          dat[1] = join_sw; // ��������
					          
					         //LED_JOIN();
					          for(i=0;i<5;i++)
					          { 
						            KEY_MID_RED(1);
						            KEY_MID_BLUE(0);

						            LED_time = 300;
						            while( LED_time != 0 ); 

						            KEY_MID_RED(0);
						            KEY_MID_BLUE(1);

						            LED_time = 300;
						            while( LED_time != 0 );
					          }

				        }
				        else    //�������5S  �������عأ��������ߵĵ���5��
				        {  
					          join_flg = 0;
					          dat[1] = leave_net; //����

					         // LED_JOIN(join_flg);
					          
					          for(i=0;i<5;i++)
					          { 
						            KEY_LEFT_RED(0);
						            KEY_MID_RED(0);
						            KEY_RIGHT_RED(0);

						            KEY_LEFT_BLUE(1);
						            KEY_MID_BLUE(1);
						            KEY_RIGHT_BLUE(1);

						            LED_time = 300;
						            while( LED_time != 0 ); 

						            KEY_LEFT_RED(1); 
						            KEY_MID_RED(0);  
						            KEY_RIGHT_RED(1);

						            KEY_LEFT_BLUE(0);  
						            KEY_MID_BLUE(1);
						            KEY_RIGHT_BLUE(0);

						            LED_time = 300;
						            while( LED_time != 0 );
					          }
				        }

				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  

				        alarm_time = 0;
			      }
			      else
			      {
				        if( key_status != 2 )
				        {
					          TRAN_info1.data_len = 17;
					          
					          dat[2] = 0;  // 
					          dat[3] = DTN_curtain; //   �豸��
					          
					          dat[8+8] = 2; //1���ֽڣ�1�� 0�£�2��ͣ
					          
					          curtain_status = 2;
					          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 	 
				        } 
				        key_status = 2;
			      }
		    }
	  }	
	  else if( UP_IN == 0 )
	  {
		    Delay(5000);
		    if( UP_IN == 0 )
		    {
			      alarm_time = 0;
			      motor_ck_time = 1500;

			      sampling_time = 0;
			     
			      LED_UP(); //������״̬

			      JDQ_WINDOW_UP();

			      if(key_status != 1)
			      {
				        TRAN_info1.data_len = 17   ;
				        
				        dat[2] = 0;  // 
				        dat[3] = DTN_curtain; //   �豸��
				        
				        dat[8+8] = 1; //1���ֽڣ�1�� 0�£�2��ͣ
				        curtain_status = 1;
				        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len ); 	
			      }

			      key_status = 1; //up
		    }
	  }
	  else
	  {
	    	alarm_time = 0;
	  } 
 }

u8 chcek_run_status(u8 status)
{
	
	if(status != 2)
	{
		delay_ms(20);
		
		if(chcek_run != chcek_status_pre)
		{
			chcek_status_pre = chcek_run;
			delay_ms(11);
			if(chcek_run != chcek_status_pre)
			{
				maxcnt = (maxcnt < 1) ? 0:(maxcnt-1);
			}
		}
		else if(chcek_run == chcek_status_pre)
		{
				
			
			delay_ms(11);
			if(chcek_run == chcek_status_pre)
			{
				maxcnt++;
			}
			else
			{
				maxcnt = (maxcnt < 1) ? 0:(maxcnt-1);
				chcek_status_pre = chcek_run;
			}
			
			if(maxcnt >25)
			{
				maxcnt = 0;
				
				return 2;
			}
		}

		
	}
		
	return status;
}
void IO_Init(void)
{   
  uint16_t i = 0, Conversion_Value1 = 0 ;
  u8 val1,val2;

 //����IO��Ϊ���

  GPIOC->DDR |= RLY3_PC7_Pin;//���ģʽ
  GPIOC->CR1 |= RLY3_PC7_Pin;//�������
   
  GPIOC->DDR |= RLY2_PC6_Pin;//���ģʽ
  GPIOC->CR1 |= RLY2_PC6_Pin;//������� 

  GPIOA->DDR |= RLY1_PA3_Pin;//���ģʽ
  GPIOA->CR1 |= RLY1_PA3_Pin;//������� 
   
  GPIOC->DDR &=~ KEY3_PC3_Pin;//����ģʽ
  GPIOC->DDR &=~ KEY1_PC4_Pin;//����ģʽ 
  GPIOC->DDR &=~ KEY2_PC5_Pin;//����ģʽ


  GPIOD->DDR &=~ SAMPING_PD2_Pin;//����ģʽ

  GPIOD->DDR &=~ MODE_AD_PD3_Pin;//����ģʽ       

   
  GPIOD->DDR |= LED1_PD4_Pin;//���ģʽ
  GPIOD->CR1 |= LED1_PD4_Pin;//�������
   
  GPIOB->DDR |= LED5_PB4_Pin;//���ģʽ
  GPIOB->CR1 |= LED5_PB4_Pin;//�������         

  GPIOB->DDR |= LED3_PB5_Pin;//���ģʽ
  GPIOB->CR1 |= LED3_PB5_Pin;//�������    


  GPIOD->DDR |= LED2_SWIM_PD1_Pin;//���ģʽ
  GPIOD->CR1 |= LED2_SWIM_PD1_Pin;//�������
   
  GPIOA->DDR |= LED4_PA1_Pin;//���ģʽ
  GPIOA->CR1 |= LED4_PA1_Pin;//�������         

  GPIOA->DDR |= LED6_PA2_Pin;//���ģʽ
  GPIOA->CR1 |= LED6_PA2_Pin;//�������            

   
  //����
  GPIOD->DDR |= BIT(5);//���ģʽ
  GPIOD->CR1 |= BIT(5);//�������
    
   //����
  GPIOD->DDR &=~ BIT(6);//����ģʽ   

  RLY1_OUT(RLY_OFF) ;
  RLY2_OUT(RLY_OFF) ;    
  RLY3_OUT(RLY_OFF) ; 

  Delay(5000);
  Delay(5000);
  Delay(5000);

  delay_ms(3000);
    

  //��������׳���Ļ�� ���ǿ��ص�   Ȼ���ʼ����ͬ
    
 
   // ADC1->CR1  =0x00;    // fADC = fMASTER/2����λ������͹���ģʽ

  ADC1->CR2  = 0x00;    // Ĭ�������

  ADC1->CSR  = 0x04;     //4ͨ��

  ADC1->CR1 |= 0x01;    // ����ADC
  i = 6;
  while(i--);
  ADC1->CR1 |= 0x01;    // �ӵ͹���ģʽ�л���ADC

  while(!(ADC1->CSR & 0x80));    // �ȴ�ת�����

  val2 = ADC1->DRH;
  val1 = ADC1->DRL;
  Conversion_Value1 =((((unsigned int) val2 ) << 2 ) + val1 );    // �������AD_Value������
  
  
  // ���ݶ�ȡ����ADCֵ ���ð�����Ĺ���
  // 1��Ļ��    2�� 2·�ƹ����  3��3·�ƹ����  4����·�ɵ���  
  program_num = 1;//Ļ������
  
  if( Conversion_Value1 > 180 && Conversion_Value1 < 300 )//1·��ֵ
  {
    program_num = 2;//2·�ƹ����
  }
  else if( Conversion_Value1 > 400 && Conversion_Value1 < 600 )//2·��ֵ
  {
    program_num = 3;//3·�ƹ����
  }    
  else if( Conversion_Value1 > 680  && Conversion_Value1 < 850 )//3·��ֵ  
  {
    program_num = 4;//��·�ɵ���
  }
     
  //����I/O�ڳ�ʼ��
  switch( program_num )
  {
    case 1://Ĭ����Ļ������
    {
      KEY_LEFT_RED(1);
      KEY_MID_RED(0);
      KEY_RIGHT_RED(1);

      KEY_LEFT_BLUE(0);
      KEY_MID_BLUE(1);
      KEY_RIGHT_BLUE(0);
    }break;
    
    case 2://2·86�����
    {
      KEY_LEFT_BLUE(1);    
      KEY_MID_BLUE(1);   
      KEY_RIGHT_BLUE(0);  

      KEY_LEFT_RED(0);    
      KEY_MID_RED(1);     
      KEY_RIGHT_RED(1);  
    }break; 
    
    case 3://3·86�����
    {
      KEY_LEFT_BLUE(1);    
      KEY_MID_BLUE(0);   
      KEY_RIGHT_BLUE(0);  

      KEY_LEFT_RED(0);    
      KEY_MID_RED(1);     
      KEY_RIGHT_RED(1);     
    }break;
    
    default: break; 
  }     
}

u16 get_adc(u8 chanel,u8 times)
{

	u16 val1,val2,i,n,ad_value;
	// ADC1->CR1  =0x00;    // fADC = fMASTER/2����λ������͹���ģʽ


	for (n = 0; n < times; ++n)
	{
			  ADC1->CR2  = 0x00;    // Ĭ�������

			  ADC1->CSR  = chanel;     //chanelͨ��

			  ADC1->CR1 |= 0x01;    // ����ADC
			  i = 6;
			  while(i--);
			  ADC1->CR1 |= 0x01;    // �ӵ͹���ģʽ�л���ADC

			  while(!(ADC1->CSR & 0x80));    // �ȴ�ת�����

			  val2 = ADC1->DRH;
			  val1 = ADC1->DRL;

			  ad_value += ((((unsigned short) val2 ) << 2 ) + val1 );

			  delay_ms(2);
	}

	 ADC1->CR1  =0x00;    // fADC = fMASTER/2����λ������͹���ģʽ
	return ad_value/times;
	 
}

/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/**
  * ������ʱ
  *
  *
  *
  */

 void delay_ms(uint16_t nCount)
{  
  u16 i = 0, n = 0;
 
  for(i=0;i<nCount;i++)
  {  
    n = 1000;
    while ( n != 0 )
    {
      n--;
    }
  }
}
 


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
