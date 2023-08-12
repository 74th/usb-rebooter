#include "ch32v003fun.h"
#include "usb_rebooter.h"
#include <stdio.h>

#define LOOP_MS 1000

void init_rcc(void)
{
    RCC->CFGR0 &= ~(0x1F << 11);

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_ADC1;
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
}

void setup_uart(int uartBRR)
{
    // Enable GPIOD and UART.
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

    // Push-Pull, 10MHz Output, GPIO D5, with AutoFunction
    GPIOD->CFGLR &= ~(0xf << (4 * 5));
    GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 5);

    // 115200, 8n1.  Note if you don't specify a mode, UART remains off even when UE_Set.
    USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx;
    USART1->CTLR2 = USART_StopBits_1;
    USART1->CTLR3 = USART_HardwareFlowControl_None;

    USART1->BRR = uartBRR;
    USART1->CTLR1 |= CTLR1_UE_Set;
}

// For debug writing to the UART.
int _write(int fd, const char *buf, int size)
{
    for (int i = 0; i < size; i++)
    {
        while (!(USART1->STATR & USART_FLAG_TC))
            ;
        USART1->DATAR = *buf++;
    }
    return size;
}

int main()
{
    SystemInit();
    init_rcc();

#ifdef FUNCONF_USE_UARTPRINTF
    printf("using ch32v003fun\r\n");
    printf("initialize\r\n");
#endif

#ifdef FUNCONF_USE_UARTPRINTF
    printf("initialize done\r\n");
#endif

    while (1)
    {
        loop();
        Delay_Ms(LOOP_MS);
    }
}
