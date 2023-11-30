#include "stm32f1xx_hal.h"
#include "wizchip_conf.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;

void wizchip_select(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void wizchip_deselect(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}
// Single byte read function
uint8_t wizchip_read(void) {
    uint8_t rbuf;
    HAL_SPI_Receive(&hspi1, &rbuf, 1, 0xFFFFFFFF);
    return rbuf;
}

// Single byte write function
void wizchip_write(uint8_t b) {
    HAL_SPI_Transmit(&hspi1, &b, 1, 0xFFFFFFFF);
}

// Read burst function
void wizchip_read_burst(uint8_t* pBuf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        pBuf[i] = wizchip_read();
    }
}

// Write burst function
void wizchip_write_burst(uint8_t* pBuf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        wizchip_write(pBuf[i]);
    }
}

void W5500Init(){
	uint8_t tmp;
	uint8_t memsize[2][8]={{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);//Chip Select high by default

	//send pulse on reset pin
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	tmp=0xff;
	while(tmp--);//small delay
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_read_burst, wizchip_write_burst);

	if(ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize)==-1){
		printf("wizchip Initialization failed\r\n");
		while(1);
	}
	printf("wizchip initialization success\r\n");

}
