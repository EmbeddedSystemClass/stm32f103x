/***********************************************************************************************************************
Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
�ļ��� :		SHT1X.c
����   :    SHT1X��ʪ�ȴ�����ģ������ļ�
����   :    
�汾   :    V1.0
�޸�   :  
�������:	2019.3.25
��ע:		SHT1X��IICͨ��Э�飬���ļ�����������Ϊ��������
************************************************************************************************************************/
#include "SHT1X.h"
#include <math.h>

/*��̬�ֲ�����*/
static uint64_t SHT11_Time = 0;		//���ڼ�����β���֮���ʱ�䣬�������1000(ms)
/**************/
/*************************************************************
  Function   ��SHT10_Dly  
  Description��SHT10ʱ����Ҫ����ʱ
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
    u16 i = 0;
    for(i = 500; i > 0; i--);
}


/*************************************************************
  Function   ��SHT10_Config  
  Description����ʼ�� SHT10����
  Input      : none        
  return     : none    
*************************************************************/
void SHT1X_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;        
	//��ʼ��SHT10����ʱ��
	RCC_APB2PeriphClockCmd(SHT10_AHB2_CLK|RCC_APB2Periph_AFIO ,ENABLE);	//ʹ����ӳ��
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
			
	//PD0 DATA �������        
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
	//PD1 SCK �������
	GPIO_InitStructure.GPIO_Pin = SHT10_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_SCK_PORT, &GPIO_InitStructure);

	SHT10_ConReset();        //��λͨѶ
}

/*************************************************************
  Function   ��SHT1X_GetValue
  Description������DATA����Ϊ���
  Input      : none        
  return     : u8 0-ʧ�� 1-�ɹ�   
*************************************************************/
u8 SHT1X_GetValue(void)
{
	u8  err = 0, checksum = 0;

	if(SystemCnt-SHT11_Time<1000)										//�����ϴβ�������Ƿ����1000ms
	{
		return 0;
	}
	err += SHT10_Measure(&temp_val, &checksum, TEMP);                  //��ȡ�¶Ȳ���ֵ
	err += SHT10_Measure(&humi_val, &checksum, HUMI);                  //��ȡʪ�Ȳ���ֵ
	if(err != 0)
	{
		SHT10_ConReset();
		
		return 0;
	}
	else
	{
		SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //����ʵ�ʵ���ʪ��ֵ
		dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);                 //����¶���¶�
	}
	SHT11_Time = SystemCnt;												//��¼�˴���ɲ�����ʱ��
	
	return 1;
}
/*************************************************************
  Function   ��SHT10_DATAOut
  Description������DATA����Ϊ���
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA �������        
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_DATAIn  
  Description������DATA����Ϊ����
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        //PD0 DATA ��������        
        GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ��SHT10_WriteByte  
  Description��д1�ֽ�
  Input      : value:Ҫд����ֽ�        
  return     : err: 0-��ȷ  1-����    
*************************************************************/
u8 SHT10_WriteByte(u8 value)
{
        u8 i, err = 0;
        
        SHT10_DATAOut();                          //����DATA������Ϊ���

        for(i = 0x80; i > 0; i /= 2)  //д1���ֽ�
        {
                if(i & value)
                        SHT10_DATA_H();
                else
                        SHT10_DATA_L();
                SHT10_Dly();
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_DATAIn();                                  //����DATA������Ϊ����,�ͷ�DATA��
        SHT10_SCK_H();
        err = SHT10_DATA_R();                  //��ȡSHT10��Ӧ��λ
        SHT10_SCK_L();

        return err;
}

/*************************************************************
  Function   ��SHT10_ReadByte  
  Description����1�ֽ�����
  Input      : Ack: 0-��Ӧ��  1-Ӧ��        
  return     : err: 0-��ȷ 1-����    
*************************************************************/
u8 SHT10_ReadByte(u8 Ack)
{
        u8 i, val = 0;

        SHT10_DATAIn();                                  //����DATA������Ϊ����
        for(i = 0x80; i > 0; i /= 2)  //��ȡ1�ֽڵ�����
        {
                SHT10_Dly();
                SHT10_SCK_H();
                SHT10_Dly();
                if(SHT10_DATA_R())
                        val = (val | i);
                SHT10_SCK_L();
        }
        SHT10_DATAOut();                          //����DATA������Ϊ���
        if(Ack)
                SHT10_DATA_L();                          //Ӧ��������ȥ������ȥ������(У������)
        else
                SHT10_DATA_H();                          //��Ӧ���������˽���
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_SCK_L();
        SHT10_Dly();

        return val;                                          //���ض�����ֵ
}


/*************************************************************
  Function   ��SHT10_TransStart  
  Description����ʼ�����źţ�ʱ�����£�
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(void)
{
        SHT10_DATAOut();                          //����DATA������Ϊ���

        SHT10_DATA_H();
        SHT10_SCK_L();
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_DATA_L();
        SHT10_Dly();
        SHT10_SCK_L();
        SHT10_Dly();
        SHT10_SCK_H();
        SHT10_Dly();
        SHT10_DATA_H();
        SHT10_Dly();
        SHT10_SCK_L();

}


/*************************************************************
  Function   ��SHT10_ConReset  
  Description��ͨѶ��λ��ʱ�����£�
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_ConReset(void)
{
        u8 i;

        SHT10_DATAOut();

        SHT10_DATA_H();
        SHT10_SCK_L();

        for(i = 0; i < 9; i++)                  //����SCKʱ��9c��
        {
                SHT10_SCK_H();
                SHT10_Dly();
                SHT10_SCK_L();
                SHT10_Dly();
        }
        SHT10_TransStart();                          //��������
}



/*************************************************************
  Function   ��SHT10_SoftReset  
  Description����λ
  Input      : none        
  return     : err: 0-��ȷ  1-����    
*************************************************************/
u8 SHT10_SoftReset(void)
{
        u8 err = 0;

        SHT10_ConReset();                              //ͨѶ��λ
        err += SHT10_WriteByte(SOFTRESET);//дRESET��λ����

        return err;
}


/*************************************************************
  Function   ��SHT10_ReadStatusReg  
  Description����״̬�Ĵ���
  Input      : p_value-���������ݣ�p_checksun-������У������       
  return     : err: 0-��ȷ  0-����    
*************************************************************/
u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum)
{
        u8 err = 0;

        SHT10_TransStart();                                        //��ʼ����
        err = SHT10_WriteByte(STATUS_REG_R);//дSTATUS_REG_R��ȡ״̬�Ĵ�������
        *p_value = SHT10_ReadByte(ACK);                //��ȡ״̬����
        *p_checksum = SHT10_ReadByte(noACK);//��ȡ���������
        
        return err;
}



/*************************************************************
  Function   ��SHT10_WriteStatusReg  
  Description��д״̬�Ĵ���
  Input      : p_value-Ҫд�������ֵ       
  return     : err: 0-��ȷ  1-����    
*************************************************************/
u8 SHT10_WriteStatusReg(u8 *p_value)
{
        u8 err = 0;

        SHT10_TransStart();                                         //��ʼ����
        err += SHT10_WriteByte(STATUS_REG_W);//дSTATUS_REG_Wд״̬�Ĵ�������
        err += SHT10_WriteByte(*p_value);         //д������ֵ

        return err;
}



/*************************************************************
  Function   ��SHT10_Measure  
  Description������ʪ�ȴ�������ȡ��ʪ��
  Input      : p_value-������ֵ��p_checksum-������У����        
  return     : err: 0-��ȷ 1������    
*************************************************************/
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode)
{
        u8 err = 0;
        u32 i;
        u8 value_H = 0;
        u8 value_L = 0;

        SHT10_TransStart();                                                 //��ʼ����
        switch(mode)                                                         
        {
        case TEMP:                                                                 //�����¶�
                err += SHT10_WriteByte(MEASURE_TEMP);//дMEASURE_TEMP�����¶�����
                break;
        case HUMI:
                err += SHT10_WriteByte(MEASURE_HUMI);//дMEASURE_HUMI����ʪ������
                break;
        default:
                break;
        }
        SHT10_DATAIn();
        for(i = 0; i < 72000000; i++)                             //�ȴ�DATA�źű�����
        {
                if(SHT10_DATA_R() == 0) break;             //��⵽DATA�������ˣ�����ѭ��
        }
        if(SHT10_DATA_R() == 1)                                //����ȴ���ʱ��
                err += 1;
        value_H = SHT10_ReadByte(ACK);
        value_L = SHT10_ReadByte(ACK);
        *p_checksum = SHT10_ReadByte(noACK);           //��ȡУ������
        *p_value = (value_H << 8) | value_L;
        return err;
}


/*************************************************************
  Function   ��SHT10_Calculate  
  Description��������ʪ�ȵ�ֵ
  Input      : Temp-�Ӵ������������¶�ֵ��Humi-�Ӵ�����������ʪ��ֵ
               p_humidity-�������ʵ�ʵ�ʪ��ֵ��p_temperature-�������ʵ���¶�ֵ        
  return     : none    
*************************************************************/
void SHT10_Calculate(u16 t, u16 rh, float *p_temperature, float *p_humidity)
{
        const float d1 = -39.7;
        const float d2 = +0.01;
        const float C1 = -2.0468;
        const float        C2 = +0.0367;
        const float C3 = -0.0000015955;        
        const float T1 = +0.01;
        const float T2 = +0.00008;

        float RH_Lin;                                                                                  //RH����ֵ        
        float RH_Ture;                                                                                 //RH��ʵֵ
        float temp_C;

        temp_C = d1 + d2 * t;                                                              //�����¶�ֵ        
        RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //����ʪ��ֵ
        RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;        //ʪ�ȵ��¶Ȳ���������ʵ�ʵ�ʪ��ֵ

        if(RH_Ture > 100)                                                                        //����ʪ��ֵ����
                RH_Ture        = 100;
        if(RH_Ture < 0.1)
                RH_Ture = 0.1;                                                                        //����ʪ��ֵ����

        *p_humidity = RH_Ture;
        *p_temperature = temp_C;

}


/*************************************************************
  Function   ��SHT10_CalcuDewPoint  
  Description������¶��
  Input      : h-ʵ�ʵ�ʪ�ȣ�t-ʵ�ʵ��¶�        
  return     : dew_point-¶��    
*************************************************************/
float SHT10_CalcuDewPoint(float t, float h)
{
        float logEx, dew_point;

        logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
        dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);

        return dew_point; 
}


//int main(void)
//{  
//        u16 humi_val, temp_val;
//        u8 err = 0, checksum = 0;
//        float humi_val_real = 0.0; 
//        float temp_val_real = 0.0;
//        float dew_point = 0.0;
//        
//        BSP_Init();
//        printf("\nSHT10���ҶȲ��Գ���!!!\n");
//        SHT10_Config();
//        while(1)
//        {
//                err += SHT10_Measure(&temp_val, &checksum, TEMP);                  //��ȡ�¶Ȳ���ֵ
//                err += SHT10_Measure(&humi_val, &checksum, HUMI);                  //��ȡʪ�Ȳ���ֵ
//                if(err != 0)
//                        SHT10_ConReset();
//                else
//                {
//                        SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //����ʵ�ʵ���ʪ��ֵ
//                        dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);                 //����¶���¶�
//                } 
//                printf("��ǰ�����¶�Ϊ:%2.1f�棬ʪ��Ϊ:%2.1f%%��¶���¶�Ϊ%2.1f��\r\n", temp_val_real, humi_val_real, dew_point);
//                LED1_Toggle();
//                Delay_ms(1000);
//        }
//}

///***********************************************************************************************************************
//Copyright 2019 - 2027 ���ڹ�̩�����������ɷ����޹�˾. All rights reserved.
//�ļ��� :		SHT1X.c
//����   :      SHT1X��ʪ�ȴ�����ģ������ļ�
//����   :      hongxiang.liu
//�汾   :      V1.0
//�޸�   :  
//�������:		2019.3.25
//��ע:			SHT1X��IICͨ��Э�飬���ļ�����������Ϊ��������
//************************************************************************************************************************/

///*************HEADS**********/
//#include "SHT1X.h"
///***************************/

///* ȫ�ֱ������� */
///****************/

////��ʼ��SHT1X,��������
////DTA--PB3
////CLK--PB4
//void SHT1x_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��Pc�˿�ʱ��
//	
//	GPIO_InitStructure.GPIO_Pin= SDA | SCL;   //
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��

////ͨ�紫������Ҫ11ms��������״̬���ڴ�֮ǰ������Դ����������κ�����
//    SHT1x_Reset();
//}

////����SHT1Xģʽ����д�Ĵ���
//void SHT1x_Config(void)
//{
//	u8 SHT1X_value = 0;
//	
//	delay_ms(50);											//��ʱ50ms��ȷ��SHT1X�ȶ�
//	if(!SHT1X_read_statusreg(&SHT1X_value))
//	{ 
//		if(SHT1X_value&0x07)
//		{
//			for(SHT1X_value=5;SHT1X_value!=0;SHT1X_value--)
//			{ 
//				if(!SHT1X_write_statusreg(0))break;
//				else SHT1X_softreset();
//			}
//		}
//	}
//	SHT1x_Reset();	
//}

////��ȡSHT1X����������
//void SHT1x_GetValue(void)
//{
//	u8 SHT1x_error = 0;
//	u8 checksum = 0;
//    u16 T_Value=0,H_Value=0;
//	
//	SHT1x_error+=SHT1x_Measure( &T_Value,&checksum,0);  //measure temperature 
//	SHT1x_error+=SHT1x_Measure( &H_Value,&checksum,1);  //measure humidity
//		
//	if(SHT1x_error!=0)SHT1x_Reset(); //ͨѶ����,��λ������
//	else
//	{
//		sht1x.T_Result = T_Value;
//		sht1x.H_Result = H_Value;
//		SHT1X_Caculation1((float*)&sht1x.T_Result, (float*)&sht1x.H_Result ); 
//	   
//		sht1x.Temperature=sht1x.T_Result;
//		sht1x.Humidity=(u16)sht1x.H_Result; 
//		sht1x.DEW=SHT1X_dewpoint1(sht1x.T_Result, sht1x.H_Result);
//	}
//}

//void SDA_SET_OUT(void) //����SDA�˿ڵ����ģʽ����©���
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Pin = SDA;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	// ��©���
//  GPIO_Init(GPIOB, &GPIO_InitStructure);	
//}
//void SDA_SET_IN(void)//����SDA�˿ڵ����ģʽ����©����
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = SDA;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// ��������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);	
//}

//void SCL_Pin_OUT(u8 out)//����SCL�˿ڵ����״̬
//{
//    if(out)GPIO_SetBits(GPIOB,SCL);
//    else GPIO_ResetBits(GPIOB,SCL);
//}

//void SDA_Pin_OUT(u8 out)//����SDA�˿ڵ����״̬
//{
//    SDA_SET_OUT();
//    if(out)GPIO_SetBits(GPIOB,SDA);
//    else GPIO_ResetBits(GPIOB,SDA);
//}

///*=========================================
//�� �� ����u8 SHT1x_Write_Byte(u8 value)�ֽڴ��ͺ���
//��    ��������value Ҫ���͵�����
//������������Ԫ����SHT10����һ���ֽڵ�����
//�� �� ֵ������ֵerror error��0��������ȷ
//					  error��1�����ʹ���
//=========================================*/
//u8 SHT1x_Write_Byte(u8 value)
//{
//    u8 i=0,error=0;
//    SDA_SET_OUT();
//	SCL_Pin_OUT(0);
//    for(i=0x80;i>0;i/=2) 
//    {
//        if(i&value)SDA_Pin_OUT(1); //ѭ�����룬�����ΪҪ���͵�λ
//        else SDA_Pin_OUT(0);
//        
//        delay_us(30);
//        SCL_Pin_OUT(1);
//        delay_us(30);
//        SCL_Pin_OUT(0);
//        delay_us(30);
//    }
//    
//    SDA_SET_IN();//SDA�˿�ģʽΪ��������
//    
//    SCL_Pin_OUT(1);
//    error=GPIO_ReadInputDataBit(GPIOB, SDA);
//    SCL_Pin_OUT(0);
//    
//    return error;  //error��1��ͨѶ����;error��0��ͨѶ�ɹ�
//}

///*=========================================
//�� �� ����SHT1x_Read_Byte(u8 dat)�����ݺ���
//��    ��������dat=0,������һ���ֽڵĻظ�
//              dat=1,ֹͣͨѶ
//��������������Ԫ����SHT10���͵�������һ���ֽڵ����ݣ�
//�� �� ֵ������ֵval	��Ϊ���ܵ�������
//=========================================*/
//u8 SHT1x_Read_Byte(u8 dat)
//{
//    u8 i=0,val=0;
//    
//    SDA_SET_IN();
//	SCL_Pin_OUT(0);
//    for(i=0x80;i>0;i/=2)
//    {
//        delay_us(30);
//        SCL_Pin_OUT(1);
//        delay_us(30);
//        
//       if(GPIO_ReadInputDataBit(GPIOB, SDA)==1)val=(val | i);
//       SCL_Pin_OUT(0);
//    }
//    
//    SDA_SET_OUT();
//    if(dat)SDA_Pin_OUT(0);
//    else SDA_Pin_OUT(1);
//    delay_us(30);
//    
//    SCL_Pin_OUT(1);
//    delay_us(30);
//    SCL_Pin_OUT(0);
//    delay_us(30);
//    
//    return val;
//}

////���������ź�
///*ʱ��ͼ��
//        ____    ____
//SCL: __|    |__|    |__
//     ____         _____
//SDA:     |_______|   
//*/
//void SHT1x_Start(void)
//{
//	SDA_SET_OUT(); //����SDA�˿�Ϊ���ģʽ
//    
//	SDA_Pin_OUT(1);
//	SCL_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(1);   
//	delay_us(30);
//	
//	SDA_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(0);
//	delay_us(30);
//	SCL_Pin_OUT(1);
//	delay_us(30);
//    
//	SDA_Pin_OUT(1);	
//	delay_us(30);
//	SCL_Pin_OUT(0);
//}

////ͨѶ��λʱ��
///*ʱ��ͼ��
//        __    __    __    __    __    __    __    __    __    __
//SCL: __|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__
//     ______________________________________________________       _____ 
//SDA:                                                       |______|
//*/
//void SHT1x_Reset(void)
//{
//	u8 i;
//    SDA_SET_OUT(); //����SDA�˿�Ϊ���ģʽ

//	SDA_Pin_OUT(1);	    
////    SCL_Pin_OUT(0);

//	for(i=0;i<9;i++)
//	{
//		SCL_Pin_OUT(0);
//        delay_us(30);
//		SCL_Pin_OUT(1);
//        delay_us(30);
//	}
//    SHT1x_Start(); //��������
//}
///*=========================================
//�� �� ����char s_softreset(void)�����λ����
//��    ������
//����������resets the sensor by a softreset 
//�� �� ֵ��error
//=========================================*/
//char SHT1X_softreset(void)
//{ 
//    unsigned char error=0;  
//    SHT1x_Reset();              //reset communication
//    error+=SHT1x_Write_Byte(RESET);       //send RESET-command to sensor
//    return error;                     //error=1 in case of no response form the sensor
//}
///*=========================================
//�� �� ����char s_write_statusreg(unsigned char value)д״̬����
//��    ��������value
//���������� writes the status register with checksum (8-bit)
//�� �� ֵ��error
//=========================================*/
//char SHT1X_write_statusreg(unsigned char value)
//{ 
//    unsigned char error=0;
//    SHT1x_Start();                   //transmission start
//    error+=SHT1x_Write_Byte(STATUS_REG_W);//send command to sensor
//    error+=SHT1x_Write_Byte(value);    //send value of status register
//    return error;                     //error>=1 in case of no response form the sensor
//}

///*=========================================
//�� �� ����char s_write_statusreg(unsigned char value)д״̬����
//��    ��������value
//����������reads the status register with checksum (8-bit)
//�� �� ֵ��error
//=========================================*/
//char SHT1X_read_statusreg(unsigned char *p_value)
//{ 
//    unsigned char error=0;
//    SHT1x_Start();                    //transmission start
//    error+=SHT1x_Write_Byte(STATUS_REG_R); //send command to sensor
//    error+=SHT1x_Write_Byte(ACK);        //read status register (8-bit)
//    *p_value = SHT1x_Read_Byte(noACK);   //read checksum (8-bit)  
//    return error;                     //error=1 in case of no response form the sensor
//}

///*=========================================
//�� �� ����u8 SHT1x_Measure(u8 *p_value, u8 *p_checksum, u8 Mode)��������
//��    ��������mode	ȷ���ǲ����¶Ȼ����ǲ���ʪ��
//��������������ָ�SHT10ִ���¶Ⱥ�ʪ�ȵĲ���ת��
//�� �� ֵ����
//=========================================*/
//u8 SHT1x_Measure(u16 *p_value, u8 *p_checksum, u8 Mode)
//{
//    u8 error=0,Value_H=0,Value_L=0;
//    
//    SHT1x_Start();//��ʼ����ת��
//    switch(Mode)
//    {
//        case 0: error+=SHT1x_Write_Byte(MEASURE_TEMP);break; //���¶ȵ�ֵ  
//        case 1: error+=SHT1x_Write_Byte(MEASURE_HUMI);break; //��ʪ�ȵ�ֵ
//        
//        default:   
//            break;
//    } 
//	SDA_SET_IN();//����SDA�˿�Ϊ����ģʽ
//                      
//    delay_ms(5000);//�ȴ��������
//    if(GPIO_ReadInputDataBit(GPIOB, SDA))error+=1;//����ʱ��������DQû���ͣ���˵�������д���
//    else
//    {
//        Value_H=SHT1x_Read_Byte(ACK);		 //���ݵĸ��ֽ�
//        Value_L=SHT1x_Read_Byte(ACK);	     //���ݵĵ��ֽ�    		
//        *p_value=( Value_H << 8) | Value_L;
//        *p_checksum=SHT1x_Read_Byte(noACK);	     //CRCУ����	
//    }
//    return error;
//}

///*=========================================
//�� �� ����void SHT1X_Caculation(float *p_temperature,float *p_humidity )���ݴ�����
//��    �����޲���
//�����������¶Ⱥ�ʪ�Ȳ���������¶�ֵ�����ʪ��ֵ
//�� �� ֵ���޷���ֵ
//=========================================*/
//void SHT1X_Caculation(float *p_temperature,float *p_humidity )
//{
//    const float c1=-4.0;
//    const float c2=+0.0405;
//	const float c3=-0.0000028;			//����Ϊ12λʪ��������ʾȡֵ
//	const float t1=+0.01;
//	const float t2=+0.00008;			//����Ϊ14λ�¶�������ʾȡֵ

//	
//	float T=*p_temperature;
//	float H=*p_humidity;
//	float H_nonline_Compensation;
//	float Humi_Comp;
//	float Temp_Comp;
//	
//	Temp_Comp=T * 0.01 - 39.6;					          //�¶ȵĲ���
//		
//	H_nonline_Compensation=c3*H*H  + c2*H + c1;                  //���ʪ�ȷ����Բ���
//    Humi_Comp=( Temp_Comp - 25 ) * ( t1 + t2*H ) + H_nonline_Compensation; //���ʪ�ȶ����¶������Բ���
//	
//	if( Humi_Comp > 100 ) Humi_Comp=100;			  //���ʪ�����ֵ����
//	if( Humi_Comp < 0.1 ) Humi_Comp=0.1;			  //���ʪ����Сֵ����
//	 
//	*p_temperature=Temp_Comp;						//�����¶Ȳ�����Ľ��
//	*p_humidity=Humi_Comp; 						  //�������ʪ�Ȳ�����Ľ��
//}
///*=========================================
//�� �� ����void SHT1X_Caculation1(float *p_temperature,float *p_humidity )���ݴ�����
//��    �����޲���
//�����������¶Ⱥ�ʪ�Ȳ���������¶�ֵ�����ʪ��ֵ
//�� �� ֵ���޷���ֵ
//=========================================*/
//void SHT1X_Caculation1(float *p_temperature,float *p_humidity )
//{
//    const int c1=40000000;
//    const int c2=405000;
//	const int c3=28;			//����Ϊ12λʪ��������ʾȡֵ
//	const int t1=1;
//	const int t2=8;			//����Ϊ14λ�¶�������ʾȡֵ

//	
//	float T=*p_temperature;
//	float H=*p_humidity;
//	float H_nonline_Compensation;
//	int Humi_Comp;
//	int Temp_Comp;
//	
//	Temp_Comp=(signed long)(T * t1 - 3960);					          //�¶ȵĲ���
//		
//	H_nonline_Compensation=(c2*H -c3* H*H-c1)/100000;                  //���ʪ�ȷ����Բ���
//    Humi_Comp=(float)( Temp_Comp - 2500 ) * ( t1 + t2*H )/100000 + H_nonline_Compensation; //���ʪ�ȶ����¶������Բ���
//	
//	if( Humi_Comp > 10000 ) Humi_Comp=10000;			  //���ʪ�����ֵ����
//	if( Humi_Comp < 10 ) Humi_Comp=10;			  //���ʪ����Сֵ����
//	 
//	*p_temperature=Temp_Comp;			//�����¶Ȳ�����Ľ��
//	*p_humidity=Humi_Comp; 						  //�������ʪ�Ȳ�����Ľ��
//}

///*=========================================
//�� �� ����float SHT1X_dewpoint1(float t,float h)¶�����ݴ�����
//��    �����¶�T(��ʵֵ),ʪ��H(��ʵֵ)
//��������������¶���ֵ
//�� �� ֵ��dew_point
//=========================================*/
////-----�˺�������������-----------------------
//float SHT1X_dewpoint(float t,float h) //¶��
//{ 
//    float k,dew_point ;
//  
//    k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
//    dew_point = 243.12*k/(17.62-k);
//    return dew_point;
//}
////----------------------------------------

///*=========================================
//�� �� ����float SHT1X_dewpoint1(float t,float h)¶�����ݴ�����
//��    �����¶�T,ʪ��H
//��������������¶���ֵ
//�� �� ֵ��dew_point
//=========================================*/
//float SHT1X_dewpoint1(float t,float h) //¶��
//{ 
//    const float tn1=24312;  //for 0=< T <=50;
//    const float m1=1762;    //for 0=< T <=50;
//    
//    const float tn2=27262;  //for -40=< T <0;
//    const float m2=2246;    //for -40=< T <0;
//    
//    float dew_point,h1,t1 ;
//    
//    if((t>=0)&&(t<=50))
//    {
//        h1=tn1*(log10(h)+(m1*t)/(tn1+t*100));
//        t1=m1-log10(h)*100-((m1*t*100)/(tn1+t*100));
//        dew_point=h1/t1;
//    }
//    else if((t<0)&&(t>=-40))
//    {
//        h1=tn2*(log10(h)+(m2*t)/(tn2+t*100));
//        t1=m2-log10(h)*100-((m2*t*100)/(tn2+t*100));
//        dew_point=h1/t1;    
//    }
//    return dew_point;
//}







