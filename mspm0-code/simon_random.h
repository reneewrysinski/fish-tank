#ifndef SIMON_RANDOM_H_
#define SIMON_RANDOM_H_

#include <stdint.h>

/* Random number generation */
// -------------------------------------
void srand(uint16_t seed);
uint16_t rand();
uint32_t GenerateRandomNumber();

#endif
