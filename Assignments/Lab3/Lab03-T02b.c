//Part B
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8PinData=4;
uint8_t RGB_BGR=0;
uint8_t count=0;

//function prototype(s)
void blink(uint8_t);
void delay(void);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    //red = 2, blue = 4, green = 8
    while(1)
    {
        ui8PinData=2; //Red
        blink(ui8PinData);
        delay();

        ui8PinData=8; //Green
        blink(ui8PinData);
        delay();

        ui8PinData=4; //Blue
        blink(ui8PinData);
        delay();

        ui8PinData=10; //Yellow
        blink(ui8PinData);
        delay();

        ui8PinData=6; //Purple
        blink(ui8PinData);
        delay();

        ui8PinData=12; //Cyan
        blink(ui8PinData);
        delay();

        ui8PinData=14; //White
        blink(ui8PinData);
        delay();
    }
}

void blink(uint8_t ui8PinData)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
    SysCtlDelay(8000000);
}

void delay(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    SysCtlDelay(8000000);
}