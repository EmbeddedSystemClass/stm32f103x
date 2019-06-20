 
#include "DS1302.h" 
 
#include "delay.h"  

DS_TIME_T   DS_TIME;




u8   ChangeToHex(u8 dat);
u8   ChangeToDec(u8 dat);
void   DS1302_IO_Init(void);
void   WriteOneChar_DS1302(u8  m);	
u8  ReadOneChar_1302(void);
u8  ReadOneWord_1302(u8  address);
void  WriteOneWord_DS1302(u8 address,u8  shuju);
void  Settime_ds1302(u16 year,u8 month,u8 day ,u8 hour,u8 minute,u8 sec);
void  Gettime_ds1302(void);



/**ʮ��������ת��Ϊʮ������*******/
u8   ChangeToHex(u8 dat)
{
     u8  temp;
     temp=(dat/10)*16+dat%10;
     return  temp;
}


/**ʮ������ת��Ϊʮ����*******/
u8   ChangeToDec(u8 dat)
{
     u8  temp;
     temp=(dat/16)*10+dat%16;
     return  temp;
}




void   DS1302_IO_Init(void)
{
   GPIO_InitTypeDef   GPIO_InitStructure;

   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure); 
}
 
/************д��һ�ֽ�����****************/
void   WriteOneChar_DS1302(u8  m)
{
  u8   i=0;
	for(i=0;i<8;i++)
	{ 
	    DS1302_SCLK(0);
		  delay_us(2);
		  if( m&0x01==1 )
			{ 
		  	DS1302_OUTPUT(1);
			}
	    else
			{
		  	DS1302_OUTPUT(0); 
			}
	    delay_us(2);
			DS1302_SCLK(1);
			delay_us(2);
			m=m>>1;
			delay_ms(1); 
	}
	
}


/************д��һ�ֽ�����****************/
u8  ReadOneChar_1302(void)
{
   u8  i, temp=0x00;
	  
	 for(i=0;i<8;i++)
	 {
	    temp=temp>>1;
		  DS1302_SCLK(0);
	    delay_us(2);
	    if(DS1302_INPUT()==1)
			temp=temp|0x80; 
		  DS1302_SCLK(1);		  
 	    delay_us(2);	 
	 }
	 return  temp;
}

/*************��DS1302 ��ȡ����***************/
u8  ReadOneWord_1302(u8  address)
{
   u8  temp;
   GPIO_InitTypeDef   GPIO_InitStructure;  
	 DS1302_SCLK(0);
	 delay_us(2);
	 DS1302_CE(1);
	 delay_us(4);


   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 ;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure); 
	
   WriteOneChar_DS1302(address);
	
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 ;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	 temp= ReadOneChar_1302( );
	 delay_us(2);
	 DS1302_SCLK(0);
	 delay_us(2);
	 DS1302_CE(0);
	 
	 return  temp;
}


/************* DS1302д������***************/
void  WriteOneWord_DS1302(u8 address,u8  shuju)
{
   GPIO_InitTypeDef   GPIO_InitStructure;  
	 DS1302_SCLK(0); 
	 delay_us(2);
	 DS1302_CE(1);
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 ;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);  
	 
	 WriteOneChar_DS1302(address);
	 WriteOneChar_DS1302(shuju);
	 DS1302_SCLK(0);
	 delay_us(2);
	 DS1302_CE(0);
	 
}


/*********����ʱ�ڣ�ʱ�䣬�����꣬�£��գ�ʱ���֣���***********/
void  Settime_ds1302(u16 year,u8 month,u8 day ,u8 hour,u8 minute,u8 sec)
{
   year= ChangeToHex(year-2000);
   month=ChangeToHex(month);
   day=ChangeToHex(day);
   hour=ChangeToHex(hour);
   minute=ChangeToHex(minute);
  // week=ChangeToHex(week);
   sec=ChangeToHex(sec);
	
   ENWrite();
	 WriteOneWord_DS1302(0x8c,year);
	 WriteOneWord_DS1302(0x88,month);
	 WriteOneWord_DS1302(0x86,day);
	// WriteOneWord_DS1302(0x8a,week);
	 WriteOneWord_DS1302(0x84,hour);
	 WriteOneWord_DS1302(0x82,minute);
   WriteOneWord_DS1302(0x80,sec);
	 DISWrite();
}


/*********��ȡ���ڣ�ʱ�䣬�����꣬�£��գ�ʱ���֣���*******/
void  Gettime_ds1302(void)
{
   u8    buffer[7];
   buffer[6]=ReadOneWord_1302(0x8d);  //��ݵ���λ
   buffer[5]=ReadOneWord_1302(0x89)&0x1f;  //�·�
   buffer[4]=ReadOneWord_1302(0x87)&0x3f;  //����
  // buffer[3]=ReadOneWord_1302(0x8b)&0x3f;  //
   buffer[2]=ReadOneWord_1302(0x85)&0x1f;  //ʱ
   buffer[1]=ReadOneWord_1302(0x83)&0x7f; // ��
   buffer[0]=ReadOneWord_1302(0x81)&0x7f;	//��
	
	 DS_TIME.w_year= ChangeToDec(buffer[6])+2000;
	 DS_TIME.w_month= ChangeToDec(buffer[5]);
	 DS_TIME.w_date= ChangeToDec(buffer[4]);
	// DS_TIME.w_week= ChangeToDec(buffer[3]);
	 DS_TIME.hour= ChangeToDec(buffer[2]);
	 DS_TIME.min= ChangeToDec(buffer[1]);
   DS_TIME.sec= ChangeToDec(buffer[0]);
	 
}









