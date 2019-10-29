/****************************************************************
***@file UartApp.c
***@brief ����ģ����ؽӿں�������������ļ�
***@author  Ϳ˧
***@version v2.0
***@date    2017.06.01

***<b>�޸���ʷ��\n</b>
***- 1. 2017.06.01 Ϳ˧ �����ļ�

***<b>Copyright ��c�� 2017�� ����ҽ�ƿƼ����޹�˾</b>
*****************************************************************/
#include "myconfig.h"
//TaskHandle_t uart3Task_Handler,uart4Task_Handler;
/****************************************************************
���ܣ�UART4��Ϣ��ӡ����
��ڲ�����*string-��Ҫ��ӡ����Ϣָ��,,u8 len-��Ҫ��ӡ����Ϣ����
����ֵ����
*****************************************************************/
#if (configUSE_DEBUG_PRINTF_EN == 1)
void Uart4_Printf(u8 *string,u8 len)
{
	memcpy(uart4TxBuf,string,len);
	g_uartSta[ID_UART4].uartSendDataLen = len;
	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
}
#endif
/****************************************************************
���ܣ�UART3�����жϷ�����
��ڲ�������
����ֵ����
*****************************************************************/	
void USART3_IRQHandler(void)                	
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		Uart_ReceiveByte(ID_UART3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		if (g_uartSta[ID_UART3].uartReciveSta == UART_IDLE)
		{
			g_uartSta[ID_UART3].uartReciveSta = UART_RECEVING;
			g_uartSta[ID_UART3].uartReceiveByteIndex = 0;
		}
		else if(g_uartSta[ID_UART3].uartReciveSta == UART_RECEIVE_DONE)
		{
			g_uartSta[ID_UART3].uartReceiveByteIndex = 0;
		}
		if(g_uartSta[ID_UART3].uartReciveSta == UART_RECEVING)
		{
			MB_UartRecevieByte(ID_UART3,uart3RxBuf);
		} 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	} 
	else if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_TC);
		Uart_SendBytes(ID_UART3,uart3TxBuf,g_mbADUData.ADULen - 1,1);
	}
}
/****************************************************************
���ܣ�UART4���ݽ��պ���
��ڲ�����uint8_t *rxBuf-���ݽ��ջ���ָ��
����ֵ����
*****************************************************************/	
void Uart4_DataRcv(uint8_t *rxBuf)
{
	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	static u8 dataLen = 0;
	*(rxBuf + dataLen) = Uart_ReceiveByte(ID_UART4);
	if (*(rxBuf + dataLen) == 0xFF)
	{
		dataLen = 0;
	}
	dataLen++;
	if (dataLen >= 3)
	{
		dataLen = 0;
		vTaskNotifyGiveFromISR(uart4Task_Handler, &HigherPriorityTaskWoken );
		portYIELD_FROM_ISR( HigherPriorityTaskWoken );	
	}
}
/****************************************************************
���ܣ�UART4�����жϷ�����
��ڲ�������
����ֵ����
*****************************************************************/	
void UART4_IRQHandler(void)                	
{		
	if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
	{
		Uart_ReceiveByte(ID_UART4);
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
	}
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		Uart4_DataRcv(uart4RxBuf);
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
	} 
	else if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(UART4,USART_IT_TC);
		Uart_SendBytes(ID_UART4,uart4TxBuf,g_uartSta[ID_UART4].uartSendDataLen - 1,1);
	}
}

///****************************************************************
//���ܣ�UART3������
//��ڲ�����void *pvParameters-�������ָ��
//����ֵ����
//*****************************************************************/	
//void Uart3_Task(void *pvParameters)
//{
//	while(1)
//	{
//		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//		if (MB_SlaveAppDeal(uart3RxBuf,uart3TxBuf,g_uartSta[ID_UART3].uartReceiveByteIndex) == MB_NO_ERR)
//		{
//			Uart_SendByte(ID_UART3,uart3TxBuf[0]);
//		} 
//		else
//		{
//			Uart_SendByte(ID_UART3,uart3TxBuf[0]);
//		} 
//		vTaskDelay(100);
//	}
//}
///****************************************************************
//���ܣ�UART4���ռ�ò�ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/	
//void Uart4_TrackOccupyQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].occupySize;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4·�������ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_RouteApplyQuery(u8 trackId)
//{
//	ROUTE_APPLY_VEH_LINK_TABLE *routeApplyVehInfoNode = NULL;
//	uart4TxBuf[0] = trackId;
//	g_uartSta[ID_UART4].uartSendDataLen = 1;
//	routeApplyVehInfoNode = g_routeApplyVehInfo[trackId - 1];
//	while (routeApplyVehInfoNode->nextInfoNode != NULL)
//	{
//		memcpy(uart4TxBuf + 1,&routeApplyVehInfoNode->nextInfoNode->vehId,3);
//		g_uartSta[ID_UART4].uartSendDataLen += 3;
//		routeApplyVehInfoNode = routeApplyVehInfoNode->nextInfoNode;
//	}
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4������ϲ�ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_TrackErrQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].errState;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4ӳ�������ϲ�ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_MapTrackErrQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].mapTrackErrCauseErr;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4�����������ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_TrackVehNumQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].localCacheSize;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4ӳ������������ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_MapTrackVehNumQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].mapTrackCacheSize;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4��Դ���޲�ѯ����
//��ڲ�����u8 trackId-���ID
//����ֵ����
//*****************************************************************/
//void Uart4_PowerOverloadQuery(u8 trackId)
//{
//	uart4TxBuf[0] = trackId;
//	uart4TxBuf[1] = g_trackStateInfo[trackId - 1].pwOverload;
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4����״̬��ѯ����
//��ڲ�����u8 T_Id-ת����ID
//����ֵ����
//*****************************************************************/
//void Uart4_SchedStateQuery(u8 T_Id)
//{
//	u8 i = 0;
//	if (T_Id == 0)
//	{
//		uart4TxBuf[0] = T_Id;
//		g_uartSta[ID_UART4].uartSendDataLen = 1;
//		for (i = 0;i < RSHIFT(g_configInfo->baseCfg.workMode.workMode & 0xF0,4);i++)
//		{
//			uart4TxBuf[i * 2] = i + 1;
//			uart4TxBuf[i * 2 + 1] = g_schedVehInfo[i].schedState;
//			g_uartSta[ID_UART4].uartSendDataLen = (i + 1) * 2;
//		}			
//	}
//	else
//	{
//		uart4TxBuf[0] = T_Id;
//		uart4TxBuf[1] = g_schedVehInfo[T_Id - 1].schedState;
//		g_uartSta[ID_UART4].uartSendDataLen = 2;
//	}
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4ת����ת��������ѯ����
//��ڲ�������
//����ֵ����
//*****************************************************************/
//void Uart4_SwTransTimeQuery(void)
//{
//	uart4TxBuf[0] = (g_markBit.swTimes % 256);
//	uart4TxBuf[1] = (g_markBit.swTimes / 256);
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4ת�����ڴ�ʹ���ʲ�ѯ����
//��ڲ�������
//����ֵ����
//*****************************************************************/
//void Uart4_MemPerUseQuery(u8 memx)
//{
//	uart4TxBuf[0] = memx;
//	uart4TxBuf[1] = my_mem_perused(memx);
//	g_uartSta[ID_UART4].uartSendDataLen = 2;
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}
///****************************************************************
//���ܣ�UART4�����ջʹ���ʲ�ѯ����
//��ڲ�����u8 taskPrio-�������ȼ�
//����ֵ����
//*****************************************************************/
//void Uart4_TaskStkSizeQuery(u8 taskPrio)
//{
//	UBaseType_t taskStkUnuse = 0;
//	switch (taskPrio)
//	{
//		case UART4_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(uart4Task_Handler);
//			break;
//		case SW_PRE_CTL_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(swPreCtlTask_Handler);
//			break;
//		case VEH_INFO_UPLOAD_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(vehInfoUploadTask_Handler);
//			break;
//		case STEPMOTOR_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(stepMotorTask_Handler);
//			break;
//		case SW_STATE_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(swStateTask_Handler);
//			break;
//		case SCHED_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(schedTask_Handler);
//			break;
//		case CAN2_ASSI_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can2AssiTask_Handler);
//			break;
//		case CAN1_DATA_DISTRIBUTE_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can1DataDistributeTask_Handler);
//			break;
//		case CAN2_DATA_DISTRIBUTE_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can2DataDistributeTask_Handler);
//			break;
//		case CAN1_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can1Task_Handler);
//			break;
//		case CAN2_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can2Task_Handler);
//			break;
//		case CAN1_ASSI_TASK_PRIO:
//			taskStkUnuse = uxTaskGetStackHighWaterMark(can1AssiTask_Handler);
//			break;
//		default:
//			break;
//	}
//	memcpy(uart4TxBuf,&taskStkUnuse,sizeof(taskStkUnuse));
//	g_uartSta[ID_UART4].uartSendDataLen = sizeof(taskStkUnuse);
//	Uart_SendByte(ID_UART4,uart4TxBuf[0]);
//}

///****************************************************************
//���ܣ�UART4������
//��ڲ�����void *pvParameters-�������ָ��
//����ֵ����
//*****************************************************************/	
//void Uart4_Task(void *pvParameters)
//{
//	while(1)
//	{
//		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//		switch (uart4RxBuf[1])
//		{
//			case TRACK_OCCUPY_QUERY_CMD:
//				Uart4_TrackOccupyQuery(uart4RxBuf[2]);
//				break;
//			case ROUTE_APPLY_QUERY_CMD:
//				Uart4_RouteApplyQuery(uart4RxBuf[2]);
//				break;
//			case TRACK_ERR_QUERY_CMD:
//				Uart4_TrackErrQuery(uart4RxBuf[2]);
//				break;
//			case MAP_TRACK_ERR_QUERY_CMD:
//				Uart4_MapTrackErrQuery(uart4RxBuf[2]);
//				break;
//			case LOCAL_TRACK_VEH_NUM_QUERY_CMD:
//				Uart4_TrackVehNumQuery(uart4RxBuf[2]);
//				break;
//			case MAP_TRACK_VEH_NUM_QUERY_CMD:
//				Uart4_MapTrackVehNumQuery(uart4RxBuf[2]);
//				break;
//			case POWER_OVERLOAD_QUERY_CMD:
//				Uart4_PowerOverloadQuery(uart4RxBuf[2]);
//				break;
//			case SCHED_STATE_QUERY_CMD:
//				Uart4_SchedStateQuery(uart4RxBuf[2]);
//				break;
//			case SW_TRANS_TIME_QUERY_CMD:
//				Uart4_SwTransTimeQuery();
//				break;
//			case SW_MEM_PERUSE_QUERY_CMD:
//				Uart4_MemPerUseQuery(uart4RxBuf[2]);
//				break;
//			case TASK_STK_SIZE_QUERY_CMD:
//				Uart4_TaskStkSizeQuery(uart4RxBuf[2]);
//				break;
//			default:
//				break;
//		}
//		vTaskDelay(500);
//	}
//}
