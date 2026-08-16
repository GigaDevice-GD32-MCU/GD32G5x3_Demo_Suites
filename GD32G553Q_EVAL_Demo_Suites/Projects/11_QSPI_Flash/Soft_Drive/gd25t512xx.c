/*!
    \file    gd25t512xx.c
    \brief   the header file of SPI flash gd25t512xx driver

    \version 2026-02-10, V1.4.0, demo for GD32G5x3
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc

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

#include "gd25t512xx.h"
#include "gd32g5x3.h"
#include "systick.h"

/* QSPI timeout */
#define QSPI_MAX_TOMEOUT    ((uint32_t)0xFFFFFFFFU)                  /*!< QSPI timeout */


qspi_command_struct qspi_cmd;
qspi_polling_struct polling_cmd;

/*!
    \brief      initialize QSPI and GPIO
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

    /* configure different peripheral clocks */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_QSPI);

    /* configure QSPI GPIO: DQS/PB7, SCK/PD2, NSS/PD3, IO0/PF8, IO1/PD5, IO2/PD6, IO3/PD7 
    It is strongly recommended to use pins in groups to ensure the maximum communication clock frequency supported by QSPI.
    For details, please refer to the user manual.
    In this example, GPIO configuration is adapted for the test development board.*/
    gpio_af_set(GPIOD, GPIO_AF_10, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ,
                            GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_7);

    gpio_af_set(GPIOF, GPIO_AF_10, GPIO_PIN_8);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_8);

    /* configure reset GPIO and pull high RESET pin */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6);
    gpio_bit_reset(GPIOB, GPIO_PIN_6);
    delay_1ms(1);
    gpio_bit_set(GPIOB, GPIO_PIN_6);

    /* initialize the init parameter structure */
    qspi_struct_para_init(&qspi_init_para);
    qspi_init_para.clock_mode     = QSPI_CLOCK_MODE_LOW;
    qspi_init_para.fifo_threshold = 4;
    qspi_init_para.sample_shift1  = QSPI_SAMPLE_SHIFTING_NONE;
    qspi_init_para.cs_high_time   = QSPI_CS_HIGH_TIME_2_CYCLE;
    qspi_init_para.flash_size     = 27;
    qspi_init_para.prescaler      = 8;
    qspi_init(&qspi_init_para);
    /* enable QSPI */
    qspi_enable();
}

/*!
    \brief      read the flah id
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     none
*/
uint32_t qspi_flash_read_id(interface_mode i_mode)
{
    uint8_t temp_id[4];

    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.data_mode        = QSPI_DATA_1_LINE;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 8;
        qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    }
    /* initialize read ID command */
    qspi_cmd.instruction      = GD25T512ME_READ_ID_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_length      = 4;

    /* send the command */
    qspi_command_config(&qspi_cmd);

    /* receive data */
    qspi_data_receive(temp_id);

    return (temp_id[0] << 24) | (temp_id[1] << 16) | (temp_id[2] << 8) | temp_id[3];
}

/*!
    \brief      enable flash reset
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     none
*/
void qspi_flash_reset_enable(interface_mode i_mode)
{
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
    }

    /* initialize enable flash reset command */
    qspi_cmd.instruction      = GD25T512ME_RESET_ENABLE_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;

    /* send the command */
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      reset the flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     none
*/
void qspi_flash_reset_memory(interface_mode i_mode)
{
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
    }

    /* initialize enable flash reset command */
    qspi_cmd.instruction      = GD25T512ME_RESET_MEMORY_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;

    /* send the command */
    qspi_command_config(&qspi_cmd);
}

/*!
    \brief      enable flash write
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     none
*/
void qspi_flash_write_enbale(interface_mode i_mode)
{
    uint32_t timeout = QSPI_MAX_TOMEOUT;
    
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
    }

    /* initialize write enable command */
    qspi_cmd.instruction      = GD25T512ME_WRITE_ENABLE_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;

    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* wait for the BUSY flag to be reset */
    while((SET == qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout > 0U)){
        timeout--;
    }

    /* configure read polling mode to wait for write enabling */
    qspi_cmd.instruction         = GD25T512ME_READ_STATUS_REG1_CMD;
    polling_cmd.match            = 0x02;
    polling_cmd.mask             = GD25T512ME_SR_WEL;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;

    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.data_mode       = QSPI_DATA_1_LINE;
        qspi_cmd.dummycycles     = 0;
    } else {
        qspi_cmd.data_mode       = QSPI_DATA_4_LINES;
        qspi_cmd.dummycycles     = 8;
    }

    qspi_polling_config(&qspi_cmd, &polling_cmd);
    /* wait for the BUSY flag to be reset */
    while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout > 0U)){
        timeout--;
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     none
*/
void qspi_flash_autopolling_mem_ready(interface_mode i_mode)
{
    uint32_t timeout = QSPI_MAX_TOMEOUT;
    
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.data_mode        = QSPI_DATA_1_LINE;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    }

    /* initialize read status register command */
    qspi_cmd.instruction      = GD25T512ME_READ_STATUS_REG1_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;

    /* configure read polling mode to wait for write in progress flag */
    polling_cmd.match            = 0;
    polling_cmd.mask             = GD25T512ME_SR_WIP;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;

    qspi_polling_config(&qspi_cmd, &polling_cmd);
    /* wait for the BUSY flag to be reset */
    while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout > 0U)){
        timeout--;
    }
    /* clear the RPMF flag */
    qspi_flag_clear(QSPI_FLAG_RPMF);
}

/*!
    \brief      enable flash QPI mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void qspi_flash_quad_enbale(void)
{
    uint32_t timeout = QSPI_MAX_TOMEOUT;
    
    /* enter QPI mode */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
    qspi_cmd.instruction      = GD25T512ME_ENABLE_QPI_CMD;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* wait for the BUSY flag to be reset */
    while((SET == qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout > 0U)){
        timeout--;
    }
}

/*!
    \brief      enable flash DDR mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[out] none
    \retval     none
*/
void qspi_flash_ddr_enbale(addr_size addr_size)
{
    uint8_t volatile_reg_data[] = {0xE7};
    uint32_t timeout = QSPI_MAX_TOMEOUT;
    
    /* initialize program command */
    if(GD25T512ME_3BYTES_SIZE == addr_size) {
        qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
    } else {
        qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
    }
    /* configure flash in Quad DTR with DQS */
    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
    qspi_cmd.instruction      = GD25T512ME_WRITE_VOLATILE_CFG_REG_CMD;
    qspi_cmd.addr             = 0;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.altebytes        = 0;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.data_length      = 1;
    qspi_cmd.dummycycles      = 0;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    /* send the command */
    qspi_command_config(&qspi_cmd);

    qspi_data_transmit(volatile_reg_data);
    /* wait for the BUSY flag to be reset */
    while((SET == qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout > 0U)){
        timeout--;
    }
}

/*!
    \brief      erase the specified block of the flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[in]  addr: address of erase
    \param[in]  block_size: block size to erase
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_ERASE_4K: 4K size sector erase
      \arg        GD25T512ME_ERASE_64K: 64K size block erase
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_block_erase(interface_mode i_mode, addr_size addr_size, uint32_t addr,
                            erase_size block_size)
{
    uint32_t timeout_max, timeout = 0;

    /* enable the write access to the flash */
    qspi_flash_write_enbale(i_mode);

    /* initialize block erase command */
    if(GD25T512ME_ERASE_64K == block_size) {
        if(GD25T512ME_3BYTES_SIZE == addr_size) {
            qspi_cmd.instruction = GD25T512ME_BLOCK_ERASE_64K_CMD;
            qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
        } else {
            qspi_cmd.instruction = GD25T512ME_4_BYTE_BLOCK_ERASE_64K_CMD;
            qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
        }
        timeout_max = GD25T512ME_BLOCK_ERASE_MAX_TIME;
    } else {
        if(GD25T512ME_3BYTES_SIZE == addr_size) {
            qspi_cmd.instruction = GD25T512ME_SECTOR_ERASE_4K_CMD;
            qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
        } else {
            qspi_cmd.instruction = GD25T512ME_4_BYTE_SECTOR_ERASE_4K_CMD;
            qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
        }
        timeout_max = GD25T512ME_SECTOR_ERASE_MAX_TIME;
    }
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.addr_mode        = QSPI_ADDR_1_LINE;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    }

    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.addr             = addr;

    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* wait for the BUSY flag to be reset */
    do{
        timeout++;
    } while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout <= timeout_max));
    
    if(timeout > timeout_max) {
        return GD25T512ME_ERROR;
    }

    /* configure read polling mode to wait busy */
    qspi_flash_autopolling_mem_ready(i_mode);

    return GD25T512ME_OK;
}

/*!
    \brief      erase the the entire flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_chip_erase(interface_mode i_mode)
{
    uint32_t timeout_max, timeout = 0;

    timeout_max = GD25T512ME_BULK_ERASE_MAX_TIME;
    /* enable the write access to the flash */
    qspi_flash_write_enbale(i_mode);

    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
    }
    /* initialize chip erase command */
    qspi_cmd.instruction      = GD25T512ME_CHIP_ERASE_CMD;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.data_mode        = QSPI_DATA_NONE;
    qspi_cmd.addr_mode        = QSPI_ADDR_NONE;

    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* wait for the BUSY flag to be reset */
    do{
        timeout++;
    } while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout <= timeout_max));
    
    if(timeout > timeout_max) {
        return GD25T512ME_ERROR;
    }
    
    /* configure read polling mode to wait busy */
    qspi_cmd.instruction         = GD25T512ME_READ_STATUS_REG1_CMD;
    polling_cmd.match            = 0x00;
    polling_cmd.mask             = 0x01;
    polling_cmd.match_mode       = QSPI_MATCH_MODE_AND;
    polling_cmd.statusbytes_size = 1;
    polling_cmd.interval         = 0x10;
    polling_cmd.polling_stop     = QSPI_POLLING_STOP_ENABLE;

    if(SPI_MODE == i_mode) {
        qspi_cmd.data_mode   = QSPI_DATA_1_LINE;
        qspi_cmd.dummycycles      = 0;
    } else {
        qspi_cmd.data_mode   = QSPI_DATA_4_LINES;
        qspi_cmd.dummycycles      = 0;
    }

    /* configure read polling mode to wait busy */
    qspi_polling_config(&qspi_cmd, &polling_cmd);

    return GD25T512ME_OK;
}

/*!
    \brief      write a page of data to the flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[in]  pbuffer : pointer to the buffer
    \param[in]  write_addr: flash's internal address to write to
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_page_write(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                           uint32_t write_addr, uint32_t num_byte_to_write)
{
    uint32_t timeout_max, timeout = 0;
    uint32_t timeout1 = QSPI_MAX_TOMEOUT;
    
    timeout_max = GD25T512ME_PAGE_PROGRAM_MAX_TIME;
    
    /* enable the write access to the flash */
    qspi_flash_write_enbale(i_mode);

    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.addr_mode        = QSPI_ADDR_1_LINE;
        qspi_cmd.data_mode        = QSPI_DATA_1_LINE;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 0;
        qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
        qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    }

    /* initialize program command */
    if(GD25T512ME_3BYTES_SIZE == addr_size) {
        qspi_cmd.instruction = GD25T512ME_PAGE_PROG_CMD;
        qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
    } else {
        qspi_cmd.instruction = GD25T512ME_4_BYTE_PAGE_PROG_CMD;
        qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
    }

    qspi_cmd.addr            = write_addr;
    qspi_cmd.data_length     = num_byte_to_write;
    qspi_cmd.altebytes_mode  = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode       = QSPI_SIOO_INST_EVERY_CMD;

    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* transmission of the data */
    qspi_data_transmit(pbuffer);

    /* wait for the data transmit completed */
    while((0U == qspi_flag_get(QSPI_FLAG_TC)) && (timeout1 > 0U)){
        timeout1--;
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    /* wait for the BUSY flag to be reset */
    do{
        timeout++;
    } while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout <= timeout_max));
    
    if(timeout > timeout_max) {
        return GD25T512ME_ERROR;
    }
    
    /* configure read polling mode to wait busy */
    qspi_flash_autopolling_mem_ready(i_mode);
    
    return GD25T512ME_OK;
}

/*!
    \brief      write a block of data to the flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[in]  pbuffer : pointer to the buffer
    \param[in]  write_addr: flash's internal address to write to
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_buffer_write(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                             uint32_t write_addr, uint32_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;
    error_code status;

    addr = write_addr % GD25T512ME_PAGE_SIZE;
    count = GD25T512ME_PAGE_SIZE - addr;
    num_of_page =  num_byte_to_write / GD25T512ME_PAGE_SIZE;
    num_of_single = num_byte_to_write % GD25T512ME_PAGE_SIZE;
    /* write_addr is GD25T512ME_PAGE_SIZE aligned */
    if(addr == 0) {
        /* data_size < GD25T512ME_PAGE_SIZE */
        if(num_of_page == 0) {
            status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, num_byte_to_write);
            if(GD25T512ME_ERROR == status){
                return GD25T512ME_ERROR;
            }
        } else {
            /* data_size >= GD25T512ME_PAGE_SIZE */
            while(num_of_page--) {
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, GD25T512ME_PAGE_SIZE);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
                write_addr +=  GD25T512ME_PAGE_SIZE;
                pbuffer += GD25T512ME_PAGE_SIZE;
            }
            if(num_of_single != 0) {
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, num_of_single);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
            }
        }
    } else {
        /* write_addr is not GD25T512ME_PAGE_SIZE aligned */
        if(num_of_page == 0) {
            /* (data_size + write_addr) > GD25T512ME_PAGE_SIZE */
            if(num_of_single > count) {
                temp = num_of_single - count;
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, count);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
                write_addr +=  count;
                pbuffer += count;
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, temp);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
            } else {
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, num_byte_to_write);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
            }
        } else {
            /* data_size >= GD25T512ME_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page =  num_byte_to_write / GD25T512ME_PAGE_SIZE;
            num_of_single = num_byte_to_write % GD25T512ME_PAGE_SIZE;

            status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, count);
            if(GD25T512ME_ERROR == status){
                return GD25T512ME_ERROR;
            }
            write_addr +=  count;
            pbuffer += count;

            while(num_of_page--) {
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, GD25T512ME_PAGE_SIZE);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
                write_addr +=  GD25T512ME_PAGE_SIZE;
                pbuffer += GD25T512ME_PAGE_SIZE;
            }

            if(num_of_single != 0) {
                status = qspi_flash_page_write(i_mode, addr_size, pbuffer, write_addr, num_of_single);
                if(GD25T512ME_ERROR == status){
                    return GD25T512ME_ERROR;
                }
            }
        }
    }
    return GD25T512ME_OK;
}

/*!
    \brief      read a block of data from the flash
    \param[in]  interface_mode: flash interface mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_MODE: SPI mode
      \arg        QSPI_MODE: QSPI mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[in]  pbuffer : pointer to the buffer
    \param[in]  write_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_buffer_read(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                            uint32_t read_addr, uint32_t num_byte_to_read)
{
    uint32_t timeout_max, timeout = 0;
    uint32_t timeout1 = QSPI_MAX_TOMEOUT;
    
    timeout_max = GD25T512ME_READ_MAX_TIME;
    /* configure interface mode */
    if(SPI_MODE == i_mode) {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_1_LINE;
        qspi_cmd.dummycycles      = 8;
        qspi_cmd.addr_mode        = QSPI_ADDR_1_LINE;
        qspi_cmd.data_mode        = QSPI_DATA_1_LINE;
    } else {
        qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
        qspi_cmd.dummycycles      = 16;
        qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
        qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    }

    /* initialize read command */
    if(GD25T512ME_3BYTES_SIZE == addr_size) {
        qspi_cmd.instruction = GD25T512ME_FAST_READ_CMD;
        qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
    } else {
        qspi_cmd.instruction = GD25T512ME_4_BYTE_ADDR_FAST_READ_CMD;
        qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
    }

    qspi_cmd.addr            = read_addr;
    qspi_cmd.data_length     = num_byte_to_read;
    qspi_cmd.altebytes_mode  = QSPI_ALTE_BYTES_NONE;
    qspi_cmd.sioo_mode       = QSPI_SIOO_INST_EVERY_CMD;

    /* send the command */
    qspi_command_config(&qspi_cmd);
    /* reception of the data */
    qspi_data_receive(pbuffer);
    /* wait for the data transmit completed */
    while((0U == qspi_flag_get(QSPI_FLAG_TC)) && (timeout1 > 0U)){
        timeout1--;
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);
    /* wait for the BUSY flag to be reset */
    do{
        timeout++;
    } while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout <= timeout_max));
    
    if(timeout > timeout_max) {
        return GD25T512ME_ERROR;
    }
    return GD25T512ME_OK;
}

/*!
    \brief      read a block of data from the flash using DDR mode
    \param[in]  addr_size: the size of address
                only one parameter can be selected which is shown as below:
      \arg        GD25T512ME_3BYTES_SIZE: 3 bytes address mode
      \arg        GD25T512ME_4BYTES_SIZE: 4 bytes address mode
    \param[in]  pbuffer : pointer to the buffer
    \param[in]  write_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \retval     error_code: GD25T512ME_ERROR, GD25T512ME_OK
*/
error_code qspi_flash_buffer_read_ddr(addr_size addr_size, uint8_t *pbuffer, uint32_t read_addr, uint32_t num_byte_to_read)
{
    uint32_t timeout_max, timeout = 0;
    uint32_t timeout1 = QSPI_MAX_TOMEOUT;
    
    timeout_max = GD25T512ME_READ_MAX_TIME;
    
    /* initialize quad DDR command */
    if(GD25T512ME_3BYTES_SIZE == addr_size) {
        qspi_cmd.addr_size   = QSPI_ADDR_24_BITS;
        qspi_cmd.instruction = GD25T512ME_QUAD_IO_DTR_FAST_READ_CMD;
    } else {
        qspi_cmd.addr_size   = QSPI_ADDR_32_BITS;
        qspi_cmd.instruction = GD25T512ME_4_BYTE_ADDR_QUAD_IO_DTR_FAST_READ_CMD;
    }

    qspi_cmd.instruction_mode = QSPI_INSTRUCTION_4_LINES;
    qspi_cmd.addr             = read_addr;
    qspi_cmd.addr_mode        = QSPI_ADDR_4_LINES;
    qspi_cmd.altebytes        = 0xFE;
    qspi_cmd.altebytes_mode   = QSPI_ALTE_BYTES_4_LINES;
    qspi_cmd.altebytes_size   = QSPI_ALTE_BYTES_8_BITS;
    qspi_cmd.data_mode        = QSPI_DATA_4_LINES;
    qspi_cmd.data_length      = num_byte_to_read;
    qspi_cmd.dummycycles      = 15;
    qspi_cmd.sioo_mode        = QSPI_SIOO_INST_EVERY_CMD;
    qspi_cmd.trans_rate       = QSPI_DDR_MODE;
    qspi_cmd.trans_delay      = 0;
    qspi_command_config(&qspi_cmd);

    qspi_data_receive(pbuffer);
    /* wait for the data receive completed */
    while((RESET == qspi_flag_get(QSPI_FLAG_TC)) && (timeout1 > 0U)){
        timeout1--;
    }
    /* clear the TC flag */
    qspi_flag_clear(QSPI_FLAG_TC);

    /* wait for the BUSY flag to be reset */
    do{
        timeout++;
    } while((RESET != qspi_flag_get(QSPI_FLAG_BUSY)) && (timeout <= timeout_max));
    
    if(timeout > timeout_max) {
        return GD25T512ME_ERROR;
    }
    return GD25T512ME_OK;
}
