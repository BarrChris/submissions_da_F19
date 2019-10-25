//Part A
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8PinData=4;
uint8_t RGB_BGR=0;
uint8_t count=0;

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
        SysCtlDelay(8000000);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        SysCtlDelay(8000000);

        if (count < 2)
        {
            //red = 2, blue = 4, green = 8
            //this will do blue, green, red; 4, 8, 2
            if(ui8PinData==4)
                ui8PinData=8; //green
            else if (ui8PinData==8)
                ui8PinData=2; //red
            count++;
        }

        else if (count > 1)
        {
            //this will do red, green, blue; 2, 8, 4
            if(count == 2)
                ui8PinData=2; //red
            else if (count == 3)
                ui8PinData=8; //green
            else
                ui8PinData=4; //blue

            count++;

            if (count > 5)
            {
                count = 0;
            }
        }
    }
}