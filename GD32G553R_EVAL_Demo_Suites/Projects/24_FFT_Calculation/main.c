/*!
    \file    main.c
    \brief   FFT calculation

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
#include <stdio.h>
#include "gd32g553r_eval.h"
#include "math.h"


#define FERQ_SAMPLE         1024        //uint:Hz
#define PI                  3.1415926f
#define FFT_DATA_LENGTH     128U

float fft_real_buf[FFT_DATA_LENGTH] = {0};
/* output data */
float fft_output_buf[FFT_DATA_LENGTH*2] = {0};

/* printf FFT input data */
void display_inputdata(uint32_t length, float* indata);
/* printf FFT output data */
void display_outputdata(uint32_t length, float* outdata);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t index = 0;
    /* fft parameter variable */
    fft_parameter_struct fftconfig;

    /* configure COM port */
    gd_eval_com_init(EVAL_COM);
    
    /* Specify the parameters of a signal with a sampling frequency of 1024Hz and a signal duration of 0.125 seconds.
    Form a signal containing a 48 Hz sinusoid of amplitude 0.7 and a 120 Hz sinusoid of amplitude 1 
    input data = 0.7*sin(2*pi*48*t) + sin(2*pi*120*t) */
    for(index=0; index<FFT_DATA_LENGTH; index++){
        fft_real_buf[index] = 0.7*sin(2*PI*48*index/FERQ_SAMPLE) + sin(2*PI*120*index/FERQ_SAMPLE);
    }
    
    /* enable FFT clock */
    rcu_periph_clock_enable(RCU_FFT);
    
    /* reset the FFT */
    fft_deinit();
    
    /* initialize the FFT filter parameter struct */
    fft_struct_para_init(&fftconfig);

    /* setting FFT parameter */
    fftconfig.mode_sel      = FFT_MODE;
    fftconfig.point_num     = FFT_POINT_128;
    fftconfig.window_enable = FFT_WINDOW_DISABLE;
    fftconfig.downsamp_sel  = FFT_DOWNSAMPLE_1;
    fftconfig.image_source  = FFT_IM_ZERO;
    fftconfig.loopbuf_len   = 0U;
    fftconfig.loopbuf_index = 0U;
    fftconfig.real_addr     = (uint32_t)fft_real_buf;
    fftconfig.image_addr    = 0U;
    fftconfig.window_addr   = 0U;
    fftconfig.output_addr   = (uint32_t)fft_output_buf;
    
    /* config FFT parameter */
    fft_init(&fftconfig);
    
    /* start FFT calculation */
    fft_calculation_start();
    
    /* display input data */
    printf("FFT input data:\r\n");
    printf("\r\n");
    display_inputdata(FFT_DATA_LENGTH, fft_real_buf);
    
    /* wait calculation completion */
    while(RESET == fft_flag_get(FFT_FLAG_CCF));
    
    printf("\r\n--------------------------------\r\n");
    printf("FFT calculation completion!\r\n");
    printf("------------------------------------\r\n");
    
    /* display result */
    printf("\r\nFFT calculation result:\r\n");
    printf("\r\n");
    display_outputdata(FFT_DATA_LENGTH, fft_output_buf);

    while(1){
    }
}

/*!
    \brief      printf data by a line of 4 float number
    \param[in]  length: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
void display_inputdata(uint32_t length, float* data)
{
    for(int i=0; i < length; i++){
        printf("%f", data[i]);
        printf("+");
        printf("0*i");
        printf(", ");
    }
}

/*!
    \brief      printf data by a line of 4 float number
    \param[in]  length: length of the data to display
    \param[in]  data: pointer to the data to display
    \param[out] none
    \retval     none
*/
void display_outputdata(uint32_t length, float* data)
{
    for(int i=0; i < length; i++){
        printf("%f", data[2*i]);
        printf("+");
        printf("(%f*i)", data[2*i+1]);
        printf(", ");
    }
}
