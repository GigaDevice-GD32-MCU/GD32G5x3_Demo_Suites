/*!
    \file    gd25t512xx.h
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
#ifndef GD25T512ME_H
#define GD25T512ME_H

#include "gd32g5x3_qspi.h"

/* GD25T512ME size configuration */
#define GD25T512ME_BLOCK_64K                    (uint32_t)(16*4*1024)                 /* 16 sectors of 64KBytes */
#define GD25T512ME_SECTOR_4K                    (uint32_t)(4 * 1024)                  /* 1 sectors of 4KBytes */

#define GD25T512ME_FLASH_SIZE                   (uint32_t)(512*1024*1024/8)           /* 512 Mbits => 64MBytes */
#define GD25T512ME_PAGE_SIZE                    (uint32_t)256                         /* 262144 pages of 256 Bytes */

/* GD25T512ME timing configuration */
#define GD25T512ME_BULK_ERASE_MAX_TIME          460000U
#define GD25T512ME_BLOCK_ERASE_MAX_TIME         2400U
#define GD25T512ME_SECTOR_ERASE_MAX_TIME        400U
#define GD25T512ME_PAGE_PROGRAM_MAX_TIME        400U
#define GD25T512ME_READ_MAX_TIME                2000U

/* GD25T512ME commands */
/* read/write memory operations with 3-bytes address */
#define GD25T512ME_READ_CMD                                   0x03U       /* normal read 3 bytes address, only for 1-SPI */
#define GD25T512ME_FAST_READ_CMD                              0x0BU       /* fast read 3 bytes address */
#define GD25T512ME_QUAD_OUTPUT_FAST_READ_CMD                  0x6BU       /* quad output fast read 3 bytes address */
#define GD25T512ME_QUAD_IO_FAST_READ_CMD                      0xEBU       /* quad I/O fast read 3 bytes address */
#define GD25T512ME_QUAD_IO_DTR_FAST_READ_CMD                  0xEDU       /* dtr quad I/O fast read 3 bytes address */

#define GD25T512ME_PAGE_PROG_CMD                              0x02U       /* page program 3 bytes address */
#define GD25T512ME_QUAD_PAGE_PROG_CMD                         0x32U       /* quad page program 3 bytes address */
#define GD25T512ME_EXT_QUAD_PAGE_PROG_CMD                     0xC2U       /* extended quad page program 3 bytes address */

#define GD25T512ME_SECTOR_ERASE_4K_CMD                        0x20U       /* sector erase 4KB 3 bytes address */
#define GD25T512ME_BLOCK_ERASE_32K_CMD                        0x52U       /* block erase 32KB 3 bytes address */
#define GD25T512ME_BLOCK_ERASE_64K_CMD                        0xD8U       /* block erase 64KB 3 bytes address */
#define GD25T512ME_CHIP_ERASE_CMD                             0x60U       /* chip erase */

/* read/write memory operations with 4-bytes address */
#define GD25T512ME_4_BYTE_ADDR_READ_CMD                       0x13U       /* normal read 4 bytes address */
#define GD25T512ME_4_BYTE_ADDR_FAST_READ_CMD                  0x0CU       /* fast read 4 bytes address */
#define GD25T512ME_4_BYTE_ADDR_QUAD_OUTPUT_FAST_READ_CMD      0x6CU       /* quad output fast read 4 bytes address */
#define GD25T512ME_4_BYTE_ADDR_QUAD_IO_FAST_READ_CMD          0xECU       /* quad I/O fast read 4 bytes address */
#define GD25T512ME_4_BYTE_ADDR_QUAD_IO_DTR_FAST_READ_CMD      0xEEU       /* quad I/O DTR fast read 4 bytes address */

#define GD25T512ME_4_BYTE_PAGE_PROG_CMD                       0x12U       /* page program 4 bytes address */
#define GD25T512ME_4_BYTE_QUAD_PAGE_PROG_CMD                  0x34U       /* quad page program 4 bytes address */
#define GD25T512ME_4_BYTE_EXT_QUAD_PAGE_PROG_CMD              0x3EU       /* extended quad page program 4 byte address */

#define GD25T512ME_4_BYTE_SECTOR_ERASE_4K_CMD                 0x21U       /* sector erase 4KB 4 bytes address */
#define GD25T512ME_4_BYTE_BLOCK_ERASE_32K_CMD                 0x5CU       /* block erase 32KB 4 byte address */
#define GD25T512ME_4_BYTE_BLOCK_ERASE_64K_CMD                 0xDCU       /* block erase 64KB 4 byte address */

/* setting commands */
#define GD25T512ME_WRITE_ENABLE_CMD                           0x06U       /* write enable */
#define GD25T512ME_WRITE_DISABLE_CMD                          0x04U       /* write disable */
#define GD25T512ME_WRITE_SR_ENABLE_CMD                        0x50U       /* volatile SR write enable */
#define GD25T512ME_PROG_ERASE_SUSPEND_CMD                     0x75U       /* program/erase suspend */
#define GD25T512ME_PROG_ERASE_RESUME_CMD                      0x7AU       /* program/erase resume */
#define GD25T512ME_ENTER_DEEP_POWER_DOWN_CMD                  0xB9U       /* enter deep power down */
#define GD25T512ME_RELEASE_FROM_DEEP_POWER_DOWN_CMD           0xABU       /* release form deep power down */
#define GD25T512ME_ENABLE_4_BYTE_ADDR_CMD                     0xB7U       /* enable 4 byte address mode */
#define GD25T512ME_DISABLE_4_BYTE_ADDR_CMD                    0xE9U       /* disable 4 byte address mode */
#define GD25T512ME_ENABLE_QPI_CMD                             0x38U       /* enter QSPI mode */
#define GD25T512ME_WRITE_VOLATILE_CFG_REG_CMD                 0x81U       /* write volatile configuration register */

/* reset commands */
#define GD25T512ME_RESET_ENABLE_CMD                           0x66U       /*!< reset enable */
#define GD25T512ME_RESET_MEMORY_CMD                           0x99U       /*!< reset memory */

/* register commands */
#define GD25T512ME_READ_ID_CMD                                0x9FU       /* read identification */
#define GD25T512ME_READ_STATUS_REG1_CMD                       0x05U       /* read status register */

/* status register */
#define GD25T512ME_SR_WIP                                     0x01U       /* write in progress */
#define GD25T512ME_SR_WEL                                     0x02U       /* write enable latch */
#define GD25T512ME_SR_PB                                      0x3CU       /* block protected against program and erase operations */

typedef enum {
    GD25T512ME_OK = 0,                        /* ok */
    GD25T512ME_ERROR                          /* fail */
} error_code;

typedef enum {
    SPI_MODE = 0,                             /* 1-1-1 commands, power on H/W default setting  */
    QSPI_MODE                                 /* 4-4-4 commands */
} interface_mode;

typedef enum {
    GD25T512ME_3BYTES_SIZE = 0,               /* 3 bytes address mode */
    GD25T512ME_4BYTES_SIZE                    /* 4 bytes address mode */
} addr_size;

typedef enum {
    GD25T512ME_ERASE_4K = 0,                  /* 4K size sector erase */
    GD25T512ME_ERASE_64K,                     /* 64K size block erase */
} erase_size;

/* initialize QSPI and GPIO */
void qspi_flash_init(void);
/* read the flah id */
uint32_t qspi_flash_read_id(interface_mode i_mode);
/* enable flash reset */
void qspi_flash_reset_enable(interface_mode i_mode);
/* reset the flash */
void qspi_flash_reset_memory(interface_mode i_mode);
/* enable flash write */
void qspi_flash_write_enbale(interface_mode i_mode);
/* poll the status of the write in progress(wip) flag in the flash's status register */
void qspi_flash_autopolling_mem_ready(interface_mode i_mode);
/* enable flash QPI mode */
void qspi_flash_quad_enbale(void);
/* enable flash DDR mode */
void qspi_flash_ddr_enbale(addr_size addr_size);
/* erase the specified block of the flash */
error_code qspi_flash_block_erase(interface_mode i_mode, addr_size addr_size, uint32_t addr,
                            erase_size block_size);
/* erase the the entire flash */
error_code qspi_flash_chip_erase(interface_mode i_mode);
/* write a page of data to the flash */
error_code qspi_flash_page_write(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                           uint32_t write_addr, uint32_t num_byte_to_write);
/* write a block of data to the flash */
error_code qspi_flash_buffer_write(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                             uint32_t write_addr, uint32_t num_byte_to_write);
/* read a block of data from the flash */
error_code qspi_flash_buffer_read(interface_mode i_mode, addr_size addr_size, uint8_t *pbuffer,
                            uint32_t read_addr, uint32_t num_byte_to_read);
/* read a block of data from the flash using DDR mode */
error_code qspi_flash_buffer_read_ddr(addr_size addr_size, uint8_t *pbuffer, uint32_t read_addr,
                                uint32_t num_byte_to_read);
#endif /* #define GD25T512ME_H */
