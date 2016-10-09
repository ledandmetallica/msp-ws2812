#include "../pixels.h"
#include "driverlib.h"
//
// set the number of LEDs
//

//
// WS2812x timing
//
#define SPI_CLOCK 6000000 // 6000000 = 6MHz = 0.167us per bit

#define WS2812B_0 0x60 // = high for 332ns  01100000
#define WS2812B_1 0x7C // = high for 830ns  01111100

#define SPI_INSTANCE EUSCI_B0_BASE //EUSCI_B0_MODULE

//void DataInit();
void ws2812_init(uint16_t ledCount);
void ws2812_sendData(Pixels* pixelData);
void ws2812_setLED(uint8_t ledNumber, uint8_t r, uint8_t g, uint8_t b);
