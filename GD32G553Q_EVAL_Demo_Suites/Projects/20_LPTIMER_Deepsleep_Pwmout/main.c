/*!
    \file    main.c
    \brief   running led

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
#include <stdio.h>
#include "systick.h"
#include "gd32g553q_eval.h"

/* enable the peripheral clock */
void rcu_config(void);
/* configure the GPIO ports */
void gpio_config(void);
/* configure the NVIC */
void nvic_config(void);
/* configure the LPTIMER peripheral */
void lptimer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* systick configure */
    systick_config();
    /* initialize LED1 */
    gd_eval_led_init(LED1);
    /* wakeup key configure */
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);

    /* RCU configure */
    rcu_config();
    /* GPIO configure */
    gpio_config();
    /* NVIC configure */
    nvic_config();
    /* LPTIMER configure */
    lptimer_config();

    while(1) {
        if(RESET == gpio_input_bit_get(KEY_A_GPIO_PORT, KEY_A_PIN)) {
            pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
        }
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* LPTIMER GPIO RCU */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* LPTIMER clock */
    rcu_osci_on(RCU_IRC32K);
    rcu_osci_stab_wait(RCU_IRC32K);
    rcu_lptimer_clock_config(RCU_LPTIMERSRC_IRC32K);
    rcu_periph_clock_enable(RCU_LPTIMER);
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* LPTIMER GPIO */
    /*configure PC1(LPTIMER_O) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_1);
}

/*!
    \brief      configure the NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(LPTIMER_IRQn, 0U, 0U);

    /* LPTIMER wakeup mcu through EXTI line 35 */
    /* initialize the EXTI line 35 */
    exti_init(EXTI_35, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_35);
}

/*!
    \brief      configure the LPTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lptimer_config(void)
{
    /* ----------------------------------------------------------------------------
    LPTIMER Configuration:
    LPTIMER count with internal clock IRC32K, the prescaler is 16, the period is 10000.

    LPTIMER configuration: generate 1 PWM signal with the duty cycle:
    LPTIMERCLK = IRC32K / 16 = 2KHz

    LPTIMER_O duty cycle = (5000/ 10000)* 100  = 50%
    ---------------------------------------------------------------------------- */
    /* LPTIMER configuration */
    lptimer_parameter_struct lptimer_structure;
    /* deinit a LPTIMER */
    lptimer_deinit();
    /* initialize LPTIMER init parameter struct */
    lptimer_struct_para_init(&lptimer_structure);
    /* LPTIMER configuration */
    lptimer_structure.clocksource      = LPTIMER_INTERNALCLK;
    lptimer_structure.prescaler        = LPTIMER_PSC_16;
    lptimer_structure.extclockpolarity = LPTIMER_EXTERNALCLK_RISING;
    lptimer_structure.extclockfilter   = LPTIMER_EXTERNALCLK_FILTEROFF;
    lptimer_structure.triggermode      = LPTIMER_TRIGGER_SOFTWARE;
    lptimer_structure.extriggersource  = LPTIMER_EXTRIGGER_GPIO;
    lptimer_structure.extriggerfilter  = LPTIMER_TRIGGER_FILTEROFF;
    lptimer_structure.outputpolarity   = LPTIMER_OUTPUT_NOTINVERTED;
    lptimer_structure.outputmode       = LPTIMER_OUTPUT_PWMORSINGLE;
    lptimer_structure.countersource    = LPTIMER_COUNTER_INTERNAL;
    lptimer_init(&lptimer_structure);

    lptimer_register_shadow_disable();
    lptimer_timeout_disable();
    lptimer_inputremap(LPTIMER_INPUT0_GPIO, LPTIMER_INPUT1_GPIO);

    lptimer_interrupt_flag_clear(LPTIMER_INT_FLAG_CMPVM | LPTIMER_INT_FLAG_CARM);
    lptimer_interrupt_enable(LPTIMER_INT_CMPVM | LPTIMER_INT_CARM);

    lptimer_countinue_start(9999U, 5000U);
    /* wait EXTI trigger start LPTIMER counter*/
    while(lptimer_counter_read() == 0) {
    }
}

/*!
    \brief      toggle the led every 200ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal) {
        if(timingdelaylocal < 100U) {
            gd_eval_led_on(LED1);
        } else {
            gd_eval_led_off(LED1);
        }

        timingdelaylocal--;
    } else {
        timingdelaylocal = 200U;
    }
}
