#include <bcm2835.h>
#include <stdio.h>

#define PIN RPI_V2_GPIO_P1_12

int main(int argc, char **argv)
{
    if (!bcm2835_init()) {
        printf("bcm2835_init() fail !\r\n");
        return -1;
    }
    
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
    
    while (1)
    {
        bcm2835_gpio_write(PIN, HIGH);
        bcm2835_delay(1000);
        bcm2835_gpio_write(PIN, LOW);
        bcm2835_delay(1000);
    }
    
    bcm2835_close();
    
    return 0;
}
