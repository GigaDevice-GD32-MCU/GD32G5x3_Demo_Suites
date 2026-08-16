/*!
    \file    gd32g553q_eval.h
    \brief   definitions for leds, keys and COM ports hardware resources

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

#ifndef GD32G553Q_EVAL_H
#define GD32G553Q_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32g5x3.h"

/* exported types */
typedef enum 
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3,
    LED_MAX
}led_typedef_enum;

typedef enum 
{
    KEY_A = 0,
    KEY_B = 1,
    KEY_C = 2,
    KEY_D = 3,
    KEY_CET = 4,
    KEY_MAX
}key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;

/* eval board low layer led */
#define LED1_PIN                         GPIO_PIN_3
#define LED1_GPIO_PORT                   GPIOE
#define LED1_GPIO_CLK                    RCU_GPIOE

#define LED2_PIN                         GPIO_PIN_4
#define LED2_GPIO_PORT                   GPIOE
#define LED2_GPIO_CLK                    RCU_GPIOE

#define LED3_PIN                         GPIO_PIN_5
#define LED3_GPIO_PORT                   GPIOE
#define LED3_GPIO_CLK                    RCU_GPIOE

#define LED4_PIN                         GPIO_PIN_6
#define LED4_GPIO_PORT                   GPIOE
#define LED4_GPIO_CLK                    RCU_GPIOE

/* eval board low layer button */  
/* wakeup push-button */
#define KEY_A_PIN                        GPIO_PIN_0
#define KEY_A_GPIO_PORT                  GPIOA
#define KEY_A_GPIO_CLK                   RCU_GPIOA
#define KEY_A_EXTI_LINE                  EXTI_0
#define KEY_A_EXTI_PORT_SOURCE           EXTI_SOURCE_GPIOA
#define KEY_A_EXTI_PIN_SOURCE            EXTI_SOURCE_PIN0
#define KEY_A_EXTI_IRQn                  EXTI0_IRQn

/* tamper push-button */
#define KEY_B_PIN                        GPIO_PIN_13
#define KEY_B_GPIO_PORT                  GPIOC
#define KEY_B_GPIO_CLK                   RCU_GPIOC
#define KEY_B_EXTI_LINE                  EXTI_13
#define KEY_B_EXTI_PORT_SOURCE           EXTI_SOURCE_GPIOC
#define KEY_B_EXTI_PIN_SOURCE            EXTI_SOURCE_PIN13
#define KEY_B_EXTI_IRQn                  EXTI10_15_IRQn

/* user push-button */
#define KEY_C_PIN                        GPIO_PIN_10
#define KEY_C_GPIO_PORT                  GPIOF
#define KEY_C_GPIO_CLK                   RCU_GPIOF
#define KEY_C_EXTI_LINE                  EXTI_10
#define KEY_C_EXTI_PORT_SOURCE           EXTI_SOURCE_GPIOF
#define KEY_C_EXTI_PIN_SOURCE            EXTI_SOURCE_PIN10
#define KEY_C_EXTI_IRQn                  EXTI10_15_IRQn

/* extension push-button */
#define KEY_D_PIN                        GPIO_PIN_9
#define KEY_D_GPIO_PORT                  GPIOF
#define KEY_D_GPIO_CLK                   RCU_GPIOF
#define KEY_D_EXTI_LINE                  EXTI_9
#define KEY_D_EXTI_PORT_SOURCE           EXTI_SOURCE_GPIOF
#define KEY_D_EXTI_PIN_SOURCE            EXTI_SOURCE_PIN9
#define KEY_D_EXTI_IRQn                  EXTI5_9_IRQn

/* extension push-button */
#define KEY_CET_PIN                      GPIO_PIN_7
#define KEY_CET_GPIO_PORT                GPIOF
#define KEY_CET_GPIO_CLK                 RCU_GPIOF
#define KEY_CET_EXTI_LINE                EXTI_7
#define KEY_CET_EXTI_PORT_SOURCE         EXTI_SOURCE_GPIOF
#define KEY_CET_EXTI_PIN_SOURCE          EXTI_SOURCE_PIN7
#define KEY_CET_EXTI_IRQn                EXTI5_9_IRQn

/* eval board low layer COM */
#define COMn                             1U

/* definition for COM, connected to USART0 */
#define EVAL_COM                         USART0
#define EVAL_COM_CLK                     RCU_USART0

#define EVAL_COM_TX_PIN                  GPIO_PIN_9
#define EVAL_COM_RX_PIN                  GPIO_PIN_10

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_7

/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode);
/* return the selected key state */
FlagStatus gd_eval_key_state_get(key_typedef_enum keynum);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef __cplusplus
}
#endif

#endif /* GD32G553Q_EVAL_H */

