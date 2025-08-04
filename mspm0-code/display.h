#include "ti_msp_dl_config.h"

#ifndef display_h
#define display_h
#endif

// grid config
#define W 16
#define H 16
#define N_PIXELS (W*H)
// started writing code before actually getting the grid and seeing what type it is so I gave myself ~options~
#define SERPENTINE 1  // 1 = serpentine wiring, 0 = straight
// wanna display this bad boy in a frame and might have to change the orientation to make wires nicer so we have ~options~ here as well
#define STARTS_LEFT    0   // 1 = first LED is on the left edge of the first row; 0 = right edge
#define TOP_TO_BOTTOM  1   // 1 = first row is the top row; 0 = first row is the bottom row

extern uint8_t *txPacket;
extern volatile int transmissionComplete;
extern volatile int idx;
extern volatile int repeat;

extern volatile int n_repeats;                 // how many times to send each frame
extern int message_len;                        // should be N_PIXELS * 9

void build_encode_lut(void);
void build_frame_from_image(void);
void load_image_rgb(const uint8_t *src, int srcW, int srcH);
void send_frame(uint8_t *buf, int len);
