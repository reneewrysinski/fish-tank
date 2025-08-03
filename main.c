#include "ti_msp_dl_config.h"
#include "image_frames.h"
#include "animations.h"
#include "display.h"
#include "lab6_helper.h"
#include "effects.h"
#include "buttons.h"

// test grid main
//int main(void)
//{
//    SYSCFG_DL_init();
//
//    build_encode_lut();
//    load_image_rgb(IMG_RGB888_HALF, IMG_W, IMG_H);
//    build_frame_from_image();
//    send_frame(frame_encoded, N_PIXELS * 9);
//
//    while (1) {
//        // transmit full image once
//        send_frame(frame_encoded, N_PIXELS * 9);
//
//        // if static image, can pause or loop
//        // for animation, update image_grb[], rebuild, and send again
//        delay_cycles(400000); // simple pause between frames
//    }
//}

// actual main

// states:
// 0 = start up (only happens on startup)
// 1 = swim (default state)
// 2 = swim fast (only when SW1 held)
// 3 = play game (after SW2 pressed until game exited)
// 4 = feed (happens once when SW3 pressed then back to swim)
// 5 = sleep (sleep animation happens once then stays in sleeping animation until button pressed)
uint8_t state = 0;
int main(void)
{
    InitializeProcessor();
    InitializeTimerG0();
    InitializeGPIO();
    NVIC_EnableIRQ(TIMG0_INT_IRQn); // Enable the timer interrupt
    TIMG0->COUNTERREGS.LOAD = 326; // Set timer to N-1 for ten milliseconds
    TIMG0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
    SYSCFG_DL_init();
    build_encode_lut();
    uint8_t SW1_pressed = 0;
    uint8_t SW2_pressed = 0;
    uint8_t SW3_pressed = 0;
    uint8_t SW4_pressed = 0;
    while (1) {
        switch (state) {
            case 0: // start up animation (only happens once on startup)
                show_intro(1);
                state = 1;
                break;
            case 1: // swim (default state)
                while (1) {
                    show_swim(1);
                    if (any_button_on()) {
                        SW1_pressed = button_on(SW1);
                        SW2_pressed = button_on(SW2);
                        SW3_pressed = button_on(SW3);
                        SW4_pressed = button_on(SW4);
                        if (SW1_pressed) {
                            state = 2;
                            break;
                        }
                        if (SW2_pressed) {
                            state = 3;
                            break;
                        }
                        if (SW3_pressed) {
                            state = 4;
                            break;
                        }
                        if (SW4_pressed) {
                            state = 5;
                            break;
                        }
                    }
                }
                break;
            case 2: // swim fast (only when SW1 held)
                while (1) {
                    show_swim_fast(1);
                    SW1_pressed = button_on(SW1);
                    if (!SW1_pressed) {
                        state = 1;
                        break;
                    }
                }
                break;
            case 3: // play game (after SW2 pressed until game exited)
                while (play_game(20000000, 4000000)) {}
                state = 1;
                break;
            case 4: // feed (happens once when SW3 pressed then back to swim)
                show_feed(1);
                state = 1;
                break;
            case 5: // sleep (sleep animation happens once then stays in sleeping animation until any button pressed)
                show_sleep(1);
                while(1) {
                    show_sleeping(1);
                    if (any_button_on()) {
                        state = 1;
                        break;
                    }
                }
                break;

        }
    }
//  old while loop
//    while (1) {
//        play_game(20000000, 4000000);
//            show_swim(2);
//            show_feed(1);
//            show_swim(2);
//            show_sleep(1);
//            show_sleeping(10);
//    }
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

void TIMG0_IRQHandler(void)
{
    // This wakes up the processor!

    switch (TIMG0->CPU_INT.IIDX) {
        case GPTIMER_CPU_INT_IIDX_STAT_Z: // Counted down to zero event.
            timerTicked = 1; // set a flag so we can know what woke us up.
            break;
        default:
            break;
    }
}
