//libraries :)
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#ifndef M_PI
#define M_PI                    3.14159265358979323846 //defines PI's float variable
#endif

#define SERIES_LENGTH 100 //for our 100 data point long array

float gSeriesData[SERIES_LENGTH]; //contains our 100 data point values

int32_t i32DataCount = 0; //counter variable as a limiter

int main(void)
{
    float fRadians; //float radians variable

    ROM_FPULazyStackingEnable(); //Enables the lazy stacking of floating-point registers. If a floating-point
                                 //instruction is executed from within the interrupt context, the floating-point context
                                 //is first saved into the space reserved on the stack.
                                 //On completion of the interrupt handler, the floating-point context is only restored
                                 //if it was saved (as the result of executing a floating-point instruction).

    ROM_FPUEnable(); //Enables the floating-point unit.
                     //This function enables the floating-point unit, allowing the floating-point
                     //instructions to be executed.

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //Sets system clock at 50MHz

    fRadians = ((2 * M_PI) / SERIES_LENGTH); //Calculates for the variable to hold the value 2pi/depth of data buffer

    while(i32DataCount < SERIES_LENGTH)  //Calculates the sine wave values for each of the 100 values
    {
        //Calculates the sine wave values
        gSeriesData[i32DataCount] = sinf(fRadians * i32DataCount);

        //Continues to count till it reaches the 100th value
        i32DataCount++;
    }

    while(1)
    {
        //main loop
    }
}
