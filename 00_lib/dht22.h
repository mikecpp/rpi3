#ifndef __DHT22_H__
#define __DHT22_H__

int32_t dht22_init(int gpio);
int32_t dht22_readDHT(float *humi, float *temp);

#endif//__DHT22_H__
