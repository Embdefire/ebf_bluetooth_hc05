/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05蓝牙模块测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./hc05/bsp_hc05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>

BLTDev bltDevList;
extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	  //初始化systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
		
	ILI9341_Init();
	
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	HC05_INFO("**********HC05模块AT指令测试实验************");
	ILI9341_DispString_EN ( 40, 20, "HC05 BlueTooth Demo" );	
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05模块检测正常。");
		ILI9341_DispString_EN ( 40, 40, "HC05 module detected!" );
	}
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
		ILI9341_DispString_EN ( 20, 40, "No HC05 module detected!"  );
		ILI9341_DispString_EN ( 5, 60, "Please check the hardware connection and reset the system." );
		while(1);
	}
	ILI9341_DispStringLine_EN ((LINE(4)), "ReceiveData USART1:"  );	
	ILI9341_DispStringLine_EN ((LINE(9)), "ReceiveData HC-05:"  );	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*初始化SPP规范*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*设置模块名字*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("本模块名字为:%s ,模块已准备就绪。",hc05_name);
	ILI9341_DispStringLine_EN ( (LINE(4)), "ReceiveData USART1" );	
  ILI9341_DispStringLine_EN ( (LINE(9)), "ReceiveData HC-05" );	
	while(1)
	{	
	  if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
		{			
			DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
			if(strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"AT"))//如果数据是以AT开头的，就把KEY置高，设置蓝牙模块
			{
				BLT_KEY_HIGHT;
				delay_ms(20);
				Usart_SendStr_length(BLT_USARTx,DEBUG_USART_ReceiveData.uart_buff,DEBUG_USART_ReceiveData.datanum);	
			  Usart_SendStr_length(BLT_USARTx,"\r\n",2);	
				BLT_KEY_LOW
			}else if(strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"RED_LED"))
			{
				LED1_TOGGLE;
			}
			else
			{
				BLT_KEY_LOW;
				Usart_SendStr_length(BLT_USARTx,DEBUG_USART_ReceiveData.uart_buff,DEBUG_USART_ReceiveData.datanum);
			}
		  	LCD_ClearLine(LINE(5));
	  		LCD_ClearLine(LINE(6));
		  	LCD_ClearLine(LINE(7));
		  	LCD_ClearLine(LINE(8));
		  	ILI9341_DispStringLine_EN ( (LINE(5)), (char *)DEBUG_USART_ReceiveData.uart_buff );
		  	DEBUG_USART_ReceiveData.receive_data_flag = 0;		//接收数据标志清零
		  	DEBUG_USART_ReceiveData.datanum = 0;               
		}
		if(BLT_USART_ReceiveData.receive_data_flag == 1)
		{
			  BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
			if(strstr((char *)BLT_USART_ReceiveData.uart_buff,"RED_LED"))//在这里可以自己定义想要接收的字符串然后处理
			{
				LED1_TOGGLE; //这里接收到串口调试助手或者是手机发来的 “RED_LED”就会把板子上面的红灯取反一次	
			}
			else
			{			
				Usart_SendStr_length(DEBUG_USARTx,BLT_USART_ReceiveData.uart_buff,BLT_USART_ReceiveData.datanum); 
			  Usart_SendStr_length(DEBUG_USARTx,"\r\n",2);					
			}		
        LCD_ClearLine(LINE(10));
		  	LCD_ClearLine(LINE(11));
		  	LCD_ClearLine(LINE(12));
		  	LCD_ClearLine(LINE(13));
			  ILI9341_DispStringLine_EN ( (LINE(10)), (char *)BLT_USART_ReceiveData.uart_buff );
		  	clean_rebuff();
		}
	}
}

/*********************************************END OF FILE**********************/
