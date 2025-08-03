#include "ti_msp_dl_config.h"
#include "image_frames.h"
#include "animations.h"
#include "display.h"
#include "lab6_helper.h"
#include "effects.h"

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
    while (1) {
        play_game(20000000, 4000000);
//            show_swim(2);
//            show_feed(1);
//            show_swim(2);
//            show_sleep(1);
//            show_sleeping(10);

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
