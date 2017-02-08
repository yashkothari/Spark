/* DriverLib Includes */
#include </Users/janujanselva/workspace_v7/msp432_driverlib/driverlib/MSP432P4xx/driverlib.h>

/* Standard Includes */
#include <stdint.h>

#include <stdbool.h>

//Defines, changed later in main loop
#define TIMER_PERIOD (65535)
#define CHARGE_DUTY_CYCLE (TIMER_PERIOD >> 1)
#define DEAD_TIME 65000


#define DCHARGE_DUTY_CYCLE 26214
#define HIGH_CLK 128000
#define MED_CLK 32000
#define LOW_CLK 10000


/* Timer_A UpDown Configuration Parameter */
// This configuration is for charge signal
Timer_A_UpDownModeConfig upDownConfigA1 =
{
         TIMER_A_CLOCKSOURCE_SMCLK,             // SMCLK Clock SOurce = set to 128 KHz, 32 kHz or 10 kHz
        TIMER_A_CLOCKSOURCE_DIVIDER_2,          // SMCLK/1
        TIMER_PERIOD,                           // 65535 tick period (MAX for 16 bit timer)
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value

};


/* Timer_A Compare Configuration Parameter  (PWM1) */
Timer_A_CompareModeConfig compareConfig_PWM1 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        CHARGE_DUTY_CYCLE                           // 50% Duty Cycle
};

const Timer_A_UpModeConfig upConfig_deadTime =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_2,         // SMCLK/1 = 128 kHz
        DEAD_TIME,                              // 65535/2 + 6553 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};



//Second configuration is for discharge
Timer_A_UpDownModeConfig upDownConfigA2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce = set to 128 KHz, 32 kHz or 10 kHz
        TIMER_A_CLOCKSOURCE_DIVIDER_2,          // SMCLK/1
        TIMER_PERIOD,                           // 127 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value

};

Timer_A_CompareModeConfig compareConfig_PWM2 =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        39321                          // 40% Duty Cycle
};


//prototype of function
void configurePWMTimer( uint32_t timer_module ,  Timer_A_UpDownModeConfig updown_config, Timer_A_CompareModeConfig ccr);

/* TODO: Find out which GPIO pin to enable to allow TA2 to output as well
 */

//#######################################################################################################################################################
uint32_t frequency = 10000; // set initial frequency to 10 kHz
void setupCS();
int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    setupCS();

    /* Configuring P1.0 as output and P1.1 (switch) as input */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Setting P7.7 and P7.6 and peripheral outputs for CCR */
    //P7.7/PM_TA1.1/C0.2
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,
            GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Setting P5.6 -> CCR1, P5.7 -> CCR2
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,
                GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P10,
            GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);


    /* Configuring Timer_A1 for Up Mode */
    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig_deadTime);

    /* Enabling interrupts and starting the timer */
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA2_0);



    //Start Timer A module 1 for charge cycle
    MAP_Timer_A_configureUpDownMode(TIMER_A1_BASE, &upDownConfigA1);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UPDOWN_MODE);
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_PWM1);
    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    //Start Dead Time timer
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    //Start discharge timer
    MAP_Timer_A_configureUpDownMode(TIMER_A3_BASE, &upDownConfigA2);





    //configurePWMTimer(TIMER_A0_BASE, upDownConfigA1, compareConfig_PWM1);

    /* Sleeping when not in use */
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
}


//Clock system setup
void setupCS()
{

    //Setting SMCLK and MCLK
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK,  CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK,  CS_REFOCLK_SELECT,  CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

}


//Timer A1 Interrupt handler (for dead time)
//Start discharge timer
void  TA2_0_IRQHandler(void)
{
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    //MAP_Timer_A_disableInterrupt(TIMER_A2_BASE);
    MAP_Timer_A_disableCaptureCompareInterrupt ( TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    //MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    //uint_fast16_t count = MAP_Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0 );
    MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UPDOWN_MODE);
    MAP_Timer_A_initCompare(TIMER_A3_BASE, &compareConfig_PWM2);




}

void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1,  status);
    if (status & GPIO_PIN1)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    }
}

