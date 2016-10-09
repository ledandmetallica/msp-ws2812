# msp-ws2812
WS2812/WS2811 driver for the TI MSP432

Inspired by RobG of 43oh.com's TivaC implementation
(http://forum.43oh.com/topic/5697-ws2812-ws2811-driver-for-tivastellaris/)

Tested on a MSP432 launchpad with clock set as 48mhz. SPI is set at 6mhz, and sends one byte each interrupt.
