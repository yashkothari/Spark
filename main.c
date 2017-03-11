#include <stdint.h>
#include <stdbool.h>
#include "msp432.h"

int main(void) {
    //setup ISRs
    //setup timers
    TIMER_A0->CTL +  =
    //any other setup



    while(1) {
        //poll for key presses
    }
}

void run_charge_discharge_sequence() {
    //timer0 for time signal is high
    //timer1 for dead time

    //if first iteration
        //set charge and discharge signals low
        //elapse timer0
        //elapse timer1

    //set charge signal high
    //elapse timer 0
    //set charge signal low
    //elapse timer 1
    //set discharge signal high
    //elapse timer0
    //set discharge signal low
    //elapse timer1
}
