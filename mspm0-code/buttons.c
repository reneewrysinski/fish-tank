#include <ti/devices/msp/msp.h>

#include "lab6_helper.h"
#include "buttons.h"

int button_on(uint32_t SW){
    return !((GPIOA->DIN31_0 & SW) == SW);
}

int any_button_on(){
    return (button_on(SW1) | button_on(SW2) | button_on(SW3) | button_on(SW4));
}
