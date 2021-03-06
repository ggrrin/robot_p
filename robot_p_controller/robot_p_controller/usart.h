/*
 * uart.h
 *
 * Created: 19-May-17 10:23:28 AM
 *  Author: petrg
 */ 
#ifndef uart_h_
#define uart_h_

#ifndef F_CPU
# warning "F_CPU macro is not defined, setting up default 1MHz"
#define F_CPU 1000000LU
#endif

#ifndef USART_BAUDRATE 
#define USART_BAUDRATE 9600
# warning "USART_BAUDRATE macro is not defined, setting up default 9600 BAUD"
#endif

#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <string.h>

void usart_init(void)
{
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	//enable recv interrupt, transmission and reception
	UCSR0B |= (1 << RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
}

void usart_send_byte(uint8_t u8Data)
{
	//wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
}

uint8_t usart_receive_byte()
{
	// Wait for byte to be received
	while(!(UCSR0A&(1<<RXC0))){};
	// Return received data
	return UDR0;
}

void usart_send_buffer(uint8_t* buffer, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		usart_send_byte(buffer[i]);
	}
}

void usart_init_buffer(uint8_t* buffer, uint8_t size)
{
	for (int i = 0; i < size; i++)
	{
		buffer[i] = 0;
	}
}

#define USART_STR_BUFF_LEN 20

void usart_send_num_str(int32_t number)
{
	char buffer[USART_STR_BUFF_LEN];
	usart_init_buffer((uint8_t*)buffer, USART_STR_BUFF_LEN);
	char* num = itoa(number, buffer, 10);
	usart_send_buffer((uint8_t*)buffer, strlen(num));
}

void usart_write_line(void)
{
	usart_send_byte('\r');
	usart_send_byte('\n');
}

#endif