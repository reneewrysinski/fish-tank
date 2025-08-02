/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"


// Neopixel docs - https://learn.adafruit.com/adafruit-neopixel-uberguide/advanced-coding

// Each Neopixel is controlled by 3 bytes. We are encoding each BIT in these bytes
// as an SPI 0b100 (for a 0) or 0b110 (for a 1). So that means that 3 Neopixel bytes (24 bits)
// become 9 SPI bytes (72 bits).

// Since the "Off" signal for a neopixel is 0x00, 0x00, 0x00, this translates to
// 0b100100100100100100 or 0x924924 924924 924924

uint8_t onTxPacket[] =   {0x92, 0x4D, 0xA6, 0x92, 0x49, 0x24, 0x92, 0x4D, 0xA6};

uint8_t offTxPacket[] =  {0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24};


uint8_t *txPacket;

int transmissionComplete = 0;
int idx = 0;
int repeat = 0;
int n_repeats = 256;

int message_len = sizeof(onTxPacket) / sizeof(onTxPacket[0]);

int main(void)
{
    SYSCFG_DL_init();

    while (1) {
        transmissionComplete = 0;
        idx = 1;
        repeat = 0;
        txPacket = onTxPacket;
        NVIC_ClearPendingIRQ(SPI_0_INST_INT_IRQN);
        NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);
        DL_SPI_transmitData8(SPI_0_INST, *txPacket);
        while (!transmissionComplete)
            __WFI();

        delay_cycles(400000);


        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 4000, DL_TIMER_CC_0_INDEX);
        transmissionComplete = 0;
        idx = 1;
        repeat = 0;
        txPacket = offTxPacket;
        NVIC_ClearPendingIRQ(SPI_0_INST_INT_IRQN);
        NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);
        DL_SPI_transmitData8(SPI_0_INST, *txPacket);
        while (!transmissionComplete)
            __WFI();

        delay_cycles(400000);
    }
}

void SPI_0_INST_IRQHandler(void)
{
    switch (DL_SPI_getPendingInterrupt(SPI_0_INST)) {
        case DL_SPI_IIDX_TX:
            DL_SPI_transmitData8(SPI_0_INST, txPacket[idx]);
            idx++;
            if (idx == message_len) {
                idx = 0;
                repeat = repeat + 1;
                if (repeat == n_repeats) {
                    transmissionComplete = 1;
                    NVIC_DisableIRQ(SPI_0_INST_INT_IRQN);
                }
            }
            break;
        default:
            break;
    }
}
