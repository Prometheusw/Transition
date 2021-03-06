/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: ModBus.c
** 创建人员: 王凯
** 创建日期: 2019-07-31
** 文档描述: Modbus相关功能函数
********************************************************************************/
#include "myconfig.h"
u16 Reg[]= {0x0000,0x0000,0x0000,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,0x0001,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000/*20 警告代码*/,0x0000,/*21 ERRORCode*/0xffff,/*本转轨器编号*/0x0000,0x0000/*内存使用*/};
//14                                                                               10T1标定  T2     T3     T4
MODBUS modbus;
/*******************************************************************************
** 函数名称: Mosbus_Init
** 功能描述: MOdbus初始化设置
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/

void Mosbus_Init(void)
{
    Uart_HardwareInit(ID_UART3,UART_BPS_115200,USART_Parity_No);///<初始化UART3用于连接触摸屏
    modbus.myadd=4;  //本从设备的地址
    modbus.timrun=0; //MODbus定时器停止计时
}
/*******************************************************************************
** 函数名称: Modbud_fun3
** 功能描述: 3号功能码，读取寄存器的值
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/
void Modbud_fun3(void)  //3号功能码处理  ---主机要读取本从机的寄存器
{
    u16 Regadd;
    u16 Reglen;
    u16 byte;
    u16 i,j;
    u16 crc;
    Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //得到要读取的寄存器的首地址
    Reglen=modbus.rcbuf[4]*256+modbus.rcbuf[5];  //得到要读取的寄存器的数量
    i=0;

    modbus.Sendbuf[i++]=modbus.myadd;//本设备地址
    modbus.Sendbuf[i++]=0x03;        //功能码
    byte=Reglen*2;   //要返回的数据字节数
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
** 函数名称: Modbud_fun6
** 功能描述: 6号功能码，修改寄存器的值
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/
void Modbud_fun6(void)  //6号功能码处理
{
    u16 Regadd;
    u16 val;
    u16 i,crc,j;
    i=0;
    Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //得到要修改的地址
    val=modbus.rcbuf[4]*256+modbus.rcbuf[5];     //修改后的值
    Reg[Regadd]=val;  //修改本设备相应的寄存器
    //以下为回应主机
    modbus.Sendbuf[i++]=modbus.myadd;//本设备地址
    modbus.Sendbuf[i++]=0x06;        //功能码
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
** 函数名称: Mosbus_Event
** 功能描述: ModBus处理函数
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/
void Mosbus_Event(void)
{
    u16 crc;
    u16 rccrc;
    if(modbus.reflag==0)  //收到MODbus的数据包
    {
        return;
    }
    crc= crc16(&modbus.rcbuf[0], modbus.recount-2);       //计算校验码
    rccrc=modbus.rcbuf[modbus.recount-2]*256 + modbus.rcbuf[modbus.recount-1];  //收到的校验码
    if(crc ==  rccrc)  //数据包符号CRC校验规则
    {
        if(modbus.rcbuf[0] == modbus.myadd)  //确认数据包是否是发给本设备的
        {
            switch(modbus.rcbuf[1])  //分析功能码
            {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                Modbud_fun3();
                break;   //3号功能码处理
            case 4:
                break;
            case 5:
                break;
            case 6:
                Modbud_fun6();
                break;   //6号功能码处理
            case 7:
                break;
						case 10:
						break;
                //....
            }
        }
        else if(modbus.rcbuf[0] == 0)   //广播地址
        {

        }
    }
    modbus.recount=0;   //
    modbus.reflag=0;
}
/*******************************End of File************************************/



