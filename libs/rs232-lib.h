/**
 * Authors:
 *   Thomas Axelsson (thoax133@student.liu.se)
 *   Gustav Häger (gusha124@student.liu.se)
 * Created date: 2009-10-28
 * Last updated: 2009-11-23
 * Version: 1.0
 * Description: RS-232 library with send ring buffer.
 **/

#ifndef RS232_LIB_H
#define RS232_LIB_H

#include <stdint.h>
#include <avr/io.h>

/* Communication settings passed to init */
#define RS232_SETTING_RX (1 << RXEN)
#define RS232_SETTING_TX (1 << TXEN)
#define RS232_SETTING_RXTX (RS232_SETTING_RX | RS232_SETTING_TX)

/* Send buffer size should be a power of 2 so we can use a bit mask to wrap */
#define RS232_SEND_BUFFER_SIZE 16

/* Recieve buffer size should be a power of 2 so we can use a bit mask to wrap */
#define RS232_RECEIVE_BUFFER_SIZE 16


/* Initialize RS232. Enables the requested functions in settings.
Call this function with interrupts disabled */
void rs232_init(const uint8_t settings);
/* Add data to send buffer. Returns number of items in send buffer */
uint8_t rs232_send(const uint8_t data);
/* Clears the send buffer */
void rs232_clear_buffer(void);
/* Get the latest received data */
uint8_t rs232_get(void);
/* Get an entire line from the receive buffer*/
uint8_t rs232_get_line(uint8_t* dataBuffer, uint8_t length);

/* Returns a non-zero value if new data has arrived */
uint8_t rs232_new_data(void);
/* sends an entire string at once */
void rs232_send_string(uint8_t *string);
#endif
