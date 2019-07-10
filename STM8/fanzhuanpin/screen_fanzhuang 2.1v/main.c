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
  */ 

/* Includes ------------------------------------------------------------------*/



#include "ALL_Includes.h"
//����CPU�ڲ�ʱ��
#define  SYS_CLOCK        16
#define  TIM4_PERIOD      124


u8    Data_Len = 0;

u8    uart_rx_flg = 0;

u16   UART_NO_DAT_TIME = 0;

u8    dat_595 = 0;

u8    motor_status = 0; 
u8    screen_status = 0; 

u8    screen_sw_mode = 0;

u16   screen_relay_off_time = 0;

u16   screen_relay_on_time = 0;

u16   screen_on_time = 0;

u8    start_screen_on_tim_flg = 0;

u16   CLAMP_CK_TIM = 0;

u8    flash_led_power = 0;

u8    motor_limit = 0;

u8    motor_running_status = 0;

u16   JDQ_TIME = 0;

u8   Key_c = Key_None; //����״̬

u8   UART_RX_CMD = UART_RX_CMD_None; //





void Delay (uint16_t nCount);

void CLOCK_Config(u8 SYS_CLK);
void All_Congfig(void);
void IO_Init(void);
void write_595( u8  *dat,int  len );
void UART1_Config(void);
 
void TIM4_Config(void);

u8 KeyScan(void);
void jiance(void);
 
void  RAM_INIT( void );

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  TRAN_D_struct   TRAN_info1 ;
  u8 dat[UART_RX_LEN];

  u8 motor_run_status_cun = 0;      //���״̬
  u8 screen_status_cun = 0;         //��ʾ��״̬
  u8 ab = 0;

  All_Congfig();
    
 
  while (1)
  { 
    if( motor_run_status_cun != motor_running_status || screen_status_cun != screen_status )  //״̬�ı� ����λ������״̬
    {  
      TRAN_info1.dest_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

      TRAN_info1.dest_addr[0] = 0x00;
      TRAN_info1.dest_addr[1] = 0x00;
      TRAN_info1.dest_addr[2] = 0x00;
      TRAN_info1.dest_addr[3] = 0x00;

      TRAN_info1.source_dev_num = ( DTN_JCZX_fanzhuang << 8 | DTN_JCZX_fanzhuang >> 8 );

      TRAN_info1.source_addr[0] = 0;
      TRAN_info1.source_addr[1] = 0;
      TRAN_info1.source_addr[2] = 0;
      TRAN_info1.source_addr[3] = 0;

      TRAN_info1.TYPE_NUM = 4;   //��4��
      TRAN_info1.data_len = 4; 

      dat[0] = 0;
      dat[1] = 0x0a;
      dat[2] = screen_status;
      dat[3] = motor_running_status;


      if(send_urt_time == 0)
      {
        Rs485_COMM_SD_load_buf( 0xAAAA  ,0xBBBB ,&TRAN_info1 , dat , TRAN_info1.data_len );  //���͸���λ��

        ab =! ab;
        RS485_LED_PA2_Pin_OUT(ab);  //����LED�� ��ת

        motor_run_status_cun = motor_running_status; //����״̬
        screen_status_cun = screen_status;
      }  
    }
        
    Key_c = KeyScan();  //�жϰ���״̬
   
    Rs485_COMM_uart_fuc(); //���Ӵ���
    
    jiance();
     
    switch( screen_sw_mode )
    { 
      case 0:


      break;
      
      case screen_on://ִ����Ļ������
      {
        if(  screen_relay_on_time == 0 && start_screen_on_tim_flg == 0 )//ִ�д���������ʼ5S ����Ƿ����ɹ�
        {
          dat_595 |=SCREEN_ON_595Pin; //0  �ǿ�
          write_595( &dat_595 ,1 );
          Delay (100);
          dat_595 &= ~ SCREEN_ON_595Pin; //1  �� 
          write_595( &dat_595 ,1 );

          screen_on_time = 5000;
          start_screen_on_tim_flg = 1;
        }
     
        if( start_screen_on_tim_flg )
        {
          if( screen_on_time == 0 )
          { 
            if( SCREEN_LED_R_PC6_Pin_RD != 0 && SCREEN_LED_G_PC7_Pin_RD != 0 )//���Ǹߵ�ƽ��ʾ����
            {
              screen_relay_on_time = 200; //���ſ�
              start_screen_on_tim_flg = 0;
            } 
          } 
        }   
            
        if( SCREEN_LED_R_PC6_Pin_RD == 0 || SCREEN_LED_G_PC7_Pin_RD == 0 )//������һ������0����ʾ��ʾ���Ѿ��������
        {
          screen_on_time = 0;
          screen_sw_mode = 0;
          flash_led_power = 0;
          screen_status = screen_on;
          //�ϴ����ϼ��豸 ��Ϣ
        } 
            
        if(flash_led_power)
        {
          dat_595 |= LED1_595Pin; // 
          write_595( &dat_595 ,1 ); 
        }
        else
        {
          dat_595 &= ~ LED1_595Pin; // 
          write_595( &dat_595 ,1 ); 
        }

      } break;
      
      
      case screen_off://ִ����Ļ�ز���
      {
        if( screen_relay_off_time == 0 )
        {
          //�����̵���
          dat_595 |=RLY1_595Pin; 
          dat_595 |=  LED1_595Pin; // �ص�
          write_595( &dat_595 ,1 ); 

          if( SCREEN_LED_R_PC6_Pin_RD != 0 && SCREEN_LED_G_PC7_Pin_RD != 0 )// 
          {
            screen_sw_mode = 0;
            screen_status = screen_off;
            //�ϴ����ϼ��豸 ��Ϣ
          }  
        }
      }break;
      
      default:
      break;
    }
  }
}



void UART1_Config(void)
{
  UART1_DeInit();
  UART1_Init((u32)57600,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
 void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}


void All_Congfig(void)
{
  CLOCK_Config(SYS_CLOCK);//ϵͳʱ�ӳ�ʼ��  
  RAM_INIT();
  IO_Init();  
  UART1_Config();
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
  TIM4_Config();       //��һ����ʱ��
  enableInterrupts();
}



void  RAM_INIT( void )
{
  dat_595 = 0;
  motor_running_status = motor_stop;
  motor_status = motor_stop; 
  screen_status = screen_off; 

  screen_sw_mode = 0;
}

//����ɨ��

u8 KeyScan(void)
{
  if( KEY1_PA3_Pin_RD == 0 &&  KEY2_PB5_Pin_RD == 0 &&  KEY3_PB4_Pin_RD == 0 )
  {
    Delay(10000); 
    if( KEY1_PA3_Pin_RD == 0 &&  KEY2_PB5_Pin_RD == 0 &&  KEY3_PB4_Pin_RD == 0 )
    {
      if( KEY1_PA3_Pin_RD == 0 ) return Key_Screen;
      if( KEY2_PB5_Pin_RD == 0 ) return Key_Up;
      if( KEY3_PB4_Pin_RD == 0 ) return Key_Down;
    }
    else
    {
      return Key_None;
    }
  }
  else
  {
    return Key_None;
  }
}

//������Ļ����
void contrl_motor_up(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 

  write_595( &dat_595 ,1 );
  Delay(50000); 
  Delay(50000); 
  Delay(50000); 
  
  dat_595 &=~RLY2_595Pin;
  dat_595 |= RLY3_595Pin;   //Up  RLY3 = 1 ����

  dat_595 &=~LED2_595Pin; // 
  dat_595 |= LED3_595Pin; //

  write_595( &dat_595 ,1 );

  motor_status = motor_up; 
  motor_running_status = motor_up;
}


//������Ļ�½�
void contrl_motor_down(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 

  write_595( &dat_595 ,1 );
  Delay(50000); 
  Delay(50000); 
  Delay(50000); 

  dat_595 |= RLY2_595Pin;   //  RLY2 = 1 ����
  dat_595 &=~RLY3_595Pin;

  dat_595 |= LED2_595Pin; //
  dat_595 &=~LED3_595Pin; // 

  write_595( &dat_595 ,1 );

  motor_status = motor_down; 
  motor_running_status = motor_down;
}

//������Ļֹͣ
void contrl_motor_stop(void)
{
  dat_595 |=RLY2_595Pin;  
  dat_595 |=RLY3_595Pin; 
  
  dat_595 |=  LED2_595Pin; 
  dat_595 |=  LED3_595Pin;
  
  write_595( &dat_595 ,1 );
  
  motor_status = motor_stop;  
  motor_running_status = motor_stop; 
}


void jiance(void)
{
  //�����ػ���
  if( KEY1_PA3_Pin_RD == 0 )
  {  
    Delay(10000);  
    if( KEY1_PA3_Pin_RD == 0 )
    {
      JDQ_TIME = 5000;
      while( KEY1_PA3_Pin_RD == 0 )
      {    
        Rs485_COMM_uart_fuc(); 
        Delay (100); 
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      //������һ������0����ʾ��ʾ���Ѿ��������
          /*
      �����ʾ��ԭ��״̬�ǿ���״̬����ر���ʾ���������ʾ���ǹػ�״̬������ʾ������ʾ������������LED1ָʾ����˸��
      ������ɣ�LED1��������ʾ���ػ�LED1�� 
          */ 
      if( screen_status == screen_off )//ִ�����صĲ���
      {
        screen_sw_mode = screen_on;

        dat_595 &=~ RLY1_595Pin; //ȡ��
        write_595( &dat_595 ,1 ); 

        screen_relay_on_time = 1000;//2S ����
      }
         
      if( screen_status == screen_on )//ִ�������Ĳ���
      {
        screen_sw_mode = screen_off; 
        //�ر��� 
        dat_595 |= SCREEN_ON_595Pin; //0  �ǿ�
        write_595( &dat_595 ,1 );
        Delay(100);
        dat_595 &= ~ SCREEN_ON_595Pin; //1  �� 
        write_595( &dat_595 ,1 );

        //�ȴ�3s ��ϵ�
        screen_relay_off_time = 1000;//3S�� �̵�����
      }
    } 
  }
  
  
  //������
  if( KEY2_PB5_Pin_RD == 0 )
  {    
    Delay (10000);  
    if( KEY2_PB5_Pin_RD == 0 )   
    {
      JDQ_TIME = 5000;
      while( KEY2_PB5_Pin_RD == 0 )
      { 
        Rs485_COMM_uart_fuc();
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      if( motor_status == motor_up )  //�������������ٰ�����������ֹͣ
      {
        dat_595 |=RLY2_595Pin;  //Down
        dat_595 |=RLY3_595Pin;  //Up

        dat_595 |= LED2_595Pin; // 
        dat_595 |= LED3_595Pin; // 

        write_595( &dat_595 ,1 );
        
        motor_status = motor_stop;  
        motor_running_status = motor_stop;
        
      }
      else 
      {
        if( UPLIMIT_PC4_Pin_RD == 0 )  //����λ���ٰ����� ��Ч(δ��λ)
        {
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          Delay(50000); 
          Delay(50000); 
          Delay(50000); 
          
          dat_595 |= RLY3_595Pin;   //Up  RLY3 = 1 ����
          dat_595 &=~RLY2_595Pin;
          
          dat_595 &=~LED2_595Pin; // 
          dat_595 |= LED3_595Pin; //
          
          write_595( &dat_595 ,1 );

          motor_status = motor_up; 
          motor_running_status = motor_up;
        }
      } 
    }
  } //������
  
  
  
  //�½���
  if( KEY3_PB4_Pin_RD == 0 )
  {    
    Delay (10000); 
    if( KEY3_PB4_Pin_RD == 0 )   //ȥ��
    { 
      JDQ_TIME = 5000;
      while( KEY3_PB4_Pin_RD == 0 )  //
      {    
        Rs485_COMM_uart_fuc();  
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }; 

      if( motor_status == motor_down )  //���½��������ٰ��½�������ֹͣ
      {
        dat_595 |=RLY2_595Pin;   //Down
        dat_595 |=RLY3_595Pin;   //Up

        dat_595 |= LED2_595Pin;    // 
        dat_595 |= LED3_595Pin;    // 
        
        write_595( &dat_595 ,1 );  
        
        motor_status = motor_stop;  
        motor_running_status = motor_stop;
        

      }
      else 
      {
        if( DNLIMIT_PC3_Pin_RD == 0 )  //����λ���ٰ����� ��Ч��δ��λ��
        {    
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          
          Delay(50000); 
          Delay(50000); 
          Delay(50000); 
          
          dat_595 |= RLY2_595Pin; 
          dat_595 &=~RLY3_595Pin; 
          
          dat_595 &=~LED3_595Pin; // 
          dat_595 |= LED2_595Pin; //
          
          write_595( &dat_595 ,1 );

          motor_status = motor_down;
          motor_running_status = motor_down;
        }
      }  
    }  
  } //�½���

    
  //����λ   
  if( DNLIMIT_PC3_Pin_RD != 0 )
  {    
    if( motor_status == motor_up )
    {   
      JDQ_TIME = 5000;
      while( DNLIMIT_PC3_Pin_RD != 0 )
      {   
        Rs485_COMM_uart_fuc(); 
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }
    }
    else   //ֹͣ
    {
      dat_595 |=RLY2_595Pin;  
      dat_595 |=RLY3_595Pin; //���Ƽ̵���
      
      dat_595 |=  LED2_595Pin; // 
      dat_595 |=  LED3_595Pin; // ����ָʾ��
      
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      motor_status = motor_stop; 

      motor_running_status = motor_limit_down;
    } 
  }  //����λ 
  
  //����λ
  if( UPLIMIT_PC4_Pin_RD != 0 )
  {    
    if( motor_status == motor_down )
    {     
      JDQ_TIME = 5000;
      while( UPLIMIT_PC4_Pin_RD != 0 )
      {   
        Rs485_COMM_uart_fuc(); 
        Delay (100);
        if( JDQ_TIME == 0 )
        {
          break;
        }
      }
    }
    else 
    {
      dat_595 |=RLY2_595Pin;  
      dat_595 |=RLY3_595Pin; 
      dat_595 |=  LED2_595Pin; // 
      dat_595 |=  LED3_595Pin; // 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      write_595( &dat_595 ,1 );
      Delay(5000); 
      motor_status = motor_stop;   

      motor_running_status = motor_limit_up;
    }
  }//����λ

     
  //����
  if( CLAMP_PC5_Pin_RD != 0 )
  {
    Delay (500);  
    if( CLAMP_PC5_Pin_RD != 0 )   
    {
      if( CLAMP_CK_TIM == 0 )
      {

      /*
      if(  motor_status==motor_up  )//����  �ϱ��£��±���
      {

      dat_595&=~RLY2_595Pin;  
      dat_595&=~RLY3_595Pin; 
      write_595( &dat_595 ,1 );
      Delay (50000); 
      Delay (50000); 
      Delay (50000); 
      dat_595 |= RLY2_595Pin; 
      dat_595&=~RLY3_595Pin; 

      dat_595&=~LED3_595Pin; // 
      dat_595|= LED2_595Pin; // 
      write_595( &dat_595 ,1 );


      motor_status=motor_down; 
      motor_running_status= motor_down;
      //3s���������
      CLAMP_CK_TIM=2500;
      }
      else
      */

        if( motor_status == motor_down )
        {
          dat_595 |=RLY2_595Pin;  
          dat_595 |=RLY3_595Pin; 
          
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(50000); 
          Delay(50000); 
          Delay(50000);  
          
          dat_595 |= RLY3_595Pin; 
          dat_595 &= ~RLY2_595Pin;

          dat_595 &= ~LED2_595Pin; // 
          dat_595 |= LED3_595Pin; // 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );
          Delay(5000); 
          write_595( &dat_595 ,1 );

          motor_status = motor_up;
          motor_running_status = motor_up;
          //3s���������
          CLAMP_CK_TIM = 2500;
        }
        else 
        {
        
        }
      }
    }
  } //���� 

  //SCREEN_LED_R ��SCREEN_LED_G ��һ���ǵ͵�ƽ���ǿ�����

}


void IO_Init(void)
{
  //����IO��Ϊ���
  GPIOA->DDR |= RS485_LED_PA2_Pin;//���ģʽ
  GPIOA->CR1 |= RS485_LED_PA2_Pin;//�������

  GPIOD->DDR |= RS485_DIR_PD4_Pin;//���ģʽ
  GPIOD->CR1 |= RS485_DIR_PD4_Pin;//�������


  GPIOD->DDR |= H595_DS_PD2_Pin;//���ģʽ
  GPIOD->CR1 |= H595_DS_PD2_Pin;//������� 

  GPIOD->DDR |= H595_ST_PD3_Pin;//���ģʽ
  GPIOD->CR1 |= H595_ST_PD3_Pin;//�������   

  GPIOA->DDR |= H595_SH_PA1_Pin;//���ģʽ
  GPIOA->CR1 |= H595_SH_PA1_Pin;//������� 


  GPIOC->DDR &=~ SCREEN_LED_R_PC6_Pin;//����ģʽ
  GPIOC->DDR &=~ SCREEN_LED_G_PC7_Pin;//����ģʽ

  GPIOA->DDR &=~ KEY1_PA3_Pin;//����ģʽ
  GPIOB->DDR &=~ KEY2_PB5_Pin;//����ģʽ
  GPIOB->DDR &=~ KEY3_PB4_Pin;//����ģʽ


  GPIOC->DDR &=~ DNLIMIT_PC3_Pin;//����ģʽ
  GPIOC->DDR &=~ UPLIMIT_PC4_Pin;//����ģʽ
  GPIOC->DDR &=~ CLAMP_PC5_Pin;//����ģʽ

  /* �õ�595 
  #define  LED_SCREEN_Pin   (1 << 3) 
  #define  LED_UP_Pin       (1 << 3) 
  #define  LED_DOWN_Pin     (1 << 3) 
  */

  RS485_DIR_PD4_Pin_OUT(0);  //0�ǽ��� 
  RS485_LED_PA2_Pin_OUT(1);


  H595_DS_PD2_Pin_OUT(1);     
  H595_ST_PD3_Pin_OUT(1);     
  H595_SH_PA1_Pin_OUT(1);  


  dat_595 &=~ SCREEN_ON_595Pin;//1 �ǹ�
  dat_595 |= RLY1_595Pin;
  dat_595 |= RLY2_595Pin;
  dat_595 |= RLY3_595Pin;

  dat_595 |=  LED1_595Pin;
  dat_595 |=  LED2_595Pin;
  dat_595 |=  LED3_595Pin; 

  write_595( &dat_595 ,1 );
}
 
/*********************************************
�������ܣ� 595
��������� 
�����������
��    ע�� 
*********************************************/
//#define norly_on
 void  write_595( u8  *dt,int  len )
{  
  u8 dat[1];
  u8 c;
  int i = 0;  

  dat[0] = dt[0];

  for(i=len-1;i>=0;i--)
  {
    for( c=0;c<8;c++ )
    { 
#ifdef   norly_on  //�������߳���
      if(dat[i] & 0x80)
      {
        H595_DS_PD2_Pin_OUT(1); 
        H595_DS_PD2_Pin_OUT(1); 
      }
      else
      {
        H595_DS_PD2_Pin_OUT(0); 
        H595_DS_PD2_Pin_OUT(0); 
      }
#else
      if(dat[i] & 0x80)
      {
        H595_DS_PD2_Pin_OUT(0); 
        H595_DS_PD2_Pin_OUT(0); 
      }
      else
      {
        H595_DS_PD2_Pin_OUT(1); 
        H595_DS_PD2_Pin_OUT(1); 
      }
#endif
      H595_SH_PA1_Pin_OUT(0); 
      H595_SH_PA1_Pin_OUT(0); 
      Delay(5);  
      H595_SH_PA1_Pin_OUT(1); 
      H595_SH_PA1_Pin_OUT(1); 
      dat[i] <<= 1;
    } 
  }

  H595_ST_PD3_Pin_OUT(0);
  H595_ST_PD3_Pin_OUT(0);
  Delay (5);
  H595_ST_PD3_Pin_OUT(1);	//����	
  H595_ST_PD3_Pin_OUT(1);
}

 
/*********************************************
�������ܣ�ϵͳ�ڲ�ʱ������
���������SYS_CLK : 2��4��8��16
�����������
��    ע��ϵͳ����Ĭ���ڲ�2�ͣȣ�
*********************************************/
void CLOCK_Config(u8 SYS_CLK)
{
  //ʱ������Ϊ�ڲ�RC��16M
  CLK->CKDIVR &=~(BIT(4)|BIT(3));

  switch(SYS_CLK)
  {
    case 2: CLK->CKDIVR |=((1<<4)|(1<<3)); break;
    case 4: CLK->CKDIVR |=(1<<4); break;
    case 8: CLK->CKDIVR |=(1<<3); break;
  }
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
