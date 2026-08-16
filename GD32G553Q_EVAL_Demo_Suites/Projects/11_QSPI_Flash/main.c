/*!
    \file    main.c
    \brief   QSPI write and read demo

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

#include <stdio.h>
#include "gd32g5x3.h"
#include "gd32g553q_eval.h"
#include "systick.h"
#include "gd25t512xx.h"

#define BUFFER_SIZE              256
#define FLASH_ID                 0xC8461AFF
#define FLASH_WRITE_ADDRESS      0x00001000
#define FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS

uint32_t flash_id = 0;
uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

ErrStatus memory_compare(uint8_t *src, uint8_t *dst, int length);
void reset_buffer(uint8_t *pbuf, int length);
void fill_buffer(uint8_t *pbuf, int length);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration */
    systick_config();
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    /* configure QSPI */
    qspi_flash_init();
    /* qspi flash reset enable */
    qspi_flash_reset_enable(SPI_MODE);
    /* qspi flash reset */
    qspi_flash_reset_memory(SPI_MODE);
    delay_1ms(1);
    /* get flash id */
    flash_id = qspi_flash_read_id(SPI_MODE);
    printf("\n\rThe Flash_ID:0x%X\n\r", flash_id);

    /* flash id is correct */
    if(FLASH_ID == flash_id) {
        /* 1-line test */
        fill_buffer(tx_buffer, BUFFER_SIZE);
        /* erase the specified flash sector */
        qspi_flash_block_erase(SPI_MODE, GD25T512ME_3BYTES_SIZE, FLASH_WRITE_ADDRESS, GD25T512ME_ERASE_4K);
        /* write tx_buffer data to the flash */
        qspi_flash_buffer_write(SPI_MODE, GD25T512ME_3BYTES_SIZE, tx_buffer, FLASH_WRITE_ADDRESS, BUFFER_SIZE);
        /* read a block of data from the flash to rx_buffer */
        qspi_flash_buffer_read(SPI_MODE, GD25T512ME_3BYTES_SIZE, rx_buffer, FLASH_READ_ADDRESS, BUFFER_SIZE);
        /* test failed */
        if(ERROR == memory_compare(tx_buffer, rx_buffer, BUFFER_SIZE)) {
            printf("\n\rQSPI Flash GD25T512 1-Line Test Failed!\n\r");
        } else {
            /* test passed */
            printf("\n\rQSPI Flash GD25T512 1-Line Test Passed!\n\r");
        }

        /* 4-lines test */
        reset_buffer(rx_buffer, BUFFER_SIZE);
        qspi_flash_quad_enbale();
        /* erase the specified flash sector */
        qspi_flash_block_erase(QSPI_MODE, GD25T512ME_3BYTES_SIZE, FLASH_WRITE_ADDRESS, GD25T512ME_ERASE_4K);
        /* write tx_buffer data to the flash */
        qspi_flash_buffer_write(QSPI_MODE, GD25T512ME_3BYTES_SIZE, tx_buffer, FLASH_WRITE_ADDRESS, BUFFER_SIZE);
        /* read a block of data from the flash to rx_buffer */
        qspi_flash_buffer_read(QSPI_MODE, GD25T512ME_3BYTES_SIZE, rx_buffer, FLASH_READ_ADDRESS, BUFFER_SIZE);
        /* test failed */
        if(ERROR == memory_compare(tx_buffer, rx_buffer, BUFFER_SIZE)) {
            printf("\n\rQSPI Flash GD25T512 4-Lines Test Failed!\n\r");
        } else {
            /* spi qspi flash test passed */
            printf("\n\rQSPI Flash GD25T512 4-Lines Test Passed!\n\r");
        }

        /* 4-lines DDR mode read test */
        reset_buffer(rx_buffer, BUFFER_SIZE);
        qspi_flash_quad_enbale();
        /* erase the specified flash sector */
        qspi_flash_block_erase(QSPI_MODE, GD25T512ME_3BYTES_SIZE, FLASH_WRITE_ADDRESS, GD25T512ME_ERASE_4K);
        /* write tx_buffer data to the flash */
        qspi_flash_buffer_write(QSPI_MODE, GD25T512ME_3BYTES_SIZE, tx_buffer, FLASH_WRITE_ADDRESS, BUFFER_SIZE);
        /* read a block of data from the flash to rx_buffer */
        qspi_flash_ddr_enbale(GD25T512ME_3BYTES_SIZE);
        qspi_flash_buffer_read_ddr(GD25T512ME_3BYTES_SIZE, rx_buffer, FLASH_READ_ADDRESS, BUFFER_SIZE);
        /* test failed */
        if(ERROR == memory_compare(tx_buffer, rx_buffer, BUFFER_SIZE)) {
            printf("\n\rQSPI Flash GD25T512 4-Lines DDR Mode Test Failed!\n\r");
        } else {
            /* spi qspi flash test passed */
            printf("\n\rQSPI Flash GD25T512 4-Lines DDR Mode Test Passed!\n\r");
        }
    } else {
        /* spi flash read id fail */
        printf("\n\rQSPI Flash GD25T512: Read ID Fail!\n\r");
    }

    while(1) {
    }
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_QSPI);
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, int length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*!
    \brief      reset buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void reset_buffer(uint8_t *pbuf, int length)
{
    for(uint16_t i = 0; i < length; i++) {
        *pbuf++ = 0;
    }
}

/*!
    \brief      fill buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fill_buffer(uint8_t *pbuf, int length)
{
    for(uint16_t i = 0; i < length; i++) {
        *pbuf++ = i;
    }
}
