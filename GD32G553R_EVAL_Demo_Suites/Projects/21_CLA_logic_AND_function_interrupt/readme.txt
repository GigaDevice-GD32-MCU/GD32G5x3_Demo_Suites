/*!
    \file    readme.txt
    \brief   description of the logic AND function interrupt of CLA0

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

  This demo is based on the GD32G553R-EVAL-V1.0 board, it shows the logic AND function interrupt. In 
this demo, PB6 and PB5 is selected as input of SIGS0 and SIGS1 respectively. PA2 is used as CLA0OUT. 

  Both PB5 and PB6 are configured as output. PB6 outputs HIGH while PB5 toggled every 200ms. CLA0 rising edge 
 interrupt is enabled. When rising edge of PB5 appears, the demo goes to CLA_IRQHandler and LED1toggles. 
 Connect the PB6, PB5 and PA2 to an oscilloscope to monitor waveform. The waveform will like this:

high            __________________________________________________   PB6(input)
               |
low     _______|

high             _________           _________           __________  PB5(input)
                |         |         |         |         |         
low      _______|         |_________|         |_________|           
                
                |<-200ms->|

high             _________           _________           _________   PA2(output)
                |         |         |         |         |
low     ________|         |_________|         |_________|

                |<-200ms->|
