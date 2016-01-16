//For Atmega 328p

#include "uart.h"

void usart_init( uint16_t ubrr) {

    // Set baud rate

    UBRR0H = (uint8_t)(ubrr>>8);
    UBRR0L = (uint8_t)ubrr;

    // Enable receiver and transmitter, rxcie interrupt

    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);

    // Set frame format: 8data, 1stop bit

    UCSR0C = (3<<UCSZ00);
}

void usart_putchar(char data) {

    // Wait for empty transmit buffer

    while ( !(UCSR0A & (_BV(UDRE0))) ); 

    // Start transmission

    UDR0 = data;
}
char usart_getchar(void) {

    // Wait for incoming data

    //while ( !(UCSR0A & (_BV(RXC0))) ); - not necessary when interrupt driven

    // Return the data

    return UDR0;
}

unsigned char usart_kbhit(void) {

    //return nonzero if char waiting polled version

    unsigned char b;
    b=0;
    if(UCSR0A & (1<<RXC0)) b=1;
    return b;
}

void usart_pstr(char *s) {

    // loop through entire string

    while (*s) {
        usart_putchar(*s);
        s++;
    }
}
