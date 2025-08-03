#include "ti_msp_dl_config.h"
#include "image_frames.h"

// === CHANGES: grid config
#define W 16
#define H 16
#define N_PIXELS (W*H)
#define SERPENTINE 1  // 1 = serpentine wiring, 0 = straight
#define STARTS_LEFT    0   // 1 = first LED is on the left edge of the first row; 0 = right edge
#define TOP_TO_BOTTOM  1   // 1 = first row is the top row; 0 = first row is the bottom row

// === CHANGES: encode LUT (byte -> 3 SPI bytes)
static uint8_t ENC[256][3];
static inline void build_encode_lut(void) {
    for (int v = 0; v < 256; ++v) {
        uint32_t acc = 0;
        for (int b = 7; b >= 0; --b) {
            uint32_t sym = ((v >> b) & 1) ? 0b110 : 0b100; // 1->110, 0->100
            acc = (acc << 3) | sym;
        }
        ENC[v][0] = (acc >> 16) & 0xFF;
        ENC[v][1] = (acc >>  8) & 0xFF;
        ENC[v][2] = (acc >>  0) & 0xFF;
    }
}

// 0 = off, 255 = full brightness
static volatile uint8_t g_brightness = 16;  // start at ~50%

// Fast 8-bit scale with rounding: (v * b) / 255
static inline uint8_t scale8(uint8_t v, uint8_t b)
{
    return (uint8_t)(((uint16_t)v * (uint16_t)b + 128) >> 8);
}

static inline void encode_pixel_GRB(const uint8_t grb[3], uint8_t *dst9){
    uint8_t G=scale8(grb[0], g_brightness);
    uint8_t R=scale8(grb[1], g_brightness);
    uint8_t B=scale8(grb[2], g_brightness);
    dst9[0]=ENC[G][0]; dst9[1]=ENC[G][1]; dst9[2]=ENC[G][2];
    dst9[3]=ENC[R][0]; dst9[4]=ENC[R][1]; dst9[5]=ENC[R][2];
    dst9[6]=ENC[B][0]; dst9[7]=ENC[B][1]; dst9[8]=ENC[B][2];
}

// === CHANGES: image + mapping
static uint8_t image_grb[N_PIXELS][3];            // GRB per pixel (input)
static uint8_t frame_encoded[N_PIXELS * 9];       // encoded output (9 bytes/pixel)

static inline int map_xy_to_index(int x, int y)
{
    // Optionally flip coordinates based on start corner
    int ry = TOP_TO_BOTTOM ? y : (H - 1 - y);
    int rx;

#if SERPENTINE
    // On serpentine panels, every other row reverses
    if (STARTS_LEFT) {
        // even rows L->R, odd rows R->L
        rx = (ry & 1) ? (W - 1 - x) : x;
    } else {
        // even rows R->L, odd rows L->R
        rx = (ry & 1) ? x : (W - 1 - x);
    }
#else
    // Straight wiring (every row same direction)
    rx = STARTS_LEFT ? x : (W - 1 - x);
#endif

    return ry * W + rx;
}

static void build_frame_from_image(void) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int strip_i = map_xy_to_index(x, y);
            encode_pixel_GRB(image_grb[y*W + x], &frame_encoded[strip_i * 9]);
        }
    }
}

static const uint8_t* const FISH_SLEEP_FRAMES[] = {
    FISH_SLEEP_0, FISH_SLEEP_1, FISH_SLEEP_2, FISH_SLEEP_3, FISH_SLEEP_4,
    FISH_SLEEP_5, FISH_SLEEP_6, FISH_SLEEP_7, FISH_SLEEP_8, FISH_SLEEP_9,
    FISH_SLEEP_10, FISH_SLEEP_11, FISH_SLEEP_12, FISH_SLEEP_13, FISH_SLEEP_14,
    FISH_SLEEP_15, FISH_SLEEP_16, FISH_SLEEP_17, FISH_SLEEP_18, FISH_SLEEP_19,
    FISH_SLEEP_20, FISH_SLEEP_21, FISH_SLEEP_22
};
_Static_assert(sizeof(FISH_SLEEP_FRAMES)/sizeof(FISH_SLEEP_FRAMES[0]) == FISH_SLEEP_FRAME_COUNT, "frame count mismatch");

static const uint8_t* const FISH_SWIM_FRAMES[] = {
    FISH_SWIM_0, FISH_SWIM_1, FISH_SWIM_2, FISH_SWIM_3, FISH_SWIM_4,
    FISH_SWIM_5, FISH_SWIM_6, FISH_SWIM_7, FISH_SWIM_8, FISH_SWIM_9,
    FISH_SWIM_10, FISH_SWIM_11, FISH_SWIM_12, FISH_SWIM_13, FISH_SWIM_14,
    FISH_SWIM_15, FISH_SWIM_16, FISH_SWIM_17, FISH_SWIM_18, FISH_SWIM_19
};
_Static_assert(sizeof(FISH_SWIM_FRAMES)/sizeof(FISH_SWIM_FRAMES[0]) == FISH_SWIM_FRAME_COUNT, "frame count mismatch");

static const uint8_t* const FISH_FEED_FRAMES[] = {
    FISH_FEED_0, FISH_FEED_1, FISH_FEED_2, FISH_FEED_3, FISH_FEED_4,
    FISH_FEED_5, FISH_FEED_6, FISH_FEED_7, FISH_FEED_8, FISH_FEED_9,
    FISH_FEED_10, FISH_FEED_11, FISH_FEED_12
};
_Static_assert(sizeof(FISH_FEED_FRAMES)/sizeof(FISH_FEED_FRAMES[0]) == FISH_FEED_FRAME_COUNT, "frame count mismatch");

void show_swim(uint32_t loops);
void show_feed(uint32_t loops);
void show_sleep(uint32_t loops);

// Copy an RGB888 image (R,G,B order) into image_grb[] honoring serpentine mapping.
// If the source image (srcW×srcH) differs from panel (W×H), this will center and crop.
static void load_image_rgb888(const uint8_t *src, int srcW, int srcH)
{
    // Compute top-left where the source maps on our panel (centered)
    int x0 = (W - srcW) / 2;
    int y0 = (H - srcH) / 2;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            uint8_t r = 0, g = 0, b = 0;

            // If (x,y) falls inside the source image region, sample it; else black.
            int sx = x - x0;
            int sy = y - y0;
            if ((unsigned)sx < (unsigned)srcW && (unsigned)sy < (unsigned)srcH) {
                int sidx = (sy * srcW + sx) * 3; // RGB888 stride
                r = src[sidx + 0];
                g = src[sidx + 1];
                b = src[sidx + 2];
            }

            // Store in GRB order for WS2812
            int i = y * W + x;
            image_grb[i][0] = g;
            image_grb[i][1] = r;
            image_grb[i][2] = b;
        }
    }
}

// === ORIGINAL globals, lightly adjusted
uint8_t *txPacket;

volatile int transmissionComplete = 0;
volatile int idx = 0;
volatile int repeat = 0;

// === CHANGES: send each frame once
volatile int n_repeats = 1;

// === CHANGES: message_len will be N_PIXELS*9 for the frame
int message_len = 0;

static void send_frame(uint8_t *buf, int len) {
    transmissionComplete = 0;
    idx = 1;
    repeat = 0;
    txPacket = buf;
    message_len = len;
    NVIC_ClearPendingIRQ(SPI_0_INST_INT_IRQN);
    NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);
    DL_SPI_transmitData8(SPI_0_INST, txPacket[0]); // kick off first byte
    while (!transmissionComplete) __WFI();

    // WS2812 reset/latch low time (>50us). Adjust for your clock.
    delay_cycles(400000);
}

// test grid main
//int main(void)
//{
//    SYSCFG_DL_init();
//
//    // === CHANGES: one-time LUT and initial image
//    build_encode_lut();
//    load_image_rgb888(IMG_RGB888_HALF, IMG_W, IMG_H);
//    build_frame_from_image();
//    send_frame(frame_encoded, N_PIXELS * 9);
//
//    while (1) {
//        // === CHANGES: transmit full image once
//        send_frame(frame_encoded, N_PIXELS * 9);
//
//        // If static image, you can pause or loop.
//        // For animation, update image_grb[], rebuild, and send again.
//        delay_cycles(400000); // simple pause between frames
//    }
//}

// main that displays fish sleep animation
int main(void)
{
    SYSCFG_DL_init();
    build_encode_lut();
    while (1) {
            show_swim(2);  // e.g., 2 cycles
            show_feed(1);
            show_swim(2);
            show_sleep(1);
    }
}

static void play(const uint8_t* const frames[], uint32_t count,
                 uint32_t loops, uint32_t frame_delay_cycles)
{
    for (uint32_t l = 0; l < loops; ++l) {
        for (uint32_t frame = 0; frame < count; ++frame) {
            load_image_rgb888(frames[frame], IMG_W, IMG_H);
            build_frame_from_image();
            send_frame(frame_encoded, N_PIXELS * 9);
            delay_cycles(frame_delay_cycles);
        }
    }
}

void show_swim (uint32_t loops){ play(FISH_SWIM_FRAMES,  FISH_SWIM_FRAME_COUNT,  loops, 4000000); }
void show_feed (uint32_t loops){ play(FISH_FEED_FRAMES,  FISH_FEED_FRAME_COUNT,  loops, 4000000); }
void show_sleep(uint32_t loops){ play(FISH_SLEEP_FRAMES, FISH_SLEEP_FRAME_COUNT, loops, 4000000); }

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
