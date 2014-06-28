#define F_CPU 1000000UL  // 1 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>
#include "rs232-lib.c"
#include "i2c_lib.c"
#include "constants.h"

void init(void);
void work(void);
void do_i2c(void);
void do_kort(void);
void delay_s(uint8_t s);
void open_door(void);
void close_door(void);
uint8_t check_magdoor(void);

int main(void){
	init();
	while(1){
		work();
	}
}

void init(void){
	i2c_init(DOOR_I2C_ADR);
	rs232_init(RS232_SETTING_RXTX);
	DDRB = 0xFF;
	DDRD = (1 << 7);
	PORTD = 0x00;
	PORTB = 0x00;
	
	sei();
}

void work(void){

	uint8_t rs232 , i2c, door_status;
	rs232 = i2c = 0;

	while(!((rs232 = rs232_new_data())  && (i2c = i2c_has_data()))){
		asm("nop");
		
		if((door_status = check_magdoor())){
			i2c_set_target_adr(LOCAL_I2C_ADR);
			i2c_send_byte(door_status);
		}
	}

	if(i2c){
		do_i2c();
	}

}

void do_i2c(void){
	uint8_t data = i2c_get_byte();
	switch(data){
		case(OPEN_DOOR_COMMAND):
			open_door();
			break;
		case(CLOSE_DOOR_COMMAND):
			close_door();
			break;
		default:
			break;
	}
	rs232_send(data);
}

void open_door(void){
	PORTB |= (1 << 0);
}

void close_door(void){
	PORTB &= ~(1 << 0);
}

void delay_s(uint8_t s){
	
	uint8_t i;
	uint8_t j;
	for(j=0; j < s; j++){
		for(i=0; i < 4; i++){
			_delay_ms(250);
		}
	}
}

uint8_t check_magdoor(void){
	static uint8_t last = 0;
	static uint8_t count = 0;
	if((last ^ PORTD) & (1<<7)){
		if(count++ == 10){
			last = PORTD;
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
