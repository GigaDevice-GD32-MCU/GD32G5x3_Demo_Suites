/*!
    \file    main.c
    \brief   logic AND function interrupt of CLA0

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
#include "gd32g553r_eval.h"

void cla0_input_gpio_config();
void cla0_output_gpio_config(void);
void cla0_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();
    /* configure LED */
    gd_eval_led_init(LED1);
    /* configure the CLA handler priority */
    nvic_irq_enable(CLA_IRQn, 1, 1);
    /* configure cla0 input GPIO */
    cla0_input_gpio_config();
    /* configure cla0 output GPIO */
    cla0_output_gpio_config();
    /* reset CLA */
    cla_deinit();
    /* CLA0 configuration */
    cla0_config();

    while(1){
        delay_1ms(200);
        /* toggle PB5 */
        gpio_bit_toggle(GPIOB,GPIO_PIN_5);
    }
}

/*!
    \brief      configure the CLA0 input GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_input_gpio_config()
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure PB5 as output */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_60MHZ, GPIO_PIN_5);
    
    /* configure PB6 as output */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_6);
    /* PB6 outputs HIGH */
    gpio_bit_set(GPIOB, GPIO_PIN_6);
    gpio_bit_reset(GPIOB, GPIO_PIN_5);
}

/*!
    \brief      configure the CLA0OUT GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_output_gpio_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* connect PA2 to CLA0OUT */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_2);
    /* configure GPIO pins of CLA0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ,GPIO_PIN_2);
}

/*!
    \brief      configure the CLA0 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cla0_config(void)
{
    /* enable CLA clock */
    rcu_periph_clock_enable(RCU_CLA);
    /* select CLAIN4(PB6) as input of SIGS0 */
    cla_sigs_input_config(CLA0, SIGS0, CLA0SIGS0_CLAIN4);
    /* select CLAIN3(PB5) as input of SIGS1 */
    cla_sigs_input_config(CLA0, SIGS1, CLA0SIGS1_CLAIN3);
    /* IN0 & IN1 */
    cla_lcu_control_config(CLA0, 0xC0);
    /* flip-flop output is selected as CLA output */
    cla_output_config(CLA0, FLIP_FLOP_OUTPUT);
    /* HCLK is selected as clock source of flip-flop */
    cla_flip_flop_clocksource_config(CLA0, HCLK);
    /* rising edge is selected as clock polarity of flip-flop */
    cla_flip_flop_clockpolarity_config(CLA0, CLA_CLOCKPOLARITY_POSEDGE);
    /* reset the flip-flop output */
    cla_flip_flop_output_reset(CLA0);
    /* enable CLA0 rising edge interrupt */
    cla_posedge_interrupt_enable(CLA0PIE);
    /* enable CLA0 output */
    cla_output_enable(CLA0);
    /* enable CLA0 */
    cla_enable(CLA0);
}
