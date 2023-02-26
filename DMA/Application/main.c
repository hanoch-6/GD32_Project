

/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "common.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
#ifdef __GNUC__

  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#else

  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif /* __GNUC__ */
int main(void)
{
    /* configure systick */
    //systick_config();
    /* initilize the LEDs, USART and key */
    gd_eval_com_init(USART0);
    // gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    // SystemInit();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	taskENTER_CRITICAL();
	xTaskCreate(Led_task_init,"Task LED",configMINIMAL_STACK_SIZE,NULL,1,NULL);
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    // printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    // printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    // printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    // printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));

    vTaskStartScheduler();
    while(1){
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
		vTaskDelay(1000);
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
        }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

    return ch;
}
