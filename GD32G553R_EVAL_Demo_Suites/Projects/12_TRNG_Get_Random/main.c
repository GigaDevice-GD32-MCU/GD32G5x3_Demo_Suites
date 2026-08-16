/*!
    \file    main.c
    \brief   TRNG get random

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
#include "gd32g553r_eval.h"
#include <stdio.h>

char usart_data_get(void);
uint8_t trng_random_range_get(uint8_t min, uint8_t max);
ErrStatus trng_ready_check(void);
ErrStatus trng_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t min, max;
    uint8_t retry = 0;

    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    printf("\r\n /==============Gigadevice TRNG test=============/ \r\n");

    /* configure TRNG module */
    while((ERROR == trng_configuration()) && retry < 3) {
        printf("TRNG init fail and retry %d\r\n", retry + 1);
        retry++;
    }

    if(retry >= 3U) {
        printf("TRNG init fail! Please reboot!\r\n");
        while(1) {
        }
    }

    printf("TRNG init successful \r\n");

    while(1) {
        /* get the random number range */
        printf("Please input min num (hex format): \r\n");
        min = usart_data_get();
        printf("Please input max num (hex format): \r\n");
        max = usart_data_get();

        /* input value is invalid */
        if(min > max) {
            printf("Please input correct num \r\n");
            continue;
        }

        /* enable TRNG module */
        trng_enable();
        
        printf("Input min num is %d \r\n", min);
        printf("Input max num is %d \r\n", max);
        printf("Generate random num1 is %d \r\n", trng_random_range_get(min, max));
        printf("Generate random num2 is %d \r\n", trng_random_range_get(min, max));
        
        /* disable TRNG module */
        trng_disable();
    }
}

/*!
    \brief      check whether the TRNG module is ready
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus trng_ready_check(void)
{
    uint32_t timeout = 0;
    FlagStatus trng_flag = RESET;

    /* check wherther the random data is valid */
    do {
        timeout++;
        trng_flag = trng_flag_get(TRNG_FLAG_DRDY);
    } while((RESET == trng_flag) && (0xFFFF > timeout));

    if(SET == trng_flag) {
        if(RESET == trng_interrupt_flag_get(TRNG_INT_FLAG_CEIF)) {
            if(RESET == trng_interrupt_flag_get(TRNG_INT_FLAG_SEIF)) {
                if(RESET == trng_flag_get(TRNG_FLAG_CECS)) {
                    if(RESET == trng_flag_get(TRNG_FLAG_SECS)) {
                        return SUCCESS;
                    }
                }
            }
        }
    }

    printf("error occurred! : %x \r\n", TRNG_STAT);
    /* return check status */
    return ERROR;

}

/*!
    \brief      configure TRNG module
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus trng_configuration(void)
{

    ErrStatus reval = SUCCESS;

    while(SET != rcu_flag_get(RCU_FLAG_PLLSTB));

    /* configure the TRNG prescaler selection */
    rcu_trng_clock_config(RCU_TRNG_CKPLLQ_DIV2);

    /* TRNG module clock enable */
    rcu_periph_clock_enable(RCU_TRNG);

    /* TRNG registers reset */
    trng_deinit();
    /* configure as LFSR mode */
    trng_conditioning_reset_enable();
    trng_mode_config(TRNG_MODSEL_LFSR);
    trng_conditioning_disable();

    return reval;

}

/*!
    \brief      wait till a character is received by the USART
    \param[in]  none
    \param[out] none
    \retval     keyboard pressed character
*/
char usart_data_get(void)
{
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE)) {
    }

    return (char)(usart_data_receive(EVAL_COM));
}

/*!
    \brief      get random data in range
    \param[in]  none
    \param[out] none
    \retval     random data
*/
uint8_t trng_random_range_get(uint8_t min, uint8_t max)
{
    if(SUCCESS == trng_ready_check()) {
        return (trng_get_true_random_data() % (max - min + 1) + min);
    } else {
        return 0;
    }
}
