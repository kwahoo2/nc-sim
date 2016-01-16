// Test fixed-point multiply     
             
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

ISR(USART_RX_vect)
{
    uint8_t received = usart_getchar();
    stepbuf[head] = received;
    head++;
    if (head >= BUFSIZE)
    {
        head = 0;
        ovf = 1; //todo: message to pc
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
    PORTD = (steps[iZ] << 2);
    
    _delay_ms(10);
}

void decodeCommand(uint8_t command)
{
    
}

int main(void)
{  
    
  DDRB = 0x0F;
  DDRC = 0x0F;
  DDRD = 0x3C; //PD0 and PD1 are uart
    
  //init the UART -- uart_init() is in uart.c
  usart_init ( MYUBRR );
  
  sei();
  while(1)
  {
    while ((head > tail && ovf == 0) || (head < tail && ovf == 1))
    {
        uint8_t step = stepbuf[tail];
        if (!(step & 0b1000000))
        {
            decodeStep(step);
        }
        else
        {
            decodeCommand(step);
        }
        
        tail++;
        if (tail >= BUFSIZE)
        {
            tail = 0;
            ovf = 0;
        }
        usart_putchar(step);
    }
  }
} 




