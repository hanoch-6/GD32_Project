/**
 * @file GD_Task.c
 * @author HuangMiaozhi (hanoch1024@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-26
 * 
 * @attention
 * Jinxin Microelec co.
 * 
 */
 
#include "common.h"

void Led_task_init(void *arg)
{

    //todo 2023.2.26 调度器能正常工作，但是gpio亮灭无法设置
	rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    //gpio_bit_set(GPIOC,GPIO_PIN_13);
	printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
	while(1)
	{
		gpio_bit_write(GPIOC,GPIO_PIN_13,RESET);
		vTaskDelay(2000);
		printf("\r\HUANGMIAOZHI");
		vTaskDelay(2000);
		printf("\r\Hanoch");
		gpio_bit_reset(GPIOC,GPIO_PIN_13);
	}
}

