#ifndef effects_h
#define effects_h

extern int transmissionComplete;
extern int timerTicked;
extern int idx;
extern uint16_t txPacket[];
extern int message_len;

void pause();
void set_lights(int L1, int L2, int L3, int L4);
void set_buzzer_freq(float freq);
void buzzer_off();
int wait(int steps);
int play_note(float note, int duration);
int wait2(int steps);

#endif
