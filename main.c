/* Standard Includes */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "driver/ws2812.h"
#include "pixels.h"

Pixels *_pixels;

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    volatile uint32_t ii;

	_pixels = (Pixels*)malloc(sizeof(Pixels));
	_pixels->pixelCount = LED_COUNT;
	_pixels->pixelData = malloc(LED_BYTE_COUNT);

    ws2812_init(LED_COUNT);
    pixels_setAllColor(_pixels, 0x0000CC00);

    while(1)
    {
        /* Delay Loop */
        for(ii=0;ii<50000;ii++)
        {
        }

        ws2812_sendData(_pixels);
    }
}
