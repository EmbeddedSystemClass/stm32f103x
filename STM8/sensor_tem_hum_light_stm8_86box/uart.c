#include "ALL_Includes.h"

Uart_struct UART1_zigbee_st;

u16 send_urt_time = 0;

u8 Rs485_COMM_CHECK_BUF_VALID(u8 *p);
uint8_t Rs485_COMM_crc8(uint8_t *buf, uint8_t len);
void Rs485_COMM_uart_timeout_fution(void);
void Rs485_COMM_UART_RX_load_buf(u8 *buf, u8 len, u8 transid);
void Rs485_COMM_dma_send_buf(void);
void Rs485_COMM_SD_load_buf2(u8 *urt_buf, u8 len);
void Rs485_COMM_SD_load_buf(u16 d_head, u16 d_tail, TRAN_D_struct *TRAN_info,
                            u8 *urt_buf, u8 len);
void Rs485_COMM_uart_fuc(void);
u8 Rs485_COMM_msg_process(char *tab);

void send_uart_data(u8 *da, u16 len);
// TODO: sdada
/*********************************************
�������ܣ� ���ڷ���
��������� da: ����  u16 len:����
��������� ��
��    ע��
*********************************************/
void send_uart_data(u8 *da, u16 len)
{
  u16 i = 0;

  for (i = 0; i < len; i++)
  {
    while (!(UART1->SR & 0x80))
      ;
    UART1->DR = da[i];
    /* Wait transmission is completed */
    while (!(UART1->SR & 0x80))
      ;
  }
}

/*********************************************
�������ܣ� ��黺�������Ƿ���Ч
���������
���������
��    ע��
*********************************************/
u8 Rs485_COMM_CHECK_BUF_VALID(u8 *p)
{
  u8 i = 0;

  for (i = 0; i < 3; i++) //���ջ����е���������3���ֽڲ�����0
  {
    if (p[i] != 0)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*********************************************************************
 *  crc
 */
uint8_t Rs485_COMM_crc8(uint8_t *buf, uint8_t len)
{
  uint8_t crc;
  uint8_t i;

  crc = 0;

  while (len--)
  {
    crc ^= *buf++;

    for (i = 0; i < 8; i++)
    {
      if (crc & 0x01)

        crc = (crc >> 1) ^ 0x8C;

      else

        crc >>= 1;
    }
  }
  return crc;
}

/*************************************************
 *����1   ��ʱ������
 */
void Rs485_COMM_uart_timeout_fution(void) { UART1_zigbee_st.ACK_TIMEOUT_js++; }

/*************************************************
   �������뻺��
*/
void Rs485_COMM_UART_RX_load_buf(u8 *buf, u8 len, u8 transid)
{
  u8 i = 0, cun_rx_lun;

  cun_rx_lun = UART1_zigbee_st.uart_Rx_lun;

  for (i = 0; i < USART_TXRX_PK_NUM; i++)
  {
    if (Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[cun_rx_lun]) ==
        FALSE) //δʹ�õ�BUFF
    {
      memcpy((u8 *)UART1_zigbee_st.RXBuff[cun_rx_lun], (u8 *)buf, len);
      break;
    }
    //��ʱ���Ͳ����棬����
    cun_rx_lun = 0;
    //( cun_rx_lun >= (USART_TXRX_PK_NUM - 1) ) ? ( cun_rx_lun = 0 ) : (
    // cun_rx_lun ++ );
  }
}

/*****************��������BUF**************************/
//   Rs485_COMM_SD_load_buf   ( 0xAAAA,  0xBBBB,    &TRAN_info1 , dat ,
//   TRAN_info1.data_len );

void Rs485_COMM_SD_load_buf(u16 d_head, u16 d_tail, TRAN_D_struct *TRAN_info,
                            u8 *urt_buf, u8 len)
{
  u8 i = 0, cun_sd_lun;
  u8 crc = 0, z_c = 0;

  if (len > UART_SD_LEN)
  {
    return;
  }

  disableInterrupts();

  cun_sd_lun = UART1_zigbee_st.uart_SD_lun;

  for (i = 0; i < USART_TXRX_PK_NUM; i++)
  {
    if (Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun]) ==
        FALSE) //���û������
    {
      UART1_zigbee_st.SDBuff[cun_sd_lun][0] = d_head;      //��ͷ
      UART1_zigbee_st.SDBuff[cun_sd_lun][1] = d_head >> 8; //��ͷ

      UART1_zigbee_st.SDBuff[cun_sd_lun][2] = UART1_zigbee_st.txtransid; //���

      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3], (u8 *)TRAN_info,
             sizeof(TRAN_D_struct)); //Ŀ���豸����ʶ���  ��  ���ݳ���

      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3 + sizeof(TRAN_D_struct)],
             urt_buf, len); //�������ݣ����ݸ�ʽ�еģ�ʵ�����ݣ�

      UART1_zigbee_st.SDBuff[cun_sd_lun][3 + sizeof(TRAN_D_struct) + len] =
          d_tail; //��β
      UART1_zigbee_st.SDBuff[cun_sd_lun][3 + sizeof(TRAN_D_struct) + len + 1] =
          d_tail >> 8; //

      z_c = 3 + sizeof(TRAN_D_struct) + len + 2;
      crc = Rs485_COMM_crc8(&UART1_zigbee_st.SDBuff[cun_sd_lun][0], z_c);
      UART1_zigbee_st.SDBuff[cun_sd_lun][z_c] = crc; // CRCУ��

      UART1_zigbee_st.SDBuff_len[cun_sd_lun] = z_c + 1;

      UART1_zigbee_st.txtransid++; //���++

      //ֱ�ӷ���
      send_uart_data(UART1_zigbee_st.SDBuff[cun_sd_lun],
                     UART1_zigbee_st.SDBuff_len[cun_sd_lun]);

      memset(UART1_zigbee_st.SDBuff[cun_sd_lun], FALSE, 10);

      break;
    }
    //��ʱ���Ͳ����棬����
    cun_sd_lun = 0;
    //( cun_sd_lun >= USART_TXRX_PK_NUM - 1 ) ? ( cun_sd_lun = 0) : (
    // cun_sd_lun++ );
  }
  enableInterrupts();
}

/***  RS485����� ***/

void Rs485_COMM_uart_fuc(void)
{
  u8 i = 0;

  for (i = 0; i < USART_TXRX_PK_NUM; i++) //ͳ�ƽ��յ�����
  {
    if (Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[i]) ==
        TRUE) // ��黺���е������Ƿ���Ч
    {
      if (Rs485_COMM_msg_process(
              (char *)UART1_zigbee_st.RXBuff[i])) // ����ָ��Ĵ���
      {
        memset(UART1_zigbee_st.RXBuff[i], FALSE, 10); // ��ս��ջ���
      }
      break;
    }
  }
}

/***** ����ָ����� *****/

u8 Rs485_COMM_msg_process(char *tab)
{
  u8 crc = 0, i = 0;
  TRAN_D_struct TRAN_info2;
  u8 fuc_dat[UART_RX_LEN] = {0};
  u8 dat[UART_RX_LEN] = {0};
  // u8 lu[3];

  i = 17 + tab[16];
  if (i < UART_RX_LEN - 17 && tab[0] == 0xAA && tab[1] == 0xAA &&
      tab[i] == 0XBB && tab[i + 1] == 0XBB) //�ж������֡ͷ��֡β������
  {
    crc = Rs485_COMM_crc8((u8 *)&tab[0], i + 2);
    if (tab[i + 2] != crc) //�ж�CRC
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }

  //��Ч����
  memcpy(&TRAN_info2, (u8 *)tab + 3, sizeof(TRAN_D_struct));
  memcpy(fuc_dat, (u8 *)tab + 17, TRAN_info2.data_len); //ȡ�������еġ�ʵ�����ݡ�

  TRAN_info2.source_dev_num = (TRAN_info2.source_dev_num << 8 |
                               TRAN_info2.source_dev_num >> 8); //��ԭԴ��ַ

  TRAN_info2.dest_dev_num = (TRAN_info2.dest_dev_num << 8 |
                             TRAN_info2.dest_dev_num >> 8); //��ԴĿ���ַ

  dat[0] = 0;

  dat[2] = 0; //

  dat[4] = (u8)MCU_UID[0];
  dat[5] = (u8)(MCU_UID[0] >> 8);
  dat[6] = (u8)(MCU_UID[0] >> 16);
  dat[7] = (u8)(MCU_UID[0] >> 24);

  memset(dat + 8, 0, 8);

  if (TRAN_info2.TYPE_NUM == 3) //�ж����������Ƿ�Ϊ ����3
  {
    switch (fuc_dat[0] << 8 | fuc_dat[1]) // CMD
    {
    case notify_net_status: //��ѯ����״̬
    {
      join_flg = fuc_dat[2];
      if (fuc_dat[2] == 1)
        gb_Status = net_online;
      if (gb_Status == net_wait)
        gb_countdown = 0;
      if (join_flg)
      {
        dat[1] = notify_net_status;
#ifdef sensor_md_BODY
        dat[3] = SENSOR_body;
        TRAN_info1.source_dev_num =
            (SENSOR_body << 8 | SENSOR_body >> 8);

        TRAN_info1.data_len = 16 + 1;

        dat[8 + 8] = body_check;

        //#else
        dat[3] = DTN_86_SENSOR_MQ;
        TRAN_info1.source_dev_num =
            (DTN_86_SENSOR_MQ << 8 | DTN_86_SENSOR_MQ >> 8);

        TRAN_info1.data_len = 16 + 2;

        if (get_adc(10) > 850)
        {
          dat[8 + 8] = 1;
        }
        else
        {
          dat[8 + 8] = 0;
        }
        dat[8 + 8 + 1] = fire_check;

#endif

#ifdef tem_hum_light

        dat[3] = SENSOR_tem_hum_light;
        TRAN_info1.source_dev_num =
            (SENSOR_tem_hum_light << 8 | SENSOR_tem_hum_light >> 8);

        TRAN_info1.data_len = SENSOR_tem_LEN;

        //;
#endif

        Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat,
                               TRAN_info1.data_len);
      }
    }
    break;

    case query_dev_info: //�յ���ѯ����  ����
    {
      dat[1] = upload_info; //

#ifdef sensor_md_BODY
      dat[3] = SENSOR_body;
      TRAN_info1.source_dev_num =
          (SENSOR_body << 8 | SENSOR_body >> 8);

      TRAN_info1.data_len = 16 + 1;

      dat[8 + 8] = body_check;

      //#else
      dat[3] = DTN_86_SENSOR_MQ;
      TRAN_info1.source_dev_num =
          (DTN_86_SENSOR_MQ << 8 | DTN_86_SENSOR_MQ >> 8);

      TRAN_info1.data_len = 16 + 2;
      if (get_adc(10) > 850)
      {
        dat[8 + 8] = 1;
      }
      else
      {
        dat[8 + 8] = 0;
      }

      dat[8 + 8 + 1] = fire_check;

#endif

#ifdef tem_hum_light

      dat[3] = SENSOR_tem_hum_light;
      TRAN_info1.source_dev_num =
          (SENSOR_tem_hum_light << 8 | SENSOR_tem_hum_light >> 8);

      TRAN_info1.data_len = SENSOR_tem_LEN;

      //;
#endif

      Rs485_COMM_SD_load_buf(0xAAAA, 0xBBBB, &TRAN_info1, dat,
                             TRAN_info1.data_len);
    }
    break;

    default:
      break;
    }
  }
  return 1;
}
