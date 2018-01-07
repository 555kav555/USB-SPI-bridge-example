#include "hw_config.h"
#include "usb_desc.h"

#include "bus-spi.h"
#include "desc.h"
#include "version.h"

#include <stdlib.h>

#define COMM_IN_EP 2

typedef struct __attribute__((packed)) {
	uint8_t report;
	uint8_t reset : 1;
	uint8_t power : 1;
} pins_t;

static pins_t pins = {1, 0, 0};

static void pins_set(pins_t *v) {
	GPIO_WriteBit(GPIOB, GPIO_Pin_6, !v->reset);
	GPIO_WriteBit(GPIOB, GPIO_Pin_7, !v->power);
}

static void pins_init() {
	GPIO_InitTypeDef gpioCfg;

	gpioCfg.GPIO_Pin = GPIO_Pin_6;
	gpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	gpioCfg.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &gpioCfg);

	gpioCfg.GPIO_Pin = GPIO_Pin_7;
	gpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	gpioCfg.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &gpioCfg);

	pins_set(&pins);
}

FIFO_DECL(commBuf_in, 1024);
FIFO_DECL(commBuf_out, 1024);
volatile int commDone_out = 1;

static int comm_hasData() {
	return fifo_toRead(&commBuf_in) != 0;
}

static int comm_hasSpace() {
	return fifo_toWrite(&commBuf_out) != 0;
}

static int comm_get(char *ptr, size_t len) {
	return fifo_read(&commBuf_in, (uint8_t*)ptr, len);
}

static uint8_t comm_getByte() {
	uint8_t v = 0;
	comm_get((char*)&v, 1);
	return v;
}

static int comm_put(const char *ptr, size_t len) {
	if(commDone_out) {
		int l = 0;
		l = USB_EP_write(COMM_IN_EP, (uint8_t*)ptr, len, 0);
		l += fifo_write(&commBuf_out, (uint8_t*)ptr + l, len - l);
		commDone_out = 0;
		USB_EP_setState(COMM_IN_EP, USB_EPR_STAT_TX_VALID);
		return l;
	}
	int l = fifo_write(&commBuf_out, (uint8_t*)ptr, len);
	if(commDone_out) {
		USB_EP_writeEmpty(COMM_IN_EP);
		commDone_out = 0;
		USB_EP_setState(COMM_IN_EP, USB_EPR_STAT_TX_VALID);
	}
	return l;
}

static void comm_putByte(uint8_t v) {
	comm_put((const char*)&v, 1);
}

static void USB_epsCommInH(uint8_t num, USB_event_t evt) {
	size_t l = fifo_toEP(&commBuf_out, num);
	if(l) USB_EP_setState(num, USB_EPR_STAT_TX_VALID);
	else commDone_out = 1;
}

static void USB_epsCommOutH(uint8_t num, USB_event_t evt) {
	fifo_fromEP(&commBuf_in, num);
	USB_EP_setState(num, USB_EPR_STAT_RX_VALID);
}

static void USB_configH(uint8_t cfg) {
	USB_EP_setup(1, 1, USB_EPR_EP_TYPE_INTERRUPT | USB_EPR_STAT_TX_NAK, 0, 0, NULL);
	USB_EP_setup(2, 2, USB_EPR_EP_TYPE_BULK | USB_EPR_STAT_TX_NAK, COMMPACKET_LEN, 0, USB_epsCommInH);
	USB_EP_setup(3, 3, USB_EPR_EP_TYPE_BULK | USB_EPR_STAT_RX_VALID, 0, COMMPACKET_LEN, USB_epsCommOutH);
	USB_EP_setup(4, 4, USB_EPR_EP_TYPE_INTERRUPT | USB_EPR_STAT_TX_NAK, 8, 0, NULL);
	USB_EP_setup(5, 5, USB_EPR_EP_TYPE_INTERRUPT | USB_EPR_STAT_RX_NAK, 0, 8, NULL);
	fifo_clear(&commBuf_in);
	fifo_clear(&commBuf_out);
}

static void _set_pins_H() {
	USB_ctl_outStatus(0, &USB_control, USB_EPR_STAT_TX_VALID);
	pins_set(&pins);
}

static void USB_HID_F_controlRequestHandler() {
	switch(USB_control.request.bmRequestType.Type) {
	case USB_REQUEST_TYPE_CLASS: {
		if(USB_control.request.bmRequestType.Dir) {
			switch(USB_control.request.bRequest) {
			case USB_HID_REQUEST_GET_REPORT: {
				switch(USB_control.request.wValue.H) {
				case USB_HID_REPORT_FEATURE:
					switch(USB_control.request.wValue.L) {
					case 1:
						USB_ctl_inData(0, &USB_control, (const uint8_t*)&pins, sizeof(pins));
						break;
					}
					break;
				default:
					break;
				}
			} break;
			default:
				break;
			}
		} else {
			switch(USB_control.request.bRequest) {
			case USB_HID_REQUEST_SET_REPORT: {
				switch(USB_control.request.wValue.H) {
				case USB_HID_REPORT_FEATURE:
					switch(USB_control.request.wValue.L) {
					case 1:
						USB_ctl_outDataH(0, &USB_control, (uint8_t*)&pins, sizeof(pins), _set_pins_H);
						break;
					}
					break;
				default:
					break;
				}
			} break;
			default:
				break;
			}
		}
	} break;
	default:
		break;
	}
}

void USB_LP_CAN1_RX0_IRQHandler() {
	USB_LP_Int();
}

void USB_HP_CAN1_TX_IRQHandler() {
	USB_HP_Int();
}

int main(void) {
	Set_System();
	Set_Interrupts();

	pins_init();
	busSPI_init();

	USB_CAN_intInit(2, 0);
	USB_init();
	Set_USBDesc();
	USB_onConfig = USB_configH;
	USB_onControlRequest[0] = USB_CDC_controlRequestHandler;
	USB_onControlRequest[1] = USB_HID_controlRequestHandler;
	USB_onControlRequest[2] = USB_HID_F_controlRequestHandler;

	USB_connect(true);

	while(1) {
		if(comm_hasData() && busSPI_isEmpty()) {
			uint8_t v = comm_getByte();
			busSPI_putByte(v);
		}
		if(comm_hasSpace() && busSPI_hasData()) {
			uint8_t v = busSPI_getByte();
			comm_putByte(v);
		}
	}
}
