/****************************************************************
***@file Uart_Drv.h
***@brief 用于UART驱动的函数与变量的声明文件
***@author  涂帅
***@version v2.0
***@date    2017.06.01

***<b>修改历史：\n</b>
***- 1. 2017.06.01 涂帅 创建文件

***<b>Copyright （c） 2017， 久信医疗科技有限公司</b>
*****************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "delay.h"
#ifndef __UART_DRV_H
#define __UART_DRV_H
#define MAX_UART_NUM 6
/*串口工作状态*/
typedef enum _UART_STA
{
	UART_IDLE = 0,
	UART_RECEVING,
	UART_RECEIVE_DONE,
	UART_SENDING,
	UART_SEND_DONE
}UART_STA;
typedef struct _UART_STATUS
{
	u32 uartReciveSta:3,
uartSendSta:3,
uartReceiveByteIndex:8,
uartSendByteIndex:8,
uartSendDataLen:8;
}UART_STATUS;
extern UART_STATUS g_uartSta[MAX_UART_NUM];
/*串口ID*/
typedef enum _UART_ID
{
	ID_UART1,
	ID_UART2,
	ID_UART3,
	ID_UART4,
	ID_UART5,
	ID_UART6
}UART_ID;
/*串口波特率*/
typedef enum _UART_BPS
{
	UART_BPS_1200 = 1200,
	UART_BPS_2400 = 2400,
	UART_BPS_4800 = 4800,
	UART_BPS_9600 = 9600,
	UART_BPS_14400 = 14400,
	UART_BPS_19200 = 19200,
	UART_BPS_38400 = 38400,
	UART_BPS_56000 = 56000,
	UART_BPS_57600 = 57600,
	UART_BPS_115200 = 115200,
	UART_BPS_128000 = 128000,
	UART_BPS_256000 = 256000
}UART_BPS;
/*串口校验位*/
#define UART_PARITY_NONE USART_Parity_No
#define UART_PARITY_EVEN USART_Parity_Even
#define UART_PARITY_ODD USART_Parity_Odd
#define UART1_USE_GPIOA_EN 1
#define UART1_USE_GPIOB_EN 0
#define UART2_USE_GPIOA_EN 1
#define UART2_USE_GPIOD_EN 0
#define UART3_USE_GPIOB_EN 1
#define UART3_USE_GPIOD_EN 0
#define UART3_USE_GPIOC_EN 0
#define UART4_USE_GPIOA_EN 0
#define UART4_USE_GPIOC_EN 1
#define UART6_USE_GPIOC_EN 0
#define UART6_USE_GPIOG_EN 0
#define MAX_UART_RXBUF_SIZE 256
#define MAX_UART_TXBUF_SIZE 256
extern u8 uart4RxBuf[MAX_UART_RXBUF_SIZE],uart3RxBuf[MAX_UART_RXBUF_SIZE];
extern u8 uart4TxBuf[MAX_UART_TXBUF_SIZE],uart3TxBuf[MAX_UART_TXBUF_SIZE];
extern void Uart_SendByte(u8 uartId,u8 byte);
extern void Uart_SendBytes(u8 uartId,u8 *txBuf,u8 len,u8 startIndex);
extern u8 Uart_ReceiveByte(u8 uartId);
extern void Uart_ReceiveBytes(u8 uartId,u8 *rxBuf,u8 len);
extern void Uart_HardwareInit(u8 uartId,float bps,u16 parity);
#endif
