#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ws2812.h"
#include "../pixels.h"

static uint8_t * rgbData;

static uint16_t rgbDataCounter;
static uint8_t * rgbDataPointer;
static uint16_t lookupTable[2];

// gamma correction at driver level....
const uint8_t gamma[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
		2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6,
		6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12,
		13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21,
		21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32,
		33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
		49, 50, 50, 51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67,
		68, 69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89, 90,
		92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
		115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138,
		140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167,
		169, 171, 173, 175, 177, 180, 182, 184, 186, 189, 191, 193, 196, 198,
		200, 203, 205, 208, 210, 213, 215, 218, 220, 223, 225, 228, 231, 233,
		236, 239, 241, 244, 247, 249, 252, 255 };

/*
 * SPI Interrupt handler responsible for sending data to WS2812x
 */
void ws2812_SPIIntHandler(void) {

    SPI_clearInterruptFlag(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT);
	if (rgbDataCounter > 0) {
	    uint8_t bitNum = 0;
	    uint8_t byte = *rgbDataPointer++;

		rgbDataCounter--;

		while(bitNum++ < 7) {
			UCB0TXBUF = lookupTable[byte & 0x01];
			byte >>= 1;
			while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
		}
		UCB0TXBUF = lookupTable[byte & 0x01];
	}
}

/*
 * Configure SPI
 */
void _SPIInit() {
	eUSCI_SPI_MasterConfig spiMasterConfig2 =
	{
		EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             					// SMCLK Clock Source
		CS_getSMCLK(),                                   			// SMCLK = DCO = 48MHZ
		SPI_CLOCK,                                    				// SPICLK = 6Mhz
		EUSCI_B_SPI_MSB_FIRST,                     					// MSB First
		EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
		EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, 					// High polarity
		EUSCI_B_SPI_3PIN                           					// 3Wire SPI Mode
	};

	// OUTPUT on p1.6
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
			GPIO_PIN5 | GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);

	SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig2);
	SPI_enableModule(EUSCI_B0_BASE);
	SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIB0);
}

/*
 * Prepare data and lookup table
 */
void _DataInit(uint16_t ledCount) {
	rgbData = malloc(ledCount * 3);
	uint16_t b = ledCount;
	uint8_t* ledPointer;
	while (b > 0) {
		b--;
		ledPointer = rgbData + b;
		*ledPointer = 0;
		*(ledPointer + 1) = 0;
		*(ledPointer + 2) = 0;
	}

	lookupTable[0] = WS2812B_0;
	lookupTable[1] = WS2812B_1;
}


/*
 * When RGB data is ready, call this function to send it to WS2812x LEDs
 */
void ws2812_sendData(Pixels* pixelData) {
	// rgb ->  g, r, b
	uint16_t i = pixelData->pixelCount;
	uint8_t* pixRef = pixelData->pixelData;
	uint8_t* dataRef = rgbData;
	while (i > 0) {
		i--;
		*(dataRef) = gamma[*(pixRef + 1)];
		*(dataRef + 1) = gamma[*(pixRef)];
		*(dataRef + 2) = gamma[*(pixRef + 2)];

		dataRef += 3;
		pixRef += 3;
	}

	// You could use conditional to make sure prevoius call was finished
	while (rgbDataCounter != 0)  {};

	rgbDataCounter = LED_BYTE_COUNT;
	rgbDataPointer = rgbData;

    // Start transfer
	UCB0TXBUF = 0;
}

void ws2812_init(uint16_t ledCount) {
	_DataInit(ledCount);
	_SPIInit();
}

