#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

void timer1A_delaySec(int);

uint32_t ui32PeriodHigh, ui32PeriodLow;

int main(void)
{
    //uint32_t ui32PeriodHigh, ui32PeriodLow;
    uint32_t ui32PeriodDelay;

    //Clock Setup
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //GPIO Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Used to test the output for waveform analysis
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //-Switch -------------------
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOF);
    //--------------------------
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0); //Used to test the output for waveform analysis
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1); //Used to test the output for waveform analysis

    //Timer Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    //Calculate Delay
    ui32PeriodHigh = 0.43*(SysCtlClockGet()/1)/10;
    ui32PeriodLow = 0.57*(SysCtlClockGet()/1)/10;
    //ui32PeriodDelay = SysCtlClockGet(); //1 second delay
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32PeriodHigh -1);

    //Interrupt Enable
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    //Timer Enable
    TimerEnable(TIMER0_BASE, TIMER_A);

    //Main Loop
    while(1)
    {
        //
    }
}

void Timer0IntHandler(void)
{
    //Clear the timer Interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of the GPIO pin and
    // write back the opposite state

    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32PeriodLow); //loads in low
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
       // GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0); //Used to test the output for waveform analysis
    }

    else
    {
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32PeriodHigh); //loads in high
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
       // GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1); //Used to test the output for waveform analysis
    }
}

void timer1A_delaySec(int ttime)
// Used for delay using timer 1
{
    int i;

    SYSCTL_RCGCTIMER_R |= 2;
    TIMER1_CTL_R = 0;
    TIMER1_CFG_R = 0x04;
    TIMER1_TAMR_R = 0x02;
    TIMER1_TAILR_R = 64000 - 1;
    TIMER1_TAPR_R = 250 - 1;
    TIMER1_ICR_R = 0x1;
    TIMER1_CTL_R |= 0x01;
    for(i = 0; i < ttime; i++)
    {
        while ((TIMER1_RIS_R & 0x1) == 0);
        TIMER1_ICR_R = 0x1;
    }
}

void SW1Int(void)
// Once button (switch 1) is pressed, it will check to see if it's pressed, if true then it will keep the blue LED on for 1 second
{
    int pressed = 0;

    pressed = GPIOIntStatus(GPIO_PORTF_BASE, true);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

    if (status & GPIO_INT_PIN_4)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1); //Used to test the output for waveform analysis
        timer1A_delaySec(2);
    }
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0); //Used to test the output for waveform analysis
    SysCtlDelay(1000000);
}
