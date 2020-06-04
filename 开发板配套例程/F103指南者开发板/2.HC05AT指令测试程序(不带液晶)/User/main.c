/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>

BLTDev bltDevList;
extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	  //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	HC05_INFO("**********HC05ģ��ATָ�����ʵ��************");
	
	if(HC05_Init() == 0)
		HC05_INFO("HC05ģ����������");
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		while(1);
	}
		
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*��ʼ��SPP�淶*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	while(1)
	{	
	  if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
		{			
			DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
			if(strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"AT"))//�����������AT��ͷ�ģ��Ͱ�KEY�øߣ���������ģ��
			{
				BLT_KEY_HIGHT;
				delay_ms(20);
				Usart_SendStr_length(BLT_USARTx,DEBUG_USART_ReceiveData.uart_buff,DEBUG_USART_ReceiveData.datanum);	
			  Usart_SendStr_length(BLT_USARTx,"\r\n",2);	
				BLT_KEY_LOW;
			}else if(strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"RED_LED"))
			{
				LED1_TOGGLE;
			}
			else
			{
				BLT_KEY_LOW;
				Usart_SendStr_length(BLT_USARTx,DEBUG_USART_ReceiveData.uart_buff,DEBUG_USART_ReceiveData.datanum);
			}
			DEBUG_USART_ReceiveData.receive_data_flag = 0;		//�������ݱ�־����
			DEBUG_USART_ReceiveData.datanum = 0;               
		}
		if(BLT_USART_ReceiveData.receive_data_flag == 1)
		{
			BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
			if(strstr((char *)BLT_USART_ReceiveData.uart_buff,"RED_LED"))//����������Լ�������Ҫ���յ��ַ���Ȼ����
			{
				LED1_TOGGLE; //������յ����ڵ������ֻ������ֻ������� ��RED_LED���ͻ�Ѱ�������ĺ��ȡ��һ��
			}
			else
			{			
				Usart_SendStr_length(DEBUG_USARTx,BLT_USART_ReceiveData.uart_buff,BLT_USART_ReceiveData.datanum); 
			  Usart_SendStr_length(DEBUG_USARTx,"\r\n",2);				
			}
        clean_rebuff();
		}
	}
}

/*********************************************END OF FILE**********************/
