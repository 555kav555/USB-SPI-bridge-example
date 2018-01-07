#include "fifo.h"

void fifo_init(fifo_t *fifo, uint8_t *buf, size_t len) {
	fifo->b = buf;
	fifo->e = buf + len;
	fifo->r = buf;
	fifo->w = buf;
}

size_t fifo_toRead(fifo_t *fifo) {
	register int r = fifo->w - fifo->r;
	if(r < 0) r += (fifo->e - fifo->b);
	return r;
}

size_t fifo_toWrite(fifo_t *fifo) {
	register int r = fifo->r - fifo->w - 1;
	if(r < 0) r += (fifo->e - fifo->b);
	return r;
}

size_t fifo_read(fifo_t *fifo,uint8_t *buf,size_t len){
	register size_t l=fifo_toRead(fifo);
	if(l>len)l=len;
	register fifo_ptr_t p=fifo->r;
	if(p+l<=fifo->e){
		memcpy(buf,p,l);
	}else{
		register size_t l0=fifo->e-p;
		memcpy(buf,p,l0);
		memcpy(buf+l0,fifo->b,l-l0);
	}
	p+=l;
	if(p>=fifo->e)p-=(fifo->e-fifo->b);
	fifo->r=p;
	return l;
}

size_t fifo_write(fifo_t *fifo,const uint8_t *buf,size_t len){
	register size_t l=fifo_toWrite(fifo);
	if(l>len)l=len;
	register fifo_ptr_t p=fifo->w;
	register size_t i;
	for(i=0;i<l;++i){
		*p=buf[i];
		++p;
		if(p==fifo->e)p=fifo->b;
	}
	fifo->w=p;
	return l;
}

size_t fifo_toEP(fifo_t *fifo,uint8_t num){
	size_t l,len=0;
	while(1){
		l=fifo_toReadBlk(fifo);
		l=USB_EP_write(num,fifo->r,l,len);
		if(!l)break;
		fifo->r+=l;
		if(fifo->r==fifo->e)fifo->r=fifo->b;
		len+=l;
	}
	return len;
}

size_t fifo_fromEP(fifo_t *fifo,uint8_t num){
	size_t l,len=0;
	while(1){
		l=fifo_toWriteBlk(fifo);
		l=USB_EP_read(num,fifo->w,l,len);
		if(!l)break;
		fifo->w+=l;
		if(fifo->w==fifo->e)fifo->w=fifo->b;
		len+=l;
	}
	return len;
}

fifo_ptr_t fifo_inc(fifo_t *fifo,fifo_ptr_t p,int v){
	p+=v;
	while(p>=fifo->e)p-=(fifo->e-fifo->b);
	while(p<fifo->b)p+=(fifo->e-fifo->b);
	return p;
}
