/*!
    \file    main.c
    \brief   TMU calculation demo

    \version 2026-02-10, V1.4.0, demo for GD32G5x3
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#include "gd32g5x3.h"
#include "systick.h"
#include <stdio.h>
#include <stdlib.h>
#include "gd32g553q_eval.h"

#define RXBUFFER    11

/* USART received buffer */
char rxbuffer[RXBUFFER];
/* USART received count */
__IO uint8_t rxcount = 0;
/* TMU input data in floating point format */
float in_data_f32[2] = {0};
/* TMU output data in floating point format */
float out_data_f32[2] = {0};

/* led init function */
void led_init(void);
/* configure RCU */
void rcu_config(void);
/* configure TMU input and output data in floating point format */
void tmu_config_f32(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    /* configure the led GPIO */
    led_init();
    /* config the USART */
    gd_eval_com_init(EVAL_COM);
    /* enable USART TBE interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0, 0);

    /* configure RCU */
    rcu_config();
    /* configure TMU input and output data in floating point format */
    tmu_config_f32();

    printf("\n\r TMU Caculation Test: m*cos(theta)");
    printf("\n\r Please input the first value: theta/pi (-2^24, 2^24)");
    while(rxcount == 0);
    /* insert 1s delay */
    delay_1ms(1000);
    /* read the first input data */
    in_data_f32[0] = strtof(rxbuffer, NULL);

    if((in_data_f32[0] >= 16777216.0f)||(in_data_f32[0] <= -16777216.0f)){
        printf("\n\r the first input data is out of range ");
    }else{
        printf("\n\r Please input the second value: m ");
        rxcount = 0;
        for(int i=0;i<RXBUFFER;i++){
            rxbuffer[i] = 0;
        }
        while(rxcount == 0);
        /* insert 1s delay */
        delay_1ms(1000);
        usart_interrupt_disable(USART0, USART_INT_RBNE);
        /* read the second input data */
        in_data_f32[1] = strtof(rxbuffer, NULL);

        /* configure TMU input and output data in floating point format */
        tmu_config_f32();
        /* write data to start TMU */
        tmu_two_f32_write(in_data_f32[0], in_data_f32[1]);
        /* read two output data */
        tmu_two_f32_read(&out_data_f32[0], &out_data_f32[1]);
        /* check the TMU overflow flag */
        if(SET == tmu_flag_get(TMU_FLAG_OVRF)){
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            printf("\n\r The TMU calculation is overflow.");
        }else{
            gd_eval_led_on(LED3);
            gd_eval_led_on(LED4);
            printf("\n\r The TMU calculation result is:%11.3f",out_data_f32[0]);
        }
    }

    while(1);
}

/*!
    \brief      led init function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    /* initialize the leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* close all of leds */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      configure RCU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_TMU);
}

/*!
    \brief      configure TMU input and output data in floating point format
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tmu_config_f32(void)
{
    tmu_parameter_struct tmu_init_struct;

    tmu_struct_para_init(&tmu_init_struct);
    /* reset the TMU */
    tmu_deinit();
    /* configure TMU peripheral */
    tmu_init_struct.mode = TMU_MODE_COS;
    tmu_init_struct.scale = TMU_SCALING_FACTOR_1;
    tmu_init_struct.output_floating = TMU_OUTPUT_FLOAT_ENABLE;
    tmu_init_struct.input_floating = TMU_INPUT_FLOAT_ENABLE;
    tmu_init_struct.dma_read = TMU_READ_DMA_DISABLE;
    tmu_init_struct.dma_write = TMU_WRITE_DMA_DISABLE;
    tmu_init_struct.read_times = TMU_READ_TIMES_2;
    tmu_init_struct.write_times = TMU_WRITE_TIMES_2;
    tmu_init_struct.output_width = TMU_OUTPUT_WIDTH_32;
    tmu_init_struct.input_width = TMU_INPUT_WIDTH_32;
    tmu_init(&tmu_init_struct);
}
