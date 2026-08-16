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
#include "gd32g553r_eval.h"

#define WRITE_ENABLE_CMD                    (0x06)
#define PAGE_PROG_CMD                       (0x02)
#define QUAD_PAGE_PROG_CMD                  (0x32)
#define READ_CMD                            (0x03)
#define QUAD_FAST_READ_CMD                  (0x6B)
#define SECTOR_ERASE_CMD                    (0x20)
#define WRITE_STATUS_CMD                    (0x01)
#define READ_STATUS_REG1_CMD                (0x05)
#define READ_STATUS_REG2_CMD                (0x35)

#define BUF_SIZE                            (countof(tx_buffer))
#define countof(a)                          (sizeof(a) / sizeof(*(a)))

uint8_t tx_buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
uint8_t rx_buffer[BUF_SIZE];
qspi_command_struct qspi_cmd;
qspi_polling_struct polling_cmd;

void rcu_config(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);
void qspi_flash_init(void);
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummy_cycles, uint32_t instruction_mode,
                       uint32_t address_mode, uint32_t address_size, uint32_t data_mode);
void qspi_write_enable(void);
void qspi_polling_read_status_match_wel_status_wel(void);
void qspi_polling_read_status_match_wip(void);
void qspi_memory_map_read(void);
void qspi_flash_sector_erase(void);
void qspi_flash_program(void);
void qspi_quad_mode_enable(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i = 0;

    gd_eval_com_init(EVAL_COM);
    /* configure peripheral clock */
    rcu_config();
    printf("QSPI flash writing...\r\n");
    /* configure QSPI */
    qspi_flash_init();
    /* qspi flash sector erase */
    qspi_flash_sector_erase();
    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    /* qspi flash program */
    qspi_flash_program();
    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    printf("QSPI flash reading...\r\n");
    /* read data by memory map mode */
    qspi_memory_map_read();

    for(i = 0; i < BUF_SIZE; i++) {
        rx_buffer[i] = *(uint8_t *)(0x90000000 + i);
    }

    if(memory_compare(rx_buffer, tx_buffer, BUF_SIZE)) {
        printf("QSPI FLASH WRITE AND READ TEST SUCCESS!\r\n");
    } else {
        printf("QSPI FLASH WRITE AND READ TEST ERROR!\r\n");
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
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
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
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*!
    \brief      configure the QSPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_init(void)
{
    qspi_init_struct qspi_init_para;

    /* initialize the QSPI command parameter structure */
    qspi_cmd_struct_para_init(&qspi_cmd);
    /* initialize the QSPI read polling parameter structure */
    qspi_polling_struct_para_init(&polling_cmd);

    /* QSPI GPIO config:SCK/PB10, NSS/PB11, IO0/PB1, IO1/PB2, IO2/PA6, IO3/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_1 | GPIO_PIN_2);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_1 | GPIO_PIN_2);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_1 | GPIO_PIN_2);

    /* initialize the init parameter structure */
    qspi_struct_para_init(&qspi_init_para);
    qspi_init_para.clock_mode     = QSPI_CLOCK_MODE_HIGH;
    qspi_init_para.fifo_threshold = 4;
    qspi_init_para.sample_shift1  = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    qspi_init_para.sample_shift2  = QSPI_SHIFTING_NONE;
    qspi_init_para.cs_high_time   = QSPI_CS_HIGH_TIME_2_CYCLE;
    qspi_init_para.flash_size     = 27;
    qspi_init_para.prescaler      = 3;
    qspi_init(&qspi_init_para);
    
    /* enable QSPI */
    qspi_enable();
}

/*!
    \brief      send QSPI command
    \param[in]  instruction: QSPI instruction, reference flash commands description
    \param[in]  address: access address, 0-flash size
    \param[in]  dummy_cycles: dummy cycles, 0 - 31
    \param[in]  instruction_mode: instruction mode
      \arg        QSPI_INSTRUCTION_NONE
      \arg        QSPI_INSTRUCTION_1_LINE
      \arg        QSPI_INSTRUCTION_2_LINES
      \arg        QSPI_INSTRUCTION_4_LINES
    \param[in]  address_mode: address mode
      \arg        QSPI_ADDR_NONE
      \arg        QSPI_ADDR_1_LINE
      \arg        QSPI_ADDR_2_LINES
      \arg        QSPI_ADDR_4_LINES
    \param[in]  address_size: address size
      \arg        QSPI_ADDR_8_BITS
      \arg        QSPI_ADDR_16_BITS
      \arg        QSPI_ADDR_24_BITS
      \arg        QSPI_ADDR_32_BITS
    \param[in]  data_mode: data mode
      \arg        QSPI_DATA_NONE
      \arg        QSPI_DATA_1_LINE
      \arg        QSPI_DATA_2_LINES
      \arg        QSPI_DATA_4_LINES
    \param[out] none
    \retval     none
*/
void qspi_send_command(uint32_t instruction, uint32_t address, uint32_t dummy_cycles, uint32_t instruction_mode,
                       uint32_t address_mode, uint32_t address_size, uint32_t data_mode)
{
    qspi_cmd.instruction      = instruction;
    qspi_cmd.instruction_mode = instruction_mode;
    qspi_cmd.addr             = address;
    qspi_cmd.addr_mode        = address_mode;
    qspi_cmd.addr_size        = address_size;
    qspi_cmd.altebytes        = 0;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = data_mode;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = dummy_cycles;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_write_enable(void)
{
    /* write enable */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = WRITE_ENABLE_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      poll the status of the write enable latch(wel) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_polling_read_status_match_wel(void)
{
    polling_cmd.match            = 0x02;
    polling_cmd.mask             = 0x02;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;
    qspi_cmd.instruction         = READ_STATUS_REG1_CMD;
    qspi_cmd.data_mode           = QSPI_DATA_1_LINE;

    qspi_polling_config(&qspi_cmd, &polling_cmd);

    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_polling_read_status_match_wip(void)
{
    qspi_cmd.instruction         = READ_STATUS_REG1_CMD;
    qspi_cmd.addr_mode           = QSPI_ADDR_NONE;
    qspi_cmd.data_mode           = QSPI_DATA_1_LINE;
    qspi_cmd.instruction_mode    = QSPI_INSTRUCTION_1_LINE;
    polling_cmd.match            = 0x00;
    polling_cmd.mask             = 0x01;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;

    qspi_polling_config(&qspi_cmd, &polling_cmd);

    /* wait for the BUSY flag to be reset */
    while(RESET != qspi_flag_get(QSPI_FLAG_BUSY)) {
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);

}

/*!
    \brief      read spi flash by memory map mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_memory_map_read(void)
{
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = QUAD_FAST_READ_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_1_LINE;
    qspi_cmd.addr_size        = QSPI_ADDR_24_BITS;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.dummycycles      = 8;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    
    qspi_memorymapped_config(&qspi_cmd, 0, QSPI_TMOUT_ENABLE);
}

/*!
    \brief      qspi flash sector erase
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_sector_erase(void)
{
    /* QSPI write enable */
    qspi_write_enable();
    /* configure read polling mode to wait for write enabling */
    qspi_polling_read_status_match_wel();
    /* erase sector 0-0x1000 */
    qspi_send_command(SECTOR_ERASE_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDR_1_LINE, QSPI_ADDR_24_BITS, QSPI_DATA_NONE);
    qspi_polling_read_status_match_wip();
}

/*!
    \brief      qspi flash program
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_program(void)
{
    qspi_quad_mode_enable();
    /* QSPI write enable */
    qspi_write_enable();
    /* configure read polling mode to wait for write enabling */
    qspi_polling_read_status_match_wel();
    /* write data */
    qspi_send_command(QUAD_PAGE_PROG_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDR_1_LINE, QSPI_ADDR_24_BITS, QSPI_DATA_4_LINES);
    qspi_data_length_config(BUF_SIZE);
    qspi_data_transmit(tx_buffer);
    /* wait for the data transmit completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    qspi_polling_read_status_match_wip();
}

/*!
    \brief      enable quad flash mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_quad_mode_enable(void)
{
    uint8_t status[2] = {0x00, 0x02};

    /* QSPI write enable */
    qspi_write_enable();
    /* configure read polling mode to wait for write enabling */
    qspi_polling_read_status_match_wel();
    /* write status register */
    qspi_send_command(WRITE_STATUS_CMD, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDR_NONE, QSPI_ADDR_24_BITS, QSPI_DATA_1_LINE);
    qspi_data_length_config(2);
    qspi_data_transmit(status);
    /* wait for the data transmit completed */
    while(RESET == qspi_flag_get(QSPI_FLAG_TC)) {
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
    qspi_polling_read_status_match_wip();
}
