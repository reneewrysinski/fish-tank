#include "ti_msp_dl_config.h"
#include "display.h"

// 0 = off, 255 = full brightness
static volatile uint8_t g_brightness = 16;  // start at ~50%

// variables for raw grb image data and encoded to neopixel format data
uint8_t image_grb[N_PIXELS][3];            // GRB per pixel (input)
uint8_t frame_encoded[N_PIXELS * 9];       // encoded output (9 bytes per pixel)

uint8_t *txPacket;
volatile int transmissionComplete = 0;
volatile int idx = 0;
volatile int repeat = 0;

// how many times to send each frame
volatile int n_repeats = 1;

// will be set to N_PIXELS * 9
int message_len = 0;

// lookup table for all possible RGB to neopixel SPI bits so we aren't converting during
uint8_t ENC[256][3];
void build_encode_lut(void) {
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

// scale function for adjusting brightness
uint8_t scale8(uint8_t v, uint8_t b)
{
    return (uint8_t)(((uint16_t)v * (uint16_t)b + 128) >> 8);
}

// adjust brightness and encode to neopixel GRB
void encode_pixel_GRB(const uint8_t grb[3], uint8_t *dst9){
    uint8_t G=scale8(grb[0], g_brightness);
    uint8_t R=scale8(grb[1], g_brightness);
    uint8_t B=scale8(grb[2], g_brightness);
    dst9[0]=ENC[G][0]; dst9[1]=ENC[G][1]; dst9[2]=ENC[G][2];
    dst9[3]=ENC[R][0]; dst9[4]=ENC[R][1]; dst9[5]=ENC[R][2];
    dst9[6]=ENC[B][0]; dst9[7]=ENC[B][1]; dst9[8]=ENC[B][2];
}

// positioning type stuff
int map_xy_to_index(int x, int y)
{
    // flip coordinates based on start corner
    int ry = TOP_TO_BOTTOM ? y : (H - 1 - y);
    int rx;

#if SERPENTINE
    // on serpentine panels, every other row reverses
    if (STARTS_LEFT) {
        // even rows L->R, odd rows R->L
        rx = (ry & 1) ? (W - 1 - x) : x;
    } else {
        // even rows R->L, odd rows L->R
        rx = (ry & 1) ? x : (W - 1 - x);
    }
#else
    // straight wiring (every row same direction)
    rx = STARTS_LEFT ? x : (W - 1 - x);
#endif

    return ry * W + rx;
}

void build_frame_from_image(void) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int strip_i = map_xy_to_index(x, y);
            encode_pixel_GRB(image_grb[y*W + x], &frame_encoded[strip_i * 9]);
        }
    }
}

// copy an RGB image (R,G,B order) into image_grb[]
// to-do: maybe get rid of src stuff to simplify, we simply will not give it the wrong size image
void load_image_rgb(const uint8_t *src, int srcW, int srcH)
{
    // compute top-left where the source maps on our panel (centered)
    int x0 = (W - srcW) / 2;
    int y0 = (H - srcH) / 2;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            uint8_t r = 0, g = 0, b = 0;

            // if (x,y) falls inside the source image region, sample it, if not, black
            int sx = x - x0;
            int sy = y - y0;
            if ((unsigned)sx < (unsigned)srcW && (unsigned)sy < (unsigned)srcH) {
                int sidx = (sy * srcW + sx) * 3;
                r = src[sidx + 0];
                g = src[sidx + 1];
                b = src[sidx + 2];
            }

            // store in GRB order for neopixel
            int i = y * W + x;
            image_grb[i][0] = g;
            image_grb[i][1] = r;
            image_grb[i][2] = b;
        }
    }
}

void send_frame(uint8_t *buf, int len) {
    transmissionComplete = 0;
    idx = 1;
    repeat = 0;
    txPacket = buf;
    message_len = len;
    NVIC_ClearPendingIRQ(SPI_0_INST_INT_IRQN);
    NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);
    DL_SPI_transmitData8(SPI_0_INST, txPacket[0]); // first byte
    while (!transmissionComplete) __WFI();

    // delay before sending again
    delay_cycles(400000);
}
