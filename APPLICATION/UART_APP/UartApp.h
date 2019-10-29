/****************************************************************
***@file UartApp.h
***@brief 串口模块相关接口函数与变量声明文件
***@author  涂帅
***@version v2.0
***@date    2017.06.01

***<b>修改历史：\n</b>
***- 1. 2017.06.01 涂帅 创建文件

***<b>Copyright （c） 2017， 久信医疗科技有限公司</b>
*****************************************************************/
#include "FreeRTOSSysConfig.h"
#ifndef __UART_APP_H
#define __UART_APP_H
/*查询命令宏定义*/
#define TRACK_OCCUPY_QUERY_CMD 1///<0x03 01 轨道ID
#define ROUTE_APPLY_QUERY_CMD 2///<0x03 02 轨道ID
#define TRACK_ERR_QUERY_CMD 3///<0x03 03 轨道ID
#define MAP_TRACK_ERR_QUERY_CMD 4///<0x03 04 轨道ID
#define LOCAL_TRACK_VEH_NUM_QUERY_CMD 5///<0x03 05 轨道ID
#define MAP_TRACK_VEH_NUM_QUERY_CMD 6///<0x03 06 轨道ID
#define POWER_OVERLOAD_QUERY_CMD 7///<0x03 07 轨道ID
#define SCHED_STATE_QUERY_CMD 8///<0x03 08 转换器ID
#define SW_TRANS_TIME_QUERY_CMD 9///<0x03 09 00
#define SW_MEM_PERUSE_QUERY_CMD 10
#define TASK_STK_SIZE_QUERY_CMD 11
/*路径申请来源*/
#define SRC_FROM_STATION 1
#define SRC_FROM_TRACK 2
#define UART3_TASK_STK_SIZE 512
extern TaskHandle_t uart3Task_Handler;
#if (configUSE_DEBUG_PRINTF_EN == ENABLE)
extern void Uart4_Printf(u8 *string,u8 len);
#endif
#define UART4_TASK_STK_SIZE 512
extern TaskHandle_t uart4Task_Handler;
extern void Uart4_Task(void *pvParameters);
extern void Uart3_Task(void *pvParameters);
extern void Uart4_DataRcv(uint8_t *rxBuf);
extern void Uart4_TrackOccupyQuery(u8 trackId);
extern void Uart4_RouteApplyQuery(u8 trackId);
extern void Uart4_TrackErrQuery(u8 trackId);
extern void Uart4_MapTrackErrQuery(u8 trackId);
extern void Uart4_TrackVehNumQuery(u8 trackId);
extern void Uart4_MapTrackVehNumQuery(u8 trackId);
extern void Uart4_PowerOverloadQuery(u8 trackId);
extern void Uart4_SchedStateQuery(u8 T_Id);
extern void Uart4_SwTransTimeQuery(void);
extern void Uart4_MemPerUseQuery(u8 memx);
extern void Uart4_TaskStkSizeQuery(u8 taskPrio);
#endif
