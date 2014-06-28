#define F_CPU 1000000UL  // 1 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>
#include "rs232-lib.c"
#include "i2c_lib.c"
#include "constants.h"

typedef enum{RS_IDLE=0,RS_WAIT_ADR=1,RS_WAIT_DATA=2,RS_ECHO=3} rs232_state_t;
typedef enum{I2C_IDLE=0} i2c_rcv_state_t;

uint8_t door_status = TELL_DOOR_CLOSED;
uint8_t i2c_rcv_state = I2C_IDLE;
uint8_t rs232_state = RS_IDLE;
uint8_t i2c_send_buffer[255];
uint8_t i2c_send_length;


void init(void);
void work(void);
void do_i2c(void);
void do_rs232(void);
uint8_t check_magdoor(void);

void rs232_idle(uint8_t data);
void i2c_idle(uint8_t data);


int main(void){
	init();
	rs232_send('Q');
	while(1){
		
		work();
		
	}

}


void init(void){

	rs232_init(RS232_SETTING_RXTX);
	i2c_init(LOCAL_I2C_ADR);
	DDRB = 0xFF;
	DDRD = 0x00;
	PORTD = (1<<7);
	PORTB = 0x00;
	rs232_state = RS_IDLE;
	
	sei();
}


void work(void){
    

	uint8_t rs232 , i2c, tempdoor_status;
	rs232 = i2c = 0;
	
	//while(!((rs232 = rs232_new_data())  || (i2c = i2c_has_data()))){
	while(!(rs232 = rs232_new_data())){
		if(tempdoor_status = check_magdoor())
			door_status = tempdoor_status;
			
		
	}

	if(rs232){
		do_rs232();
	}

	/*if(i2c){
		do_i2c();
	}*/

}

void do_rs232(void){
	
	uint8_t data;
	data = rs232_get();
	
	switch(rs232_state){
		case(RS_IDLE):
			rs232_idle(data);		
		break;
		case(RS_WAIT_ADR):
			i2c_set_target_adr(data);
			i2c_send_length = 0;
			rs232_state = RS_WAIT_DATA;
		break;	
		case(RS_WAIT_DATA):
			if(data == 0)
			{
				i2c_send_bytes(i2c_send_buffer,i2c_send_length);		
			}
			else
			{
				i2c_send_buffer[i2c_send_length++] = data;
			}
		break;
		case(RS_ECHO):
			rs232_send(data);
			if(data == 0){
				rs232_state = RS_IDLE;
			}
		break;
		default:
			rs232_clear_buffer();
		break;
	}
}

void do_i2c(void){
	uint8_t data;
	data = i2c_get_byte();

	switch(i2c_rcv_state){
		case(I2C_IDLE):
			i2c_idle(data);
		break;
		case(TELL_DOOR_CLOSE_COMMAND):
			rs232_send(TELL_DOOR_CLOSED);
		break;
		case(TELL_DOOR_OPEN_COMMAND):
			rs232_send(TELL_DOOR_OPEN_COMMAND);
		default:
		break;
	}
}

void rs232_idle(uint8_t data){
	switch(data){
		/*case SEND_TO_I2C:
			rs232_state = RS_WAIT_ADR;
		break;
		case ECHO:
			rs232_state = RS_ECHO;
		break;*/
		case OPEN_DOOR:
		//	i2c_set_target_adr(DOOR_I2C_ADR);
		//	i2c_send_byte(OPEN_DOOR_COMMAND);
		open_door();
		break;
		case CLOSE_DOOR:
		//	i2c_set_target_adr(DOOR_I2C_ADR);
		//	i2c_send_byte(CLOSE_DOOR_COMMAND);
		close_door();
		break;
		case ASK_DOOR_STATUS:
		//	i2c_set_target_adr(DOOR_I2C_ADR);
		//	i2c_send_byte(ASK_DOOR_STATUS_COMMAND);

		if(door_status!=0)
			rs232_send(door_status);
		
		break;
		default:
			break;
			//error
			
	}
}

void i2c_idle(uint8_t data){
	switch(data){
		case SEND_TO_COMPUTER:
			while(i2c_has_data()){
				rs232_send(i2c_get_byte());
			}
		break;
		default:
			i2c_clear_rcv_buffer();
		break;
	}
}


void open_door(void){
	PORTB |= (1 << 0);
}

void close_door(void){
	PORTB &= ~(1 << 0);
}

uint8_t check_magdoor(void){
	static uint8_t last = 0;
	static uint8_t count = 0;
	if((last ^ PIND) & (1<<7)){
		if(count++ == 10){
			last = PIND;
			if(last & (1<<7)){
				count = 0;
				return TELL_DOOR_OPEN;	
			}else{
				count = 0;
				return TELL_DOOR_CLOSED;
			}
		}
	}else{
		count = 0;
	}
	return 0;
}
