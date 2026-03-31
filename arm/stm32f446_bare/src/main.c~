#include "periph_init.h"

int main(void)
{
    MX_GPIO_Init();

    while (1)
    {
        GPIOA->BSRR = (1 << 5);          // PA5 = 1 (LED on)
        delay_ms(500);
        GPIOA->BSRR = (1 << (5+16));     // PA5 = 0 (LED off)
        delay_ms(500);
    }
}
