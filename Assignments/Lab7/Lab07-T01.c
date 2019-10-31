#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
//includes/defines from lab5 for temperature analysis
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"

//Global variables to track temperature gauges
uint32_t ui32ADC0Value[4];
uint32_t ui32Period;
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF, ui32TempValueF2;

int main(void)
{
    //General settings
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); //changed the SYSCTL_SYSDIV from 4 to 5

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //Initialize GPIO
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //added GPIO_PIN's 1 and 3 for task1
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //GPIO settings
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //-------------------------------------------------------------------------------------
    // Included initializations for Task1
    //-------------------------------------------------------------------------------------
    //ADC settings
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 32);
    ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC0_BASE, 1);

    //Timer 1 settings
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); //enable peripheral timer1
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); //configure timer1 to periodic mode
    ui32Period = (SysCtlClockGet()/2);
    IntEnable(INT_TIMER1A);
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);//enables a specific event within the timer to generate an interrupt
    IntMasterEnable();//master interrupt enable API for all interrupts
    TimerEnable(TIMER1_BASE, TIMER_A);//enable the timer
    //-------------------------------------------------------------------------------------

    while (1); //main loop

}

void Timer1IntHandler(void)
{
    //Clears the timer1 interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    ADCIntClear(ADC0_BASE, 1);  //clears ADC
    ADCProcessorTrigger(ADC0_BASE, 1);
    while(!ADCIntStatus(ADC0_BASE, 1, false));
    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

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

    //Labels the printed ADC values as fahrenheit (F)
    UARTCharPut(UART0_BASE, 'F');
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
}
