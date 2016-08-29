#define F_CPU (16e6l)
#define FOSC F_CPU

#include <stdio.h>
#include <stdarg.h>
#include <avr/io.h>
#include <util/delay.h>
#include <ctype.h>

#include <uart.h>
#include <adc.h>

#define ENABLE  (1 << PB0) // d8
#define A0      (1 << PB4) // d12
#define A1      (1 << PB3) // d11
#define A2      (1 << PB2) // d10
#define A3      (1 << PB1) // d9
uint8_t pins[8] = {A0, A1, A2, A3};

#define PRINTBUFFSIZE 100

void set_mux_addr(uint8_t addr)
{
    uint8_t out = 0;
    for(uint8_t i = 0; i < (0x0f/2); i++)
    {
        out |= (pins[i] & addr);
    }
    PORTB |= (out < 1);
}

int print_uart(const char* format, ...)
{
    static char buffer[PRINTBUFFSIZE];

    va_list args;
    va_start(args, format);
    int written = vsprintf(buffer, format, args);
    va_end(args);

    char *start = buffer;
    while(*start)
        putByte(*(start++));
    return written;
}

int main(void)
{
    initUART();
    static uint16_t values[8];
    (void)values;
    static uint16_t value;
    print_uart("%s %d? \n", "Hello World!", 42);

    // uint16_t value = adc_read(ADC_PRESCALER_2, ADC_VREF_AVCC, 0);
    // sprintf(uart_buf, "%d\n", value);
    // print_uart(uart_buf);

    // enable multiplexer
    DDRB = 0xFF;
    DDRB &= ~(ENABLE);
    PORTB &= ~(ENABLE);

    // select address to measure.

    while(1)
    {
        for(int i = 0; i < (0x0F/2); i++)
        {
            set_mux_addr(i);
            value = adc_read(ADC_PRESCALER_128, ADC_VREF_AVCC, 0);
            values[i] = value;
            print_uart("%d:", value);
        }
        print_uart("\n");
    }
    return 0;
}
