/**
 * Authors:
 *   Thomas Axelsson (thoax133@student.liu.se)
 *   Gustav Häger (gusha124@student.liu.se)
 * Created date: 2009-10-28
 * Last updated: 2009-11-23
 * Version: 1.0
 * Description: RS-232 library with send ring buffer.
 **/

#include "rs232-lib.h"
#include <avr/interrupt.h>

#define UBRR 12 /* 4800 bps */

/*

start   end
 |       |  = count
 v       v
 _______________
|x|x|x|x|_|_|_|_|

end points to next empty position
start points to beginning of the list/the first to send
count is the current number of items in the buffer

*/
volatile uint8_t send_buffer[RS232_SEND_BUFFER_SIZE];
uint8_t send_end = 0;
volatile uint8_t send_start = 0;
volatile uint8_t send_count = 0;

volatile uint8_t receive_buffer[RS232_RECEIVE_BUFFER_SIZE];
volatile uint8_t receive_end = 0;
volatile uint8_t receive_start = 0;
volatile uint8_t receive_count = 0;


void rs232_init(const uint8_t settings)
{
	/* Set baud rate */
	UBRRH = (UBRR >> 8);
	UBRRL = UBRR;
	/* Set up tx, rx */
	UCSRB = settings;
	
	UCSRB |= (1 << RXCIE); //enable receive interrupt
	
	//setup so we use 8-bit character size, even parity and two stop bits
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1) | (1 << UPM1) | (1 << USBS);
}

//send an entire string
void rs232_send_string(uint8_t *string){
  
  while(*string != '\n'){
    rs232_send(*string);
    string++;
  }
  rs232_send('\n');
}

//send a single character on the line
uint8_t rs232_send(const uint8_t data)
{
	/* Put data in buffer */
	send_buffer[send_end] = data;
	send_end = (send_end + 1) & (RS232_SEND_BUFFER_SIZE - 1);

	/* Turn off transmit interrupt to avoid errors */
	UCSRB &= ~(1 << UDRIE);
	/* Increment count unless buffer is full (oldest element is lost) */
	if(send_count != RS232_SEND_BUFFER_SIZE)
	{
		send_count++;
	}
	else
	{
		/* Drop the oldest item */
		send_start = (send_start + 1) & (RS232_SEND_BUFFER_SIZE - 1);
	}

	/* Enable interrupt so that the new data will be sent*/
	UCSRB |= (1 << UDRIE);

	return RS232_SEND_BUFFER_SIZE - send_count;
}

void rs232_clear_buffer()
{
	send_count = 0;
	send_start = send_end;
}

inline uint8_t rs232_get()
{
  uint8_t var = receive_buffer[receive_start];
	receive_start++;
	return var;
}

uint8_t rs232_get_line(uint8_t* dataBuffer, uint8_t length)
{
	//No error checking if the string isn't ended correctly
	uint8_t i;
	for(i=0;i<length; i++){
		while(!rs232_new_data()){
			asm("nop");
		}		
		*(dataBuffer++) = rs232_get();
		if(*dataBuffer == '\0' || *dataBuffer == '\n')
			return i+1;
	}
	return 0;
	
}

inline uint8_t rs232_new_data()
{
	return !(receive_start == receive_end);
}

//send complete interrupt (move data from send buffer to receive buffer)
ISR(USART_UDRE_vect){
	/* Get the next data to send and put it in the send register */
	UDR = send_buffer[send_start];

	send_start = (send_start + 1) & (RS232_SEND_BUFFER_SIZE - 1);

	/* Decrease count and disable interrupts if the ring buffer is empty */
	if(!(--send_count))
	{
		UCSRB &= ~(1 << UDRIE);
	}
}
//receive complete interrupt (put new data in receive buffer)
ISR(USART_RXC_vect){
  
  /* get the data */
  receive_buffer[receive_end] = UDR;
  receive_end = (receive_end + 1) & (RS232_RECEIVE_BUFFER_SIZE - 1);
  
  /* check for overflow */
  if(receive_count != RS232_RECEIVE_BUFFER_SIZE){
    receive_count++;
  }else{
    receive_start=(receive_start+1)&(RS232_RECEIVE_BUFFER_SIZE - 1);
  }
  
}

