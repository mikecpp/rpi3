#include "util.h"

void delay_ms(int32_t val)
{
    vTaskDelay(val / portTICK_PERIOD_MS);
}
