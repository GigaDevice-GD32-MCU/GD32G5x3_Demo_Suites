/*!
    \file    main.c
    \brief   CAN network demo

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
#include "gd32g553r_eval.h"
#include "systick.h"
#include "string.h"

#define  BUFFER_SIZE    64U

/* global variable declarations */
can_mailbox_descriptor_struct transmit_message;
can_mailbox_descriptor_struct receive_message;
FlagStatus can0_receive_flag;

/* function declarations */
void can_gpio_config(void);
void bsp_board_config(void);
void can_config(void);
void communication_check(void);
void cache_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t i = 0U;

    /* configure systick */
    systick_config();
    /* configure board */
    bsp_board_config();
    /* configure GPIO */
    can_gpio_config();
    /* initialize CAN and filter */
    can_config();

    printf("\r\n communication test CAN0, please press WAKEUP key to start! \r\n");

    can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
    can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);

    /* fill the data for CAN transmit */
    for(i = 0; i < BUFFER_SIZE; i++){
        transmit_message.data[i] = i;
    }
    /* initialize transmit message */
    transmit_message.rtr = 0U;
    transmit_message.ide = 0U;
    transmit_message.code = CAN_MB_TX_STATUS_DATA;
    transmit_message.brs = 1U;
    transmit_message.fdf = 1U;
    transmit_message.esi = 0U;
    transmit_message.prio = 0U;
    transmit_message.data_bytes = BUFFER_SIZE;
    transmit_message.id = 0x55U;

    receive_message.rtr = 0U;
    receive_message.ide = 0U;
    receive_message.code = CAN_MB_RX_STATUS_EMPTY;
    /* rx mailbox */
    receive_message.id = 0x55U;
    can_mailbox_config(CAN0, 0U, &receive_message);

    while(1) {
        /* test whether the WAKEUP key is pressed */
        if(1U == gd_eval_key_state_get(KEY_WAKEUP)) {
            delay_1ms(100U);
            if(1 == gd_eval_key_state_get(KEY_WAKEUP)) {
                /* transmit message */
                can_mailbox_config(CAN0, 1U, &transmit_message);

                printf("\r\nCAN0 transmit data: \r\n");
                for(i = 0U; i < transmit_message.data_bytes; i++) {
                    printf("%02x ", transmit_message.data[i]);
                }

                /* waiting for the WAKEUP key up */
                while(1U == gd_eval_key_state_get(KEY_WAKEUP));
            }
        }
        communication_check();
    }
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN port clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure CAN0_TX GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_12);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_12);

    /* configure CAN0_RX GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_11);
}

/*!
    \brief      configure BSP
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_board_config(void)
{
    /* configure USART */
    gd_eval_com_init(EVAL_COM);

    /* configure WAKEUP key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);

    /* configure LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_off(LED1);
}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config(void)
{
    can_parameter_struct can_parameter;
    can_fd_parameter_struct fd_parameter;

    /* select CK_APB2 as CAN's clock source */
    rcu_can_clock_config(IDX_CAN0, RCU_CANSRC_APB2);
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    /* initialize CAN register */
    can_deinit(CAN0);
    /* initialize CAN */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FD_INIT_STRUCT, &fd_parameter);

    /* initialize CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0U;
    /* baud rate 1Mbps */
    can_parameter.resync_jump_width = 1U;
    can_parameter.prop_time_segment = 2U;
    can_parameter.time_segment_1 = 4U;
    can_parameter.time_segment_2 = 1U;
    can_parameter.prescaler = 27U;

    /* initialize CAN */
    can_init(CAN0, &can_parameter);

    /* FD parameter configurations */
    fd_parameter.bitrate_switch_enable = ENABLE;
    fd_parameter.iso_can_fd_enable = ENABLE;
    fd_parameter.mailbox_data_size = CAN_MAILBOX_DATA_SIZE_64_BYTES;
    fd_parameter.tdc_enable = DISABLE;
    fd_parameter.tdc_offset = 0U;
    /* FD baud rate 1Mbps */
    fd_parameter.resync_jump_width = 1U;
    fd_parameter.prop_time_segment = 2U;
    fd_parameter.time_segment_1 = 4U;
    fd_parameter.time_segment_2 = 1U;
    fd_parameter.prescaler = 27U;

    can_fd_config(CAN0, &fd_parameter);

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_Message_IRQn, 0U, 0U);

    /* enable CAN MB0 interrupt */
    can_interrupt_enable(CAN0, CAN_INT_MB0);

    can_operation_mode_enter(CAN0, CAN_NORMAL_MODE);
}

/*!
    \brief      check received data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void communication_check(void)
{
    uint8_t i = 0U;

    /* CAN1 receive data correctly, the received data is printed */
    if(SET == can0_receive_flag) {
        can0_receive_flag = RESET;

        /* check the receive message */
        can_mailbox_receive_data_read(CAN0, 0U, &receive_message);
        if(receive_message.data_bytes > 0U) {
            printf("\r\nCAN0 receive data: \r\n");
            for(i = 0U; i < receive_message.data_bytes; i++) {
                printf("%02x ", receive_message.data[i]);
            }
            gd_eval_led_toggle(LED1);
        }
    }
}
