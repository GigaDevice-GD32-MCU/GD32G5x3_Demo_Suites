/*!
    \file    main.h
    \brief   the header of main

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

#ifndef MAIN_H
#define MAIN_H

/* plain text size in bytes, it must be a multiple of 16(128 bits = 16 bytes) for AES algorithm */
#define TEXT_SIZE                   64
#define IV_SIZE                     16
#define AAD_SIZE                    16
#define GCM_TAG_SIZE                16
#define CCM_IV_SIZE                 8
#define CCM_TAG_SIZE                6
#define ECB                         1
#define CBC                         2
#define CTR                         3
#define DES                         "DES"
#define TDES                        "TDES"
#define AES128                      "AES128"
#define AES192                      "AES192"
#define AES256                      "AES256"

#define ALGORITHM_USE_DES           (0x01)
#define ALGORITHM_USE_TDES          (0x02)
#define ALGORITHM_USE_AES           (0x04)
#define MODE_USE_ECB                (0x10)
#define MODE_USE_CBC                (0x20)
#define MODE_USE_CTR                (0x40)
#define MODE_USE_GCM                (0x80)
#define MODE_USE_CCM                (0x100)
#define MODE_USE_CFB                (0x200)
#define MODE_USE_OFB                (0x400)

/* key with 128 bits */
__attribute__((aligned(4)))
uint8_t aes_key_128[16] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                           0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70
                          };//(ASCII)abcdefghijklmnop

/* key with 192 bits */
__attribute__((aligned(4)))
uint8_t aes_key_192[24] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                           0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
                           0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
                          };//(ASCII)abcdefghijklmnoprstuvwx

/* key with 256 bits */
__attribute__((aligned(4)))
uint8_t aes_key_256[32] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                           0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
                           0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
                           0x79, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A
                          };//(ASCII)abcdefghijklmnoprstuvwxyzzzzzzz

/* key selection */
__attribute__((aligned(4)))
uint8_t *key_select[3] = {aes_key_128, aes_key_192, aes_key_256};
/* key size selection */
__attribute__((aligned(4)))
uint16_t keysize[3] = {128, 192, 256};

/* key with 64 bits for DES */
__attribute__((aligned(4)))
uint8_t des_key[8] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68};//(ASCII)abcdefgh

/* vectors for des cbc */
__attribute__((aligned(4)))
uint8_t cbc_vectors[8] = {0x49, 0x4A, 0x4B, 0x4C, 0x00, 0x00, 0x00, 0x02};//(ASCII)IJKL + [0x00, 0x00, 0x00, 0x02]

/* key with 192 bits for TDES */
__attribute__((aligned(4)))
uint8_t tdes_key[24] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
                        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
                       };//(ASCII)abcdefghijklmnopqrstuvwx
/* vectors with 128 bits */
__attribute__((aligned(4)))
uint8_t vectors[16] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                       0x49, 0x4A, 0x4B, 0x4C, 0x00, 0x00, 0x00, 0x02
                      };//(ASCII)IJKL + [0x00, 0x00, 0x00, 0x02]

/* vectors for CCM mode */
__attribute__((aligned(4)))
uint8_t ccm_vectors[CCM_IV_SIZE] = {0x49, 0x4A, 0x4B, 0x4C, 0x00, 0x00, 0x00, 0x02};//(ASCII)IJKL + [0x00, 0x00, 0x00, 0x02]

/* message to be encrypted */
__attribute__((aligned(4)))
uint8_t plaintext[TEXT_SIZE] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
    0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
    0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
    0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7A, 0x7A
};

/* AAD message for GCM and CCM mode */
__attribute__((aligned(4)))
uint8_t addmessage[AAD_SIZE] = {0x47, 0x44, 0x33, 0x32, 0x2D, 0x4D, 0x43, 0x55,
                                0x47, 0x44, 0x33, 0x32, 0x2D, 0x4D, 0x43, 0x55
                               };//(ASCII)GD-32MCUGD-32MCU

#endif /* MAIN_H */
