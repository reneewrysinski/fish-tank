#include "ti_msp_dl_config.h"
#include "image_frames.h"
#include "animations.h"
#include "display.h"

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
    SYSCFG_DL_init();
    build_encode_lut();
    while (1) {
            show_swim(2);
            show_feed(1);
            show_swim(2);
            show_sleep(1);
            show_sleeping(10);

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
