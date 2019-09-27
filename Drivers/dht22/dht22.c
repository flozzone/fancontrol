#include "gpio.h"
#include "main.h"
#include "tim.h"
#include "dht22.h"


uint16_t bits[40];

uint8_t  hMSB = 0;
uint8_t  hLSB = 0;
uint8_t  tMSB = 0;
uint8_t  tLSB = 0;
uint8_t  parity_rcv = 0;

static GPIO_InitTypeDef DHT22_PORT;


void DHT22_Init(void) {
	//RCC_APB2PeriphClockCmd(DHT22_GPIO_CLOCK,ENABLE);

	DHT22_PORT.Mode = GPIO_MODE_OUTPUT_PP;
    DHT22_PORT.Pin = DHT22_Pin;
    DHT22_PORT.Pull = GPIO_NOPULL;
    DHT22_PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT22_GPIO_Port,&DHT22_PORT);
}

uint32_t DHT22_GetReadings(void) {
	uint32_t wait;
	uint8_t i;

    DHT22_PORT.Mode = GPIO_MODE_OUTPUT_PP;
    DHT22_PORT.Pin = DHT22_Pin;
    DHT22_PORT.Pull = GPIO_NOPULL;
    DHT22_PORT.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT22_GPIO_Port,&DHT22_PORT);

	// Generate start impulse for sensor

    DHT22_GPIO_Port->BRR = DHT22_Pin; // Pull down SDA (Bit_SET)
    usrDelayUS(3000); // Host start signal at least 800us
    DHT22_GPIO_Port->BSRR = DHT22_Pin; // Release SDA (Bit_RESET)
    usrDelayUS(25);
    //DHT22_GPIO_Port->BSRR = DHT22_Pin; // Release SDA (Bit_RESET)

	// Switch pin to input with Pull-Up
	DHT22_PORT.Mode = GPIO_MODE_INPUT;
    DHT22_PORT.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT22_GPIO_Port,&DHT22_PORT);

	// Wait for AM2302 to start communicate
	wait = 0;
	while ((DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 200)) usrDelayUS(2);
	if (wait > 50) return DHT22_RCV_NO_RESPONSE;

	// Check ACK strobe from sensor
	wait = 0;
	while (!(DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 100)) usrDelayUS(1);
	if ((wait < 16) || (wait > 30)) return DHT22_RCV_BAD_ACK1;

	wait = 0;
	while ((DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 100)) usrDelayUS(1);
	if ((wait < 16) || (wait > 30)) return DHT22_RCV_BAD_ACK2;

	// ACK strobe received --> receive 40 bits
	i = 0;
	while (i < 40) {
		// Measure bit start impulse (T_low = 50us)
		wait = 0;
		while (!(DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 40)) usrDelayUS(1);
		if (wait > 32) {
			// invalid bit start impulse length
			bits[i] = 0xffff;
			while ((DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 40)) usrDelayUS(1);
		} else {
			// Measure bit impulse length (T_h0 = 25us, T_h1 = 70us)
			wait = 0;
			while ((DHT22_GPIO_Port->IDR & DHT22_Pin) && (wait++ < 40)) usrDelayUS(1);
			bits[i] = (wait < 32) ? wait : 0xffff;
		}

		i++;
	}

	for (i = 0; i < 40; i++) if (bits[i] == 0xffff) return DHT22_RCV_RCV_TIMEOUT;

	return DHT22_RCV_OK;
}

uint16_t DHT22_DecodeReadings(void) {
	uint8_t parity;
	uint8_t  i = 0;

	hMSB = 0;
	for (; i < 8; i++) {
		hMSB <<= 1;
		if (bits[i] > 14) hMSB |= 1;
	}
	hLSB = 0;
	for (; i < 16; i++) {
		hLSB <<= 1;
		if (bits[i] > 14) hLSB |= 1;
	}
	tMSB = 0;
	for (; i < 24; i++) {
		tMSB <<= 1;
		if (bits[i] > 14) tMSB |= 1;
	}
	tLSB = 0;
	for (; i < 32; i++) {
		tLSB <<= 1;
		if (bits[i] > 14) tLSB |= 1;
	}
	for (; i < 40; i++) {
		parity_rcv <<= 1;
		if (bits[i] > 14) parity_rcv |= 1;
	}

	parity  = hMSB + hLSB + tMSB + tLSB;

	return (parity_rcv << 8) | parity;
}

uint16_t DHT22_GetHumidity(void) {
	return (hMSB << 8) + hLSB;
}

uint16_t DHT22_GetTemperature(void) {
	return (tMSB << 8) + tLSB;
}
