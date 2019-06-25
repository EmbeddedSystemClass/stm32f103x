#include "global.h"	

  /*
	
	5̬
	
״̬	A         B           C
0	  0FF(1)     0FF(1)      0FF(1)
1	  0FF(1)     0FF(1)       0N(0)
2	  0FF(1)     0N(0)       0FF(1) 
3	  0N(0)      0FF(1)      0FF(1)
4   ż��  ǰ4����� ������� ʱ��Ҳ���
	 
  2̬
״̬	A         B           C
0	  0FF(1)     0FF(1)      0FF(1)
1	  0N(0)      0FF(1)      0FF(1)
   
	*/
	 

void  write_595_LED( u8  *dat,int  len );

void  write_595_JDQ( u8  *dat,int  len );

 

void  write_595_JDQ( u8  *dat,int  len )
{
	u8   c;
	int  i=0;  
	for(i=len-1;i>=0;i--)
	{
		for( c=0;c<8;c++ )
		{ 
			if(dat[i]&0x80)
			{
				DS_595=1; 
			}
			else
			{
				DS_595=0; 
			}

			SH_595=0; 
			delay_us(1);
			SH_595=1; 		
			dat[i]<<=1;
		} 
	}		
	ST_595 = 0;
	delay_us(1);
	ST_595 = 1;	//����	
}


void  write_595_LED( u8  *dat,int  len )
{
	u8   c;
	int  i=0;  

	for(i=len-1;i >= 0;i--)
	{
		for( c=0;c<8;c++ )
		{ 
			if(dat[i] & 0x80)
			{
				LED_595_DS = 1; 
			}
			else
			{
				LED_595_DS = 0; 
			}

			LED_595_SH = 0; 
			delay_us(1);
			LED_595_SH = 1; 		
			dat[i] <<= 1;
		} 
	}	
	
	LED_595_ST = 0;
	delay_us(1);
	LED_595_ST = 1;	//����	
}

