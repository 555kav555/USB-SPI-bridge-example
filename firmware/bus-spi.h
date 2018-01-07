#pragma once

#include <stdint.h>
#include "hw_config.h"

void busSPI_init();

static inline void busSPI_putByte(uint8_t v) {
	SPI_I2S_SendData(SPI1, v);
}

static inline uint8_t busSPI_getByte() {
	return SPI_I2S_ReceiveData(SPI1);
}

static inline int busSPI_isEmpty() {
	return SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE);
}

static inline int busSPI_hasData() {
	return SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE);
}
