#include <ti/devices/msp/msp.h>

#include "lab6_helper.h"
#include "effects.h"
#include "buttons.h"

int transmissionComplete = 0; // flag for SPI ISR wakeup
int timerTicked = 0; // flag for timer ISR wakeup
int idx = 0;

uint16_t txPacket[] =  {0x0, 0x0, 0xE5F0, 0x1010, 0xE510, 0x10F0, 0xE510, 0xF010, 0xE510, 0x0010, 0xFFFF, 0xFFFF};
int message_len = sizeof(txPacket) / sizeof(txPacket[0]);

void pause(){
    // Clear pending SPI interrupts
    NVIC_ClearPendingIRQ(SPI0_INT_IRQn);
    NVIC_EnableIRQ(SPI0_INT_IRQn);
    transmissionComplete = 0; // reset flag
    idx = 1; // reset pointer to point at the second element of the SPI message
    SPI0->TXDATA = *txPacket; // This will start TX ISR running.
    // It will stop itself at the end of the message, and disable SPI interrupts.

    while (!timerTicked) // Wait for timer wake up
        __WFI();

    timerTicked = 0; // reset timer interrupt flag
}

void set_lights(int L1, int L2, int L3, int L4){
    if (L1)
        txPacket[2] = 0xE5F0;
    else
        txPacket[2] = 0xE000;

    if (L2)
        txPacket[4] = 0xE510;
    else
        txPacket[4] = 0xE000;

    if (L3)
        txPacket[6] = 0xE510;
    else
        txPacket[6] = 0xE000;

    if (L4)
        txPacket[8] = 0xE510;
    else
        txPacket[8] = 0xE000;
}

void set_buzzer_freq(float freq){
    TIMA1->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
    TIMA1->COUNTERREGS.LOAD = (int)(8000000 / freq) - 1; // Period is LOAD+1 - this is 8000000/4000 = 2kHz
    // HEADS UP: This sets the frequency of the buzzer!
    TIMA1->COUNTERREGS.CC_01[0] = (TIMA1->COUNTERREGS.LOAD  + 1) / 2; // half of load to make this a square wave
}

void buzzer_off(){
    TIMA1->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED); // Disable the buzzer
}

int wait(int steps) {
    for (int i = 0; i < steps; i++) {
        pause();
        if (any_button_on()) {
            return 1;
        }
    }
    return 0;

}

int play_note(float note, int duration) {
    set_buzzer_freq(note);
    if (wait(duration))
        return 1;
    buzzer_off();
    if (wait(5))
        return 1;
    return 0;
}

int wait2(int steps) {
    for (int i = 0; i < steps; i++) {
        pause();
    }
    return 0;

}
