#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"

#ifndef M_PI
#define M_PI                    3.14159265358979323846 //defines PI's float variable
#endif

#define SERIES_LENGTH 1000 //for our 1000 data point long array

float gSeriesData[SERIES_LENGTH]; //contains our 1000 data point values

int32_t i32DataCount = 0; //counter variable as a limiter

void InitUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); //enables UART module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //enables GPIO port a

    GPIOPinConfigure(GPIO_PA0_U0RX); //PA0 as RX pin
    GPIOPinConfigure(GPIO_PA1_U0TX); //PA1 as TX pin
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //sets UART pin type

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); //sets the clock source
    UARTStdioConfig(0, 115200, 16000000); //disables uartstdio, sets baud rate to 115200, uses clock
}

int main(void)
{
    float fRadians_sin, fRadians_cos; //float radians variable

    ROM_FPULazyStackingEnable(); //Enables the lazy stacking of floating-point registers. If a floating-point
                                 //instruction is executed from within the interrupt context, the floating-point context
                                 //is first saved into the space reserved on the stack.
                                 //On completion of the interrupt handler, the floating-point context is only restored
                                 //if it was saved (as the result of executing a floating-point instruction).

    ROM_FPUEnable(); //Enables the floating-point unit.
                     //This function enables the floating-point unit, allowing the floating-point
                     //instructions to be executed.

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //Sets system clock at 50MHz

    fRadians_sin = ((2 * M_PI * 50) / SERIES_LENGTH); //Calculates for the variable to hold the value 2pi/depth of data buffer
    fRadians_cos = ((2 * M_PI * 200) / SERIES_LENGTH); //Calculates for the variable to hold the value 2pi/depth of data buffer

    InitUART();

    while(i32DataCount < SERIES_LENGTH)  //Calculates the sine wave values for each of the 1000 values
    {
        //Calculates the sine wave values
        //sin(2pi*50t) + 0.5cos(2pi*200t)
        gSeriesData[i32DataCount] = (1.0 * sinf(fRadians_sin * i32DataCount)) + (0.5 * cosf(fRadians_cos * i32DataCount)); //given equation

        //print equation to terminal
        UARTprintf("gSeriesData[%d] = sin(2PI * 50 * %d) + 0.5cos(2PI * 200 * %d)\n",
                   i32DataCount, i32DataCount, i32DataCount);

        //Continues to count till it reaches the 100th value
        i32DataCount++;

        SysCtlDelay(10000000);
    }

    while(1)
    {
        //main loop
    }
}
