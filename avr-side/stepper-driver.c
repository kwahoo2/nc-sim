#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
//#include <stdlib.h>

//set up the debugging utility ASSERT
#define __ASSERT_USE_STDERR
#include <assert.h>
#include "uart.h"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/********************************************************************************
Macros and Defines
********************************************************************************/

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

#define BUFSIZE 250

// UART file descriptor
// putchar and getchar are in uart.c
FILE uart_str = FDEV_SETUP_STREAM(usart_putchar, usart_getchar, _FDEV_SETUP_RW);

//volatile uint8_t steps[4] =  {0x09, 0x0C, 0x06, 0x03}; //4 full steps
volatile uint8_t steps[8] =  {0x09, 0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01}; //8 half steps

volatile uint8_t stepbuf[BUFSIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile uint8_t ovf = 0; //there was a buffer overflow?
  
volatile uint8_t iX = 0;
volatile uint8_t iY = 0;
volatile uint8_t iZ = 0;

volatile uint8_t countTim = 0;
volatile uint8_t loopsTim = 16; //16 000 000/128/96/100/16 =>0.8mm/s (96 halfsteps) M6x1
volatile uint8_t timOvf = 0;

volatile uint8_t powerUp = 0b11000000; //if 0b0_ motor enabled, 0b_0 stepper motors power enabled 
volatile uint8_t waitForCmdData = 0; //wait for next byte after command byte 
volatile uint8_t lastCommand = 0;

void timerInit(void)
{
    //TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // Set 8 bit timer2 with prescaler 1024
    TCCR2B |= _BV(CS22) | _BV(CS20); // Set 8 bit timer2 with prescaler 128
    TCCR2A |= _BV(WGM21); // set CTC mode
    OCR2A = 100; // CTC mode aimed value
    TIMSK2 |= _BV(OCIE2A); // allow interrupt CTC
}

ISR(TIMER2_COMPA_vect)
{
    if (countTim == 255) 
    {
        timOvf = 1;
    }
    countTim++;    
}

ISR(USART_RX_vect)
{
    uint8_t received = usart_getchar();
    stepbuf[head] = received;
    head++;
    if (head >= BUFSIZE)
    {
        head = 0;
        ovf = 1;
    }
}

void decodeStep(uint8_t step)
{
    if (step & 0b00000001)
    {
        iX++;
        if (iX > 7) iX = 0;
    }
    if (step & 0b00001000)
    {
        if (iX > 0) 
        {
            iX--;
        }
        else
        {
            iX = 7;
        }
    }
    
    if (step & 0b00000010)
    {
        iY++;
        if (iY > 7) iY = 0;
    }
    if (step & 0b00010000)
    {
        if (iY > 0) 
        {
            iY--;
        }
        else
        {
            iY = 7;
        }
    }

    if (step & 0b00000100)
    {
        iZ++;
        if (iZ > 7) iZ = 0;
    }
    if (step & 0b00100000)
    {
        if (iZ > 0)
        {
            iZ--;
        }
        else
        {
            iZ = 7;
        }
    }
    PORTB = steps[iX];
    PORTC = steps[iY];
    PORTD = (steps[iZ] << 2) | powerUp;
}

void decodeCommand(uint8_t command)
{
    if (waitForCmdData == 1)
    {
        if (lastCommand == 0b11000000)
        {
            loopsTim = command; //values 0-255, higher is slower
        }
        waitForCmdData = 0;
        lastCommand = 0;
    }
    else
    {
        if (command == 0b11000000) //0b11 reserved for velocity setup
        {   
            waitForCmdData = 1; //value will be send in next byte
            lastCommand = command;
        }
        //if ((command & 0b10100000) == 0b10100000) //reserved for power modes setup
        if ((command & 0b11111100) == 0b10100000) //power modes setup, more explicit if condition
        {

            powerUp = (command & 0b00000011) << 6; //port 6 and 7
            PORTD = (steps[iZ] << 2) | powerUp;
        }
    }
}

int main(void)
{  
    
  DDRB = 0x0F;
  DDRC = 0x0F;
  DDRD = 0xFC; //PD0 and PD1 are uart
  PORTD = powerUp;
    
  //init the UART -- uart_init() is in uart.c
  usart_init ( MYUBRR );
  timerInit();
  
  sei();
  while(1)
  {
    while ((head > tail && ovf == 0) || (head < tail && ovf == 1))
    {
        uint8_t step = stepbuf[tail];
        if ((!(step & 0b10000000)) && (!waitForCmdData))
        {
            if ((countTim >= loopsTim) || (timOvf == 1))
            {
                decodeStep(step);
                countTim = 0;
                timOvf = 0;
                tail++;
                usart_putchar(step);
            }
        }
        else
        {
            decodeCommand(step);
            tail++;
            step = 0b01111111; //const echo for commands
            usart_putchar(step);
        }
        
        if (tail >= BUFSIZE)
        {
            tail = 0;
            ovf = 0;
        }
    }
  }
} 




