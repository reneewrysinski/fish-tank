#include "ti_msp_dl_config.h"
#include "image_frames.h"

#ifndef animations_h
#define animations_h
#endif

static const uint8_t* const FISH_SLEEP_FRAMES[] = {
    FISH_SLEEP_0, FISH_SLEEP_1, FISH_SLEEP_2, FISH_SLEEP_3, FISH_SLEEP_4,
    FISH_SLEEP_5, FISH_SLEEP_6, FISH_SLEEP_7, FISH_SLEEP_8, FISH_SLEEP_9,
    FISH_SLEEP_10, FISH_SLEEP_11, FISH_SLEEP_12, FISH_SLEEP_13, FISH_SLEEP_14,
    FISH_SLEEP_15, FISH_SLEEP_16, FISH_SLEEP_17, FISH_SLEEP_18, FISH_SLEEP_19,
    FISH_SLEEP_20, FISH_SLEEP_21, FISH_SLEEP_22
};

static const uint8_t* const FISH_SLEEPING_FRAMES[] = {
    FISH_SLEEP_19, FISH_SLEEP_20, FISH_SLEEP_21, FISH_SLEEP_22
};

static const uint8_t* const FISH_SWIM_FRAMES[] = {
    FISH_SWIM_0, FISH_SWIM_1, FISH_SWIM_2, FISH_SWIM_3, FISH_SWIM_4,
    FISH_SWIM_5, FISH_SWIM_6, FISH_SWIM_7, FISH_SWIM_8, FISH_SWIM_9,
    FISH_SWIM_10, FISH_SWIM_11, FISH_SWIM_12, FISH_SWIM_13, FISH_SWIM_14,
    FISH_SWIM_15, FISH_SWIM_16, FISH_SWIM_17, FISH_SWIM_18, FISH_SWIM_19
};

static const uint8_t* const FISH_FEED_FRAMES[] = {
    FISH_FEED_0, FISH_FEED_1, FISH_FEED_2, FISH_FEED_3, FISH_FEED_4,
    FISH_FEED_5, FISH_FEED_6, FISH_FEED_7, FISH_FEED_8, FISH_FEED_9,
    FISH_FEED_10, FISH_FEED_11, FISH_FEED_12
};

void show_swim(uint32_t loops);
void show_feed(uint32_t loops);
void show_sleep(uint32_t loops);
void show_sleeping(uint32_t loops);
