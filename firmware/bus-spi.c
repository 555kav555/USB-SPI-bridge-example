#include "bus-spi.h"

#define SPIIF_PORT GPIOA
#define SPIIF_PIN_CS GPIO_Pin_4
#define SPIIF_PIN_SCK GPIO_Pin_5
#define SPIIF_PIN_MISO GPIO_Pin_6
#define SPIIF_PIN_MOSI GPIO_Pin_7

void busSPI_init() {
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

	SPI_InitTypeDef spiCfg;
	spiCfg.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiCfg.SPI_Mode = SPI_Mode_Master;
	spiCfg.SPI_DataSize = SPI_DataSize_8b;
	spiCfg.SPI_CPOL = SPI_CPOL_Low;
	spiCfg.SPI_CPHA = SPI_CPHA_1Edge;
	spiCfg.SPI_NSS = SPI_NSS_Hard;
	spiCfg.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	spiCfg.SPI_FirstBit = SPI_FirstBit_MSB;
	spiCfg.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &spiCfg);
	SPI_Cmd(SPI1, ENABLE);

	GPIO_InitTypeDef gpioCfg;
	gpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	gpioCfg.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioCfg.GPIO_Pin = SPIIF_PIN_MISO;
	GPIO_Init(SPIIF_PORT, &gpioCfg);
	gpioCfg.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioCfg.GPIO_Pin = SPIIF_PIN_SCK | SPIIF_PIN_MOSI | SPIIF_PIN_CS;
	GPIO_Init(SPIIF_PORT, &gpioCfg);
}
