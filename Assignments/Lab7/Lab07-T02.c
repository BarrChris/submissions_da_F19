#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "driverlib/rom.h"

uint32_t ui32ADC0Value[4];
uint32_t ui32Status;
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF, ui32TempValueF2;

int main(void)
{

    //General settings
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); //changed the SYSCTL_SYSDIV from 4 to 5
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //temp gpio

    //Initialize GPIO and UART stuff
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //added GPIO_PIN's 1 and 3 for task1
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntMasterEnable();//master interrupt enable API for all interrupts
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //ADC settings
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 32);
    ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC0_BASE, 1);
    //ROM_ADCIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RX);
    //ROM_ADCIntEnable(ADC0_BASE, 1);

    while (1); //main loop
}

void UARTIntHandler(void)
{
    ui32Status = UARTIntStatus(UART0_BASE, true); //interrupt status
    char newchar; //character value for cmd
    UARTIntClear(UART0_BASE, ui32Status);

    while(UARTCharsAvail(UART0_BASE)) //checks which character in terminal
    {
        newchar = UARTCharGet(UART0_BASE); //set newchar equal to the character typed by the user
        UARTCharPut(UART0_BASE, newchar); //echo the character typed by the user

        if(newchar == 'R')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02); //red LED on
        }
        if(newchar == 'r')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00); //red LED off

        if(newchar == 'G')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08); //Green LED on
        }
        if(newchar == 'g')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //Green LED off
		
		if(newchar == 'B')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //turn LED off to switch colors
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //Blue LED on
        }
        if(newchar == 'b')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00); //blue LED off

        if(newchar == 'T')
        {
            ROM_ADCIntClear(ADC0_BASE, 1);
            ROM_ADCProcessorTrigger(ADC0_BASE, 1);

            while(!ROM_ADCIntStatus(ADC0_BASE, 1, false));

            ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

            UARTCharPut(UART0_BASE, ':');
            UARTCharPut(UART0_BASE, ' ');

            //ADC Calculations
            ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
            ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
            ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

            //Convert ADC values to char variables to print to terminal
            ui32TempValueF2 = ui32TempValueF;
            volatile uint32_t ones, tens;
            ones = (ui32TempValueF2%10) + '0';
            tens = ui32TempValueF2/10 + '0';
            UARTCharPut(UART0_BASE, tens);
            UARTCharPut(UART0_BASE, ones);

            UARTCharPut(UART0_BASE, 'F'); //print F for fahrenheit
            UARTCharPut(UART0_BASE, '\n'); //print new line
            UARTCharPut(UART0_BASE, '\r'); //return on new line
        }
    }
}
