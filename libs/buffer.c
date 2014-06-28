#ifndef BUFFER_C
#define BUFFER_C

typedef struct{
	uint8_t adr;
	uint8_t start;
	uint8_t stop;
	uint8_t data[32];
}buffer;

void init_buffer(volatile buffer *buf,uint8_t adr){
	buf->start = 0;
	buf->stop = 0;
	buf->adr = adr;
}

uint8_t buffer_empty(volatile buffer *buf){
	return (buf->start==buf->stop);	
}

void add_buffer(uint8_t data,volatile buffer *buf){
	buf->data[buf->stop] = data;
	buf->stop = (buf->stop + 1); //& 31; //satans viktigt
}
uint8_t buffer_look_ahead(volatile buffer *buf){
	return buf->data[buf->start];
}
uint8_t get_buffer(volatile buffer *buf){
	
	uint8_t retur_data = buf->data[buf->start];
	buf->start = (buf->start + 1); //& 31; //satans viktigt oxo
	return retur_data;
}
#endif
