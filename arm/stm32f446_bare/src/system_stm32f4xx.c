#include "stm32f4xx.h"

uint32_t SystemCoreClock = 16000000;

void SystemInit(void)
{
    SCB->VTOR = FLASH_BASE | 0x00000000;
}

void SystemCoreClockUpdate(void)
{
    /* Для простоты оставляем HSI */
    SystemCoreClock = 16000000;
}
