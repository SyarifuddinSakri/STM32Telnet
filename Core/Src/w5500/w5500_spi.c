#include "stm32f1xx_hal.h"
#include "wizchip_conf.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;

uint8_t SPIReadWrite(uint8_t data){
	while ((hspi1.Instance->SR & SPI_FLAG_TXE)!=SPI_FLAG_TXE);
	*(__IO uint8_t*)&hspi1.Instance->DR=data;

	while((hspi1.Instance->SR & SPI_FLAG_RXNE)!=SPI_FLAG_RXNE);
	return(*(__IO uint8_t*)&hspi1.Instance->DR);
}

void wizchip_select(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void wizchip_deselect(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

uint8_t wizchip_read(void){
	uint8_t rb;
	rb=SPIReadWrite(0x00);//passing dummy data
	return rb;
}

void wizchip_write(uint8_t wb){
	SPIReadWrite(wb);//writing a byte into the SPI Interface
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len){
	for(int i=0; i<len;i++){
		pBuf=SPIReadWrite(0x00);
		pBuf++;
	}
}
void wizchip_writeburst(uint8_t* pBuf, uint16_t len){
	for(int i=0; i<len;i++){
		SPIReadWrite(*pBuf);
		pBuf++;
	}
}

void W5500IOInit(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void W5500Init(){
	uint8_t tmp;
	uint8_t memsize[2][8]={{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

	W5500IOInit();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);//Chip Select high by default

	tmp=0xff;
	while(tmp--);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);//Select chip by putting chip select low

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);

	if(ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize)==-1){
		printf("wizchip Initialization failed\r\n");
		while(1);
	}
	printf("wizchip initialization success\r\n");

}
