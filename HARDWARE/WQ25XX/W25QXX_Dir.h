/****************************************************************
***@file W25QXX_Drv.h
***@brief 存储模块相关接口函数与变量声明文件
***@author  涂帅
***@version v2.0
***@date    2017.06.01

***<b>修改历史：\n</b>
***- 1. 2017.06.01 涂帅 创建文件

***<b>Copyright （c） 2017， 久信医疗科技有限公司</b>
*****************************************************************/
#ifndef __W25QXX_H
#define __W25QXX_H	
#include "includes.h"
#include "sys.h"
#define USE_W25QXX	0	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号		   

#define	W25QXX_CS 		PBout(12)  		//W25QXX的片选信号
#define MAX_W25QXX_BUF_SIZE 1024
////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25QXX_ADDR_BASE 0x000000
void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//读取FLASH ID
u8	 W25QXX_ReadSR(void);        		//读取状态寄存器 
void W25QXX_Write_SR(u8 sr);  			//写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒

void SPI1_Init(void);			 //初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节
void SPI2_Init(void);			 //初始化SPI1口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节
u16 CRC16(u8 *buf,u16 len);
bool ReadFlash(void);

extern u8 W25QXX_SendBuf[MAX_W25QXX_BUF_SIZE],W25QXX_RecvBuf[MAX_W25QXX_BUF_SIZE];
#endif
