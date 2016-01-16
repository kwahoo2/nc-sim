#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

/********************************************************************************
Function Prototypes
********************************************************************************/

void usart_init(uint16_t ubrr);
char usart_getchar( void );
void usart_putchar( char data );
void usart_pstr (char *s);
unsigned char usart_kbhit(void);