/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: ModBus.c
** ������Ա: ����
** ��������: 2019-07-31
** �ĵ�����: Modbus��ع��ܺ���
********************************************************************************/
#include "myconfig.h"
u16 Reg[]= {0x0000,0x0000,0x0000,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0x0001,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000/*20 �������*/,0x0000,/*21 ERRORCode*/0xffff,/*��ת�������*/0x0000,0x0000/*�ڴ�ʹ��*/};
//14                                                                               10T1�궨  T2     T3     T4
MODBUS modbus;
/*******************************************************************************
** ��������: Mosbus_Init
** ��������: MOdbus��ʼ������
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/

void Mosbus_Init(void)
{
    Uart_HardwareInit(ID_UART3,UART_BPS_115200,USART_Parity_No);///<��ʼ��UART3�������Ӵ�����
    modbus.myadd=4;  //�����豸�ĵ�ַ
    modbus.timrun=0; //MODbus��ʱ��ֹͣ��ʱ
}
/*******************************************************************************
** ��������: Modbud_fun3
** ��������: 3�Ź����룬��ȡ�Ĵ�����ֵ
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Modbud_fun3(void)  //3�Ź����봦��  ---����Ҫ��ȡ���ӻ��ļĴ���
{
    u16 Regadd;
    u16 Reglen;
    u16 byte;
    u16 i,j;
    u16 crc;
    Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //�õ�Ҫ��ȡ�ļĴ������׵�ַ
    Reglen=modbus.rcbuf[4]*256+modbus.rcbuf[5];  //�õ�Ҫ��ȡ�ļĴ���������
    i=0;

    modbus.Sendbuf[i++]=modbus.myadd;//���豸��ַ
    modbus.Sendbuf[i++]=0x03;        //������
    byte=Reglen*2;   //Ҫ���ص������ֽ���
//modbus.Sendbuf[i++]=byte/256;  //
    modbus.Sendbuf[i++]=byte%256;

    for(j=0; j<Reglen; j++)
    {
        modbus.Sendbuf[i++]=Reg[Regadd+j]/256;
        modbus.Sendbuf[i++]=Reg[Regadd+j]%256;
    }
    crc=crc16(modbus.Sendbuf,i);
    modbus.Sendbuf[i++]=crc/256;  //
    modbus.Sendbuf[i++]=crc%256;


    for(j=0; j<i; j++)
    {
        Uart_SendByte(ID_UART3,modbus.Sendbuf[j]);
    }

}

/*******************************************************************************
** ��������: Modbud_fun6
** ��������: 6�Ź����룬�޸ļĴ�����ֵ
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Modbud_fun6(void)  //6�Ź����봦��
{
    u16 Regadd;
    u16 val;
    u16 i,crc,j;
    i=0;
    Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //�õ�Ҫ�޸ĵĵ�ַ
    val=modbus.rcbuf[4]*256+modbus.rcbuf[5];     //�޸ĺ��ֵ
    Reg[Regadd]=val;  //�޸ı��豸��Ӧ�ļĴ���
    //����Ϊ��Ӧ����
    modbus.Sendbuf[i++]=modbus.myadd;//���豸��ַ
    modbus.Sendbuf[i++]=0x06;        //������
    modbus.Sendbuf[i++]=Regadd/256;
    modbus.Sendbuf[i++]=Regadd%256;
    modbus.Sendbuf[i++]=val/256;
    modbus.Sendbuf[i++]=val%256;
    crc=crc16(modbus.Sendbuf,i);
    modbus.Sendbuf[i++]=crc/256;
    modbus.Sendbuf[i++]=crc%256;

    for(j=0; j<i; j++)
    {
        Uart_SendByte(ID_UART3,modbus.Sendbuf[j]);
    }

}

/*******************************************************************************
** ��������: Mosbus_Event
** ��������: ModBus��������
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Mosbus_Event(void)
{
    u16 crc;
    u16 rccrc;
    if(modbus.reflag==0)  //�յ�MODbus�����ݰ�
    {
        return;
    }
    crc= crc16(&modbus.rcbuf[0], modbus.recount-2);       //����У����
    rccrc=modbus.rcbuf[modbus.recount-2]*256 + modbus.rcbuf[modbus.recount-1];  //�յ���У����
    if(crc ==  rccrc)  //���ݰ�����CRCУ�����
    {
        if(modbus.rcbuf[0] == modbus.myadd)  //ȷ�����ݰ��Ƿ��Ƿ������豸��
        {
            switch(modbus.rcbuf[1])  //����������
            {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                Modbud_fun3();
                break;   //3�Ź����봦��
            case 4:
                break;
            case 5:
                break;
            case 6:
                Modbud_fun6();
                break;   //6�Ź����봦��
            case 7:
                break;
						case 10:
						break;
                //....
            }
        }
        else if(modbus.rcbuf[0] == 0)   //�㲥��ַ
        {

        }
    }
    modbus.recount=0;   //
    modbus.reflag=0;
}
/*******************************End of File************************************/


