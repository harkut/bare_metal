#include "periph_init.h"

void MX_GPIO_Init(void)
{
    /* Включаем тактирование GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA5 как выход (MODER[11:10] = 01) */
    GPIOA->MODER &= ~(3 << (5*2));
    GPIOA->MODER |= (1 << (5*2));

    /* Push-pull, высокая скорость, без подтяжки */
    GPIOA->OTYPER &= ~(1 << 5);
    GPIOA->OSPEEDR |= (3 << (5*2));
    GPIOA->PUPDR &= ~(3 << (5*2));
}

void delay_ms(uint32_t ms)
{
    /* Простая задержка, скорректированная под 16 МГц */
    for (uint32_t i = 0; i < ms; i++)
        for (uint32_t j = 0; j < 40000; j++)   // увеличено для заметного мигания
            __NOP();
}
