#include "ti_msp_dl_config.h"
#include "animations.h"
#include "display.h"
#include "image_frames.h"
#include "effects.h"
#include "buttons.h"

uint8_t frame_encoded[N_PIXELS * 9];

uint8_t get_num_frames(const uint8_t** array){
    uint8_t i = 0;
    while (array[i] != NULL) {
        i++;
    }
    return i;
}

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

void show_swim(uint32_t loops){ play(FISH_SWIM_FRAMES,  get_num_frames(FISH_SWIM_FRAMES),  loops, 4000000); }
void show_feed(uint32_t loops){ play(FISH_FEED_FRAMES,  get_num_frames(FISH_FEED_FRAMES),  loops, 4000000); }
void show_sleep(uint32_t loops){ play(FISH_SLEEP_FRAMES, get_num_frames(FISH_SLEEP_FRAMES), loops, 4000000); }
void show_sleeping(uint32_t loops){ play(FISH_SLEEPING_FRAMES, get_num_frames(FISH_SLEEPING_FRAMES), loops, 4000000); }
