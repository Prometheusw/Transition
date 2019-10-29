/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: Uart_Drv.c
** ������Ա: ����
** ��������: 2019-07-31
** �ĵ�����: ���ڵ����� 3  4
********************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** ��������: Uart_HardwareInit
** ��������: ���ڳ�ʼ������
** ����˵��: uartId: ���ں�
**			 bps: ������
**			 parity: У�鷽ʽ
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Uart_HardwareInit(u8 uartId,float bps,u16 parity)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
//USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bps;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = parity;//��żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    switch (uartId)
    {
    case ID_UART3:
    {   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
        //����3��Ӧ���Ÿ���ӳ��
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
        //USART3�˿�����
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
        GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB10��PB11}

        USART_Init(USART3, &USART_InitStructure); //��ʼ������3
        USART_Cmd(USART3, DISABLE);  //ʹ�ܴ���3
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
        USART_ClearFlag(USART3, USART_FLAG_TC);

        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
        //USART_ITConfig(USART3, USART_IT_TC, ENABLE);//��������ж�

        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;//��ռ���ȼ�9
        NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
        NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

    }
    break;

    case ID_UART4:
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��USART4ʱ��
        //����4��Ӧ���Ÿ���ӳ��
        GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10����ΪUSART4
        GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11����ΪUSART4
        //USART4�˿�����
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10��GPIOC11
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
        GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC10��PC11
        USART_Init(UART4, &USART_InitStructure); //��ʼ������4
        USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���4

        while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
        USART_ClearFlag(UART4, USART_FLAG_TC);
        USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�
        /// USART_ITConfig(UART4, USART_IT_TC, ENABLE);//��������ж�

        //USART_ClearFlag(UART4, USART_FLAG_TC);
        NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����4�ж�ͨ��
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
        NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
        NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

    }
    break;
    default:
        break;
    }
}
/*******************************************************************************
** ��������: Uart_SendByte
** ��������: ���ڷ���һ���ֽں���
** ����˵��: uartId: ���ں�
**			 byte: �ֽ�
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Uart_SendByte(uint8_t uartId,uint8_t byte)
{
    switch (uartId)
    {
    case ID_UART4:
        USART_SendData(UART4,byte);
        while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);
        USART_ClearFlag(UART4,USART_FLAG_TC );
        break;
    case ID_UART3:
        USART_SendData(USART3,byte);
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
        USART_ClearFlag(USART3,USART_FLAG_TC );
        //g_uartSta[uartId].uartSendSta = UART_SENDING;
        break;
    default:
        break;
    }
}
/*******************************************************************************
** ��������: Uart_ReceiveByte
** ��������: ���ڽ���һ���ֽ�
** ����˵��: uartId: ���ں�
** ����˵��: uint8_t�����յ����ֽ�
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
uint8_t Uart_ReceiveByte(uint8_t uartId)
{
    static u8 ch = 0;
    switch (uartId)
    {
    case ID_UART4:
        ch = (UART4->DR);
        break;
    case ID_UART3:
        ch = (USART3->DR);
        break;
    default:
        ch = 0;
        break;
    }
    return ch;
}
/*******************************************************************************
** ��������: USART3_IRQHandler
** ��������: �����жϺ���
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void USART3_IRQHandler(void)
{
    u8 sbuf;
    OSIntEnter();//��CPU�����ж�Ƕ�ײ���
    if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
    {
        Uart_ReceiveByte(ID_UART3);
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        sbuf=USART3->DR;
        if( modbus.reflag==1)  //�����ݰ����ڴ���
        {
            return;
        }
        modbus.rcbuf[modbus.recount++]=sbuf;
        modbus.timout=0;
        if(modbus.recount==1)  //�յ�����������һ֡���ݵĵ�һ�ֽ�
        {
            modbus.timrun=1;  //������ʱ
        }
    }
    OSIntExit();

}
/*******************************************************************************
** ��������: UART4_IRQHandler
** ��������: ����4�жϺ����������û���õ�
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void UART4_IRQHandler(void)
{
    USART_ClearFlag(UART4, USART_FLAG_TC);
}
/*****************************End of Head************************************/

//int fputc(int ch, FILE *f)
//{
//	USART_SendData(USART3,(uint8_t)ch);
//	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET)
//		
//	{
//	}
//	return ch;
//}