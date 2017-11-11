#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "dht22.h"

#define MAXdhtData           5	// to complete 40 = 5*8 Bits
#define DHT_TIMEOUT_ERROR   -1

static const char* TAG = "DHT";

int   DHTgpio     = 4;				
float humidity    = 0.;
float temperature = 0.;

static int getSignalLevel(int usTimeOut, bool state)
{
	int uSec = 0;

	while( gpio_get_level(DHTgpio)==state ) {
		if( uSec > usTimeOut ) 
			return -1;
		++uSec;
		ets_delay_us(1);		// uSec delay
	}
	
	return uSec;
}

//////////////////////////////////////////////////////////////////////////////
// Public API
//

int32_t dht22_init(int gpio)
{
	DHTgpio = gpio;
    
    return 0;
}

int32_t dht22_readDHT(float *humi, float *temp)
{
    int uSec = 0;

    uint8_t dhtData[MAXdhtData];
    uint8_t byteInx = 0;
    uint8_t bitInx = 7;

	for(int k = 0; k<MAXdhtData; k++) {
		dhtData[k] = 0;
    }    

	gpio_set_direction(DHTgpio, GPIO_MODE_OUTPUT);
	gpio_set_level(DHTgpio, 0);
	ets_delay_us(3000);			

	gpio_set_level(DHTgpio, 1);
	ets_delay_us(25);

	gpio_set_direction(DHTgpio, GPIO_MODE_INPUT); 
	uSec = getSignalLevel(85, 0);
	if( uSec<0 ) return DHT_TIMEOUT_ERROR; 

	uSec = getSignalLevel(85, 1);
	if( uSec<0 ) return DHT_TIMEOUT_ERROR;
 
	for( int k = 0; k < 40; k++ ) {
		uSec = getSignalLevel(56, 0);
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		uSec = getSignalLevel(75, 1);
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		if (uSec > 40) {
			dhtData[ byteInx ] |= (1 << bitInx);
			}
	
		if (bitInx == 0) { bitInx = 7; ++byteInx; }
		else bitInx--;
	}

	humidity = dhtData[0];
	humidity *= 0x100;					// >> 8
	humidity += dhtData[1];
	humidity /= 10;						// get the decimal

	temperature = dhtData[2] & 0x7F;	
	temperature *= 0x100;				// >> 8
	temperature += dhtData[3];
	temperature /= 10;

	if( dhtData[2] & 0x80 ) 			// negative temp, brrr it's freezing
		temperature *= -1;
	
	if (dhtData[4] != ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) {
        *humi = 0;
        *temp = 0;
		return -1;
    }    

    *humi = humidity;
    *temp = temperature;
 
    return 0;
}
