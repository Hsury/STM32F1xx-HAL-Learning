#include "wkup.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//待机唤醒 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//系统进入待机模式
void Sys_Enter_Standby(void)
{
    __HAL_RCC_APB2_FORCE_RESET();       //复位所有IO口 
   	__HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
			  	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //设置WKUP用于唤醒
    HAL_PWR_EnterSTANDBYMode();                         //进入待机模式     
}
 
     
//检测WKUP脚的信号
//返回值1:连续按下3s以上
//      0:错误的触发	
u8 Check_WKUP(void) 
{
	u8 t=0;	//记录按下的时间
	LED0=0; //亮灯DS0 
	while(1)
	{
		if(WKUP_KD)
		{
			t++;			//已经按下了 
			delay_ms(30);
			if(t>=100)		//按下超过3秒钟
			{
				LED0=0;	 	//点亮DS0 
				return 1; 	//按下3s以上了
			}
		}else 
		{ 
			LED0=1;
			return 0; //按下不足3秒
		}
	}
}  

//外部中断线0中断服务函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

//中断线0中断处理过程
//此函数会被HAL_GPIO_EXTI_IRQHandler()调用
//GPIO_Pin:引脚
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_0)//PA0
    {
        if(Check_WKUP())//关机
        {
            Sys_Enter_Standby();//进入待机模式
        }
    }    
}

//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;  //中断,上升沿
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //快速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    //检查是否是正常开机
    if(Check_WKUP()==0)
    {
        Sys_Enter_Standby();//不是开机，进入待机模式
    }

    HAL_NVIC_SetPriority(EXTI0_IRQn,0x02,0x02);//抢占优先级2，子优先级2
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
