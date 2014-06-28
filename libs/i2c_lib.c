#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "buffer.c"

#include "i2c_lib.h"

//define the i2c-pins
#define SDA 1
#define SCL 0

#include <util/delay.h>

volatile buffer i2c_data_buffer;
volatile buffer i2c_rcv_buffer;
volatile uint8_t i2c_active;

void i2c_clear_rcv_buffer(void){
	init_buffer(&i2c_rcv_buffer,0x00);
}
void i2c_init(uint8_t addr){
	i2c_active = 0;
	i2c_state = SLAVE;
	init_buffer(&i2c_rcv_buffer,0x00);
	TWAR = addr;
	TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
}

void i2c_set_target_adr(uint8_t adr){
	init_buffer(&i2c_data_buffer,adr);
}
void i2c_send_byte(uint8_t data){
	add_buffer(data,&i2c_data_buffer);
	
	if(i2c_state == SLAVE){
		i2c_state = MASTER;
		//transmit start condition
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	}
}

void i2c_send_bytes(uint8_t* data,uint8_t length){
	uint8_t i;

	for(i=0;i<length;i++){
		add_buffer(*(data++),&i2c_data_buffer);
	}

	if(i2c_state == SLAVE){
		i2c_state = MASTER;
		//transmit start condition
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	}
}

uint8_t i2c_has_data(void){
	return !buffer_empty(&i2c_rcv_buffer) && !i2c_active;
}

uint8_t i2c_get_byte(void){
	PORTB = PORTB + 1;
	return get_buffer(&i2c_rcv_buffer);
}

inline void master_i2c_interrupt(void){
	switch(TW_STATUS){
		case(TW_START):
			TWDR = i2c_data_buffer.adr; //send adr
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		break;
		case(TW_MT_SLA_ACK):
		case(TW_MT_DATA_ACK):			
			if(buffer_look_ahead(&i2c_data_buffer) == '\0'){
				TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (1 << TWIE);
				i2c_state = SLAVE;
			}else{
				TWDR = get_buffer(&i2c_data_buffer);
				TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
			}
		break;
		case(TW_REP_START):
			/*TWDR = i2c_data_buffer.adr;
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);*/
		case(TW_MT_DATA_NACK):
		case(TW_MT_SLA_NACK):
		break;
		default:
			
		break;
	}
}

inline void slave_i2c_interrupt(void){
	switch(TW_STATUS){
		case(TW_SR_SLA_ACK):
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		break;
		case(TW_SR_GCALL_ACK):
		break;
		case(TW_SR_ARB_LOST_GCALL_ACK):
		break;
		case(TW_SR_DATA_ACK):
			i2c_active = 1;
			add_buffer(TWDR,&i2c_rcv_buffer);
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT);
		break;
		case(TW_SR_DATA_NACK):
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT);
		break;
		case(TW_SR_GCALL_DATA_ACK):
		break;
		case(TW_SR_GCALL_DATA_NACK):
		break;
		case(TW_SR_STOP):
			i2c_active = 0;
			TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWIE) | (1 << TWEA);
		break;
		default:

		break;
	}
}

ISR(TWI_vect){
	switch(i2c_state){
		case MASTER:		
			master_i2c_interrupt();
		break;
		case SLAVE:
			slave_i2c_interrupt();
		break;
		case IDLE:
			i2c_state = SLAVE;
		break;
		default:
			//this is a problem
		break;
	}
}
