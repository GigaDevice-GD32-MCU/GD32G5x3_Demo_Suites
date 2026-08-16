/*!
    \file    main.c
    \brief   HRTIMER TIMER Breath LED demo

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

void gpio_config(void);
void timer_config(void);

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /*Configure PA8(TIMER0_CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_8);
    /* configure HRTIMER_ST0CH1(PA9) */
    gpio_af_set(GPIOA,GPIO_AF_13,GPIO_PIN_9);
    gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_100_220MHZ,GPIO_PIN_9);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = SystemCoreClock / 216 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 216;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

     /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,250);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
}

/**
    \brief      configure the HRTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void hrtimer_config(void)
{
/* -----------------------------------------------------------------------
    fHRTIMER_CK:216MHz, from CK_SYS
    waveform period: 27000/(216M/4)=500us
    ST0_CH1_O: set request at Slave_TIMER0 period event, reset request at Slave_TIMER0 compare 0 event
  ----------------------------------------------------------------------- */
    hrtimer_baseinit_parameter_struct baseinit_para;
    hrtimer_timerinit_parameter_struct timerinit_para;
    hrtimer_timercfg_parameter_struct timercfg_para;
    hrtimer_comparecfg_parameter_struct comparecfg_para;
    hrtimer_channel_outputcfg_parameter_struct outcfg_para;
    
    /* clock config */
    rcu_periph_clock_enable(RCU_HRTIMER);
    rcu_hrtimer_clock_config(RCU_HRTIMERSRC_CKSYS);
    
    /* periodic DLL calibration */
    hrtimer_dll_calibration_start(HRTIMER0, HRTIMER_CALIBRATION_16384_PERIOD);
    while(RESET == hrtimer_common_flag_get(HRTIMER0, HRTIMER_FLAG_DLLCAL));
    
    /* Slave_TIMER0 time base clock config  */
    hrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period = 27000;
    baseinit_para.prescaler = HRTIMER_PRESCALER_DIV4;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode = HRTIMER_COUNTER_MODE_CONTINOUS;
    hrtimer_timers_base_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &baseinit_para);
    
    /* initialize Slave_TIMER0 to work in waveform mode */
    hrtimer_timerinit_struct_para_init(&timerinit_para);
    timerinit_para.cnt_bunch = HRTIMER_TIMERBUNCHNMODE_MAINTAINCLOCK;
    timerinit_para.dac_trigger = HRTIMER_DAC_TRIGGER_NONE;
    timerinit_para.half_mode = HRTIMER_HALFMODE_DISABLED;
    timerinit_para.repetition_update = HRTIMER_UPDATEONREPETITION_DISABLED;
    timerinit_para.reset_sync = HRTIMER_SYNCRESET_DISABLED;
    timerinit_para.shadow = HRTIMER_SHADOW_DISABLED;
    timerinit_para.start_sync = HRTIMER_SYNISTART_DISABLED;
    timerinit_para.update_selection = HRTIMER_MT_ST_UPDATE_SELECTION_INDEPENDENT;
    hrtimer_timers_waveform_init(HRTIMER0, HRTIMER_SLAVE_TIMER0, &timerinit_para);
    
    /* configure the general behavior of a Slave_TIMER0 which work in waveform mode */
    hrtimer_timercfg_struct_para_init(&timercfg_para);
    timercfg_para.balanced_mode = HRTIMER_STXBALANCEDMODE_DISABLED;
    timercfg_para.cnt_reset = HRTIMER_STXCNT_RESET_NONE;
    timercfg_para.deadtime_enable = HRTIMER_STXDEADTIME_DISABLED;
    timercfg_para.delayed_idle = HRTIMER_STXDELAYED_IDLE_DISABLED;
    timercfg_para.fault_enable = HRTIMER_STXFAULTENABLE_NONE;
    timercfg_para.fault_protect = HRTIMER_STXFAULT_PROTECT_READWRITE;
    timercfg_para.reset_update = HRTIMER_STXUPDATEONRESET_DISABLED;
    timercfg_para.update_source = HRTIMER_STXUPDATETRIGGER_NONE;
    hrtimer_slavetimer_waveform_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, &timercfg_para);
    
    /* configures the compare unit of a Slave_TIMER0 which work in waveform mode */
    hrtimer_comparecfg_struct_para_init(&comparecfg_para);
    comparecfg_para.compare_value = 11250;
    hrtimer_slavetimer_waveform_compare_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_COMPARE0, &comparecfg_para);
    
    /* configures the ST0_CH0_O output of a Slave_TIMER0 work in waveform mode */
    hrtimer_channel_outputcfg_struct_para_init(&outcfg_para);
    outcfg_para.carrier_mode = HRTIMER_CHANNEL_CARRIER_DISABLED;
    outcfg_para.deadtime_bunch = HRTIMER_CHANNEL_BUNCH_ENTRY_REGULAR;
    outcfg_para.fault_state = HRTIMER_CHANNEL_FAULTSTATE_NONE;
    outcfg_para.idle_bunch = HRTIMER_CHANNEL_BUNCH_IDLE_DISABLE;
    outcfg_para.idle_state = HRTIMER_CHANNEL_IDLESTATE_INACTIVE;
    outcfg_para.polarity = HRTIMER_CHANNEL_POLARITY_HIGH;
    outcfg_para.reset_request = HRTIMER_CHANNEL_RESET_CMP0;
    outcfg_para.set_request = HRTIMER_CHANNEL_SET_PER;
    hrtimer_slavetimer_waveform_channel_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_ST0_CH1, &outcfg_para);
    
    /* enable output channel */
    hrtimer_output_channel_enable(HRTIMER0, HRTIMER_ST0_CH1);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int16_t i = 0;
    FlagStatus breathe_flag = SET;
    
    /* configure the GPIO ports */
    gpio_config();
    
    /* configure the TIMER peripheral */
    timer_config();
    
    /* configure the HRTIMER peripheral */
    hrtimer_config();
    
    /* configure systick */
    systick_config();

    /* enable TIMER and HRTIMER counter */
    hrtimer_timers_counter_enable(HRTIMER0, HRTIMER_ST0_COUNTER);
    timer_enable(TIMER0);
    
    while (1){
        /* delay a time in milliseconds */
        delay_1ms(40);
        if (SET == breathe_flag){
             i = i + 10;
        }else{
            i = i - 10;
        }
        if(490 <= i){
            breathe_flag = RESET;
        }
        if(10 >= i){
            breathe_flag = SET;
        }
        /* configure HRTIMER/TIMER channel output pulse value */
        timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0, i);
        hrtimer_slavetimer_compare_value_config(HRTIMER0, HRTIMER_SLAVE_TIMER0, HRTIMER_COMPARE0, i*45);
    }
}
