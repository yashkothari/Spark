#include <stdint.h>
#include <stdbool.h>
#include "msp432.h"

void updown_run_charge_discharge_sequence() {
    //using updown timer
    //TAxCCR1 for charge
    //TAxCCR2 for discharge
    //tdead = ttimer * (TAxCCR1 - TAxCCR2)

    TIMER_A0->CTL &= ~BIT2;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UPDOWN;
            // bits15-10=XXXXXX, reserved
            // bits9-8=10,       clock source to SMCLK
            // bits7-6=00,       input clock divider /1
            // bits5-4=00,       up/down mode
            // bit3=X,           reserved
            // bit2=0,           set this bit to clear
            // bit1=0,           interrupt disable
            // bit0=0,           clear interrupt pending
}


void main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;  /* Stop watchdog timer */

    //GPIO setup
    P2->SEL0 |= 0x30;
    P2->SEL1 &= 0x0;
    P2->DIR |= 0x30;

    //init timer
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__1 |
                    TIMER_A_CTL_MC__STOP | TIMER_A_CTL_CLR;
            // bits15-10=XXXXXX, reserved
            // bits9-8=10,       clock source to SMCLK
            // bits7-6=00,       input clock divider /1
            // bits5-4=00,       stop mode
            // bit3=X,           reserved
            // bit2=1,           set this bit to clear
            // bit1=0,           interrupt disable
            // bit0=0,           clear interrupt pending

    //charge signal
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CM__NONE | TIMER_A_CCTLN_OUTMOD_2;// | TIMER_A_CCTLN_CCIE;
            // bits15-14=00,     no capture
            // bits13-12=XX,     don't care
            // bit11=X,          don't care
            // bit10=X,          don't care
            // bit9=X,           reserved
            // bit8=0,           compare mode
            // bits7-5=110,      toggle/set mode
            // bit4=0,           interrupt enable
            // bit3=X,           read-only
            // bit2=X,           don't care
            // bit1=0,           clear capture overflow
            // bit0=0,           clear interrupt pending

    //discharge signal
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM__NONE | TIMER_A_CCTLN_OUTMOD_6;// | TIMER_A_CCTLN_CCIE;
            // bits15-14=00,     no capture
            // bits13-12=XX,     don't care
            // bit11=X,          don't care
            // bit10=X,          don't care
            // bit9=X,           reserved
            // bit8=0,           compare mode
            // bits7-5=010,      toggle/reset mode
            // bit4=0,           interrupt enable
            // bit3=X,           reserved
            // bit2=X,           don't care
            // bit1=0,           clear capture overflow
            // bit0=0,           clear interrupt pending

    TIMER_A0->CCR[0] = 0xFFFF;
    TIMER_A0->CCR[1] = 0x5555;
    TIMER_A0->CCR[2] = 0xAAAA;

    updown_run_charge_discharge_sequence();

    while(1) { }
}

void yk_run_charge_discharge_sequence() {
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

