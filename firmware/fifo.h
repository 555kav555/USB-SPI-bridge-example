#ifndef __FIFO_H
#define __FIFO_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32f10x_usb.h"

typedef uint8_t* fifo_ptr_t;

typedef struct{
	fifo_ptr_t r;
	fifo_ptr_t w;
	fifo_ptr_t e;
	fifo_ptr_t b;
}fifo_t;

typedef struct{
	fifo_t fifo;
	uint8_t num;
}usb_fifo_t;

#define FIFO_DECL(N,S) uint8_t N##_buffer[S];fifo_t N={.b=N##_buffer,.e=N##_buffer+(S),.r=N##_buffer,.w=N##_buffer}
#define FIFO_OVER(N,B,S) fifo_t N={.b=(uint8_t*)(B),.e=(uint8_t*)(B)+(S),.r=(uint8_t*)(B),.w=(uint8_t*)(B)}

static inline void fifo_clear(fifo_t *fifo){fifo->r=fifo->w;}

void fifo_init(fifo_t *fifo,uint8_t *buf,size_t len);

static inline size_t fifo_toReadBlk(fifo_t *fifo){register int r=fifo->w-fifo->r; return (r<0)?(fifo->e-fifo->r):r;}
static inline size_t fifo_toWriteBlk(fifo_t *fifo){register int r=fifo->r-fifo->w-1; return (r<0)?(fifo->e-fifo->w):r;}

size_t fifo_toRead(fifo_t *fifo);
size_t fifo_toWrite(fifo_t *fifo);
size_t fifo_read(fifo_t *fifo,uint8_t *buf,size_t len);
size_t fifo_write(fifo_t *fifo,const uint8_t *buf,size_t len);
size_t fifo_toEP(fifo_t *fifo,uint8_t num);
size_t fifo_fromEP(fifo_t *fifo,uint8_t num);
fifo_ptr_t fifo_inc(fifo_t *fifo,fifo_ptr_t p,int v);

#endif
