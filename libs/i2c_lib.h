#ifndef I2C_LIB_C
#define I2C_LIB_C

typedef enum {IDLE, MASTER, SLAVE} i2c_states;
volatile i2c_states i2c_state = IDLE;
void i2c_send_byte(uint8_t data);
void i2c_send_bytes(uint8_t* data,uint8_t length);
uint8_t i2c_has_data(void);
uint8_t i2c_get_byte(void);
void i2c_clear_rcv_buffer(void);
#endif
