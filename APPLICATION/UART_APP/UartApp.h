/****************************************************************
***@file UartApp.h
***@brief ����ģ����ؽӿں�������������ļ�
***@author  Ϳ˧
***@version v2.0
***@date    2017.06.01

***<b>�޸���ʷ��\n</b>
***- 1. 2017.06.01 Ϳ˧ �����ļ�

***<b>Copyright ��c�� 2017�� ����ҽ�ƿƼ����޹�˾</b>
*****************************************************************/
#include "FreeRTOSSysConfig.h"
#ifndef __UART_APP_H
#define __UART_APP_H
/*��ѯ����궨��*/
#define TRACK_OCCUPY_QUERY_CMD 1///<0x03 01 ���ID
#define ROUTE_APPLY_QUERY_CMD 2///<0x03 02 ���ID
#define TRACK_ERR_QUERY_CMD 3///<0x03 03 ���ID
#define MAP_TRACK_ERR_QUERY_CMD 4///<0x03 04 ���ID
#define LOCAL_TRACK_VEH_NUM_QUERY_CMD 5///<0x03 05 ���ID
#define MAP_TRACK_VEH_NUM_QUERY_CMD 6///<0x03 06 ���ID
#define POWER_OVERLOAD_QUERY_CMD 7///<0x03 07 ���ID
#define SCHED_STATE_QUERY_CMD 8///<0x03 08 ת����ID
#define SW_TRANS_TIME_QUERY_CMD 9///<0x03 09 00
#define SW_MEM_PERUSE_QUERY_CMD 10
#define TASK_STK_SIZE_QUERY_CMD 11
/*·��������Դ*/
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
