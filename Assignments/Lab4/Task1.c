#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

uint32_t ui32PeriodHigh, ui32PeriodLow;

int main(void)
{
    //uint32_t ui32PeriodHigh, ui32PeriodLow;

    //Clock Setup
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //GPIO Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Used to test the output for waveform analysis
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0); //Used to test the output for waveform analysis

    //Timer Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    //Calculate Delay
    ui32PeriodHigh = 0.43*(SysCtlClockGet()/1)/10;
    ui32PeriodLow = 0.57*(SysCtlClockGet()/1)/10;
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
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0); //Used to test the output for waveform analysis
    }

    else
    {
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32PeriodHigh); //loads in high
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 1); //Used to test the output for waveform analysis
    }
}

