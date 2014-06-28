typedef struct{
	uint8_t buffer_start;
	uint8_t buffer_stop;
	uint8_t data[32];
}buffer;

uint8_t add_buffer(uint8_t data, buffer);
uint8_t get_buffer(buffer);