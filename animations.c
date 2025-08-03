#include "ti_msp_dl_config.h"
#include "animations.h"
#include "display.h"
#include "image_frames.h"
#include "effects.h"
#include "buttons.h"
#include "simon_random.h"
#include "notes.h"
#include "lab6_helper.h"
#include <stdio.h>

uint8_t frame_encoded[N_PIXELS * 9];

uint8_t get_num_frames(const uint8_t** array){
    uint8_t i = 0;
    while (array[i] != NULL) {
        i++;
    }
    return i;
}

// base function for showing basic animations since they all basically do the same (all except game)
void play(const uint8_t* frames[], uint32_t count,
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
void show_swim_fast(uint32_t loops){ play(FISH_SWIM_FRAMES,  get_num_frames(FISH_SWIM_FRAMES),  loops, 2000000); }
void show_feed(uint32_t loops){ play(FISH_FEED_FRAMES,  get_num_frames(FISH_FEED_FRAMES),  loops, 4000000); }
void show_sleep(uint32_t loops){ play(FISH_SLEEP_FRAMES, get_num_frames(FISH_SLEEP_FRAMES), loops, 4000000); }
void show_sleeping(uint32_t loops){ play(FISH_SLEEPING_FRAMES, get_num_frames(FISH_SLEEPING_FRAMES), loops, 4000000); }


int play_game(uint32_t long_delay_cycles, uint32_t short_delay_cycles) {
    // setup type stuff
    int seed = GenerateRandomNumber();
    srand(seed);
    // step 1
//    printf("step 1 started!\n");
    play(GAME_INTRO, get_num_frames(GAME_INTRO), 1, long_delay_cycles);
    // step 2
//    printf("step 2 started!\n");
    wait3(32767, FISH_GAME_1);
    // step 3
    printf("step 3 started!\n");
    int SW1_pressed = button_on(SW1);
    int SW2_pressed = button_on(SW2);
    int SW3_pressed = button_on(SW3);
    int SW4_pressed = button_on(SW4);
    uint8_t chosen_spot = 0;
    if (SW1_pressed) {
//        play_note(NOTE_G_4, 20);
        play(GAME_HIDE_R, get_num_frames(GAME_HIDE_R), 1, long_delay_cycles);
        chosen_spot = 0;
    }
    else if (SW2_pressed) {
//        play_note(NOTE_E_4, 20);
        play(GAME_HIDE_Y, get_num_frames(GAME_HIDE_Y), 1, long_delay_cycles);
        chosen_spot = 1;
    }
    else if (SW3_pressed) {
//        play_note(NOTE_C_4, 20);
        play(GAME_HIDE_P, get_num_frames(GAME_HIDE_P), 1, long_delay_cycles);
        chosen_spot = 2;
    }
    else if (SW4_pressed) {
//        play_note(NOTE_G_3, 20);
        play(GAME_HIDE_G, get_num_frames(GAME_HIDE_G), 1, long_delay_cycles);
        chosen_spot = 3;
    }
    // step 4
    printf("step 4 started!\n");
    uint8_t win_spot = rand();
    // step 5
    printf("step 5 started!\n");
    play(GAME_WAVE, get_num_frames(GAME_WAVE), 1, short_delay_cycles);
    // step 6
    printf("step 6 started!\n");
    if (win_spot == 0) {
        play(GAME_R_WIN, get_num_frames(GAME_R_WIN), 1, short_delay_cycles);
        delay_cycles(long_delay_cycles);
    }
    else if (win_spot == 1) {
        play(GAME_Y_WIN, get_num_frames(GAME_Y_WIN), 1, short_delay_cycles);
        delay_cycles(long_delay_cycles);
    }
    else if (win_spot == 2) {
        play(GAME_P_WIN, get_num_frames(GAME_P_WIN), 1, short_delay_cycles);
        delay_cycles(long_delay_cycles);
    }
    else if (win_spot == 3) {
        play(GAME_G_WIN, get_num_frames(GAME_G_WIN), 1, short_delay_cycles);
        delay_cycles(long_delay_cycles);
    }
    // step 7
    printf("step 7 started!\n");
    if (chosen_spot == win_spot) {
        // step 7a
        play(GAME_WIN, get_num_frames(GAME_WIN), 1, long_delay_cycles);
        // step 7b
//        play_note(NOTE_G_3, 10);
//        play_note(NOTE_C_4, 10);
//        play_note(NOTE_E_4, 10);
//        play_note(NOTE_G_4, 10);
//        play_note(NOTE_C_5, 10);
//        play_note(NOTE_E_5, 10);
//        play_note(NOTE_G_5, 30);
//        play_note(NOTE_E_5, 30);
//        play_note(NOTE_GS_3, 10);
//        play_note(NOTE_C_4, 10);
//        play_note(NOTE_DS_4, 10);
//        play_note(NOTE_GS_4, 10);
//        play_note(NOTE_C_5, 10);
//        play_note(NOTE_DS_5, 10);
//        play_note(NOTE_GS_5, 30);
//        play_note(NOTE_E_5, 30);
//        play_note(NOTE_AS_3, 10);
//        play_note(NOTE_D_4, 10);
//        play_note(NOTE_F_4, 10);
//        play_note(NOTE_AS_4, 10);
//        play_note(NOTE_D_5, 10);
//        play_note(NOTE_F_5, 10);
//        play_note(NOTE_AS_5, 30);
//        play_note(NOTE_AS_5, 10);
//        play_note(NOTE_AS_5, 10);
//        play_note(NOTE_AS_5, 10);
//        play_note(NOTE_C_6, 60);
        // step 8a
        wait3(32767, FISH_GAME_20);
    }
    else {
        // step 7a
        play(GAME_LOSE, get_num_frames(GAME_LOSE), 1, long_delay_cycles);
        // step 7b
//        play_note(NOTE_C_5, 15);
//        play_note(NOTE_A_4, 15);
//        play_note(NOTE_F_4, 15);
//        play_note(NOTE_A_4, 20);
//        play_note(NOTE_B_4, 20);
//        play_note(NOTE_A_4, 20);
//        play_note(NOTE_GS_4, 20);
//        play_note(NOTE_AS_4, 20);
//        play_note(NOTE_GS_4, 20);
//        play_note(NOTE_G_4, 60);
        // step 8a
        wait3(32767, FISH_GAME_22);
    }
    // step 8b
    SW1_pressed = button_on(SW1);
    SW2_pressed = button_on(SW2);
    SW3_pressed = button_on(SW3);
    SW4_pressed = button_on(SW4);
    if (SW1_pressed || SW2_pressed) { // either right button pressed: return 1 to keep playing
        return 1;
    }
    if (SW3_pressed || SW4_pressed) { // either left button pressed: return 1 to exit to home
        return 0;
    }

}
