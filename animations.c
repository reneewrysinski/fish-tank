#include "animations.h"
#include "display.h"
#include "image_frames.h"
#include "ti_msp_dl_config.h"

uint8_t frame_encoded[N_PIXELS * 9];

// base function for showing basic animations since they all basically do the same (all except game)
void play(const uint8_t* const frames[], uint32_t count,
                 uint32_t loops, uint32_t frame_delay_cycles)
{
    for (uint32_t l = 0; l < loops; ++l) {
        for (uint32_t frame = 0; frame < count; ++frame) {
            load_image_rgb(frames[frame], IMG_W, IMG_H);
            build_frame_from_image();
            send_frame(frame_encoded, N_PIXELS * 9);
            delay_cycles(frame_delay_cycles);
        }
    }
}

void show_swim(uint32_t loops){ play(FISH_SWIM_FRAMES,  FISH_SWIM_FRAME_COUNT,  loops, 4000000); }
void show_feed(uint32_t loops){ play(FISH_FEED_FRAMES,  FISH_FEED_FRAME_COUNT,  loops, 4000000); }
void show_sleep(uint32_t loops){ play(FISH_SLEEP_FRAMES, FISH_SLEEP_FRAME_COUNT, loops, 4000000); }
void show_sleeping(uint32_t loops){ play(FISH_SLEEPING_FRAMES, FISH_SLEEPING_FRAME_COUNT, loops, 4000000); }
