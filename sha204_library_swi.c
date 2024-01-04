#include <pico/printf.h>

#include "sha204_library.h"
#include "sha204_library_swi.pio.h"

#define SWI_UART_BAUD 230400
#define SWI_UART_PIO pio0
#define SWI_UART_SM 0

static uint pio_offset;

static inline void debug_printf(const char *__restrict, ...) {
#ifdef SHA204_SWI_DEBUG
    printf("> wakeup...\n");
#endif
}

void swi_init(uint8_t pin) {
    pio_offset = pio_add_program(SWI_UART_PIO, &sha204_pio_program);
    sha204_pio_program_init(SWI_UART_PIO, SWI_UART_SM, pio_offset, pin, SWI_UART_BAUD);
}

uint8_t swi_send_wakeup() {
    debug_printf("> wakeup...\n");
    sha204_pio_program_wake(SWI_UART_PIO, SWI_UART_SM, pio_offset);
    return SWI_FUNCTION_RETCODE_SUCCESS;
}

uint8_t swi_send_bytes(const uint8_t *buffer, uint8_t buffer_size) {
    debug_printf("> ");
    for (uint8_t i = 0; i < buffer_size; i++) {
        debug_printf("%02X ", buffer[i]);
    }
    debug_printf("\n");

    sha204_pio_program_tx_start(SWI_UART_PIO, SWI_UART_SM, pio_offset);
    for (uint8_t i = 0; i < buffer_size; i++) {
        sha204_pio_program_put_blocking(SWI_UART_PIO, SWI_UART_SM, buffer[i]);
    }
    sha204_pio_program_tx_stop(SWI_UART_PIO, SWI_UART_SM);
    return SWI_FUNCTION_RETCODE_SUCCESS;
}

uint8_t swi_send_byte(uint8_t value) {
    return swi_send_bytes(&value, 1);
}

uint8_t swi_receive_bytes(uint8_t *buffer, uint8_t buffer_size) {
    debug_printf("%d <... ", buffer_size);
    if (!sha204_pio_program_wait_readable(SWI_UART_PIO, SWI_UART_SM, 10 * 1000)) {
        debug_printf("swi_receive_bytes timeout [1]...\n");
        return SWI_FUNCTION_RETCODE_TIMEOUT;
    }
    for (uint8_t i = 0; i < buffer_size; ++i) {
        if (!sha204_pio_program_wait_readable(SWI_UART_PIO, SWI_UART_SM, 1000)) {
            debug_printf("swi_receive_bytes timeout [2]...\n");
            return SWI_FUNCTION_RETCODE_TIMEOUT;
        }
        buffer[i] = sha204_pio_program_get(SWI_UART_PIO, SWI_UART_SM);
        debug_printf("%02X ", buffer[i]);
    }
    debug_printf("...>\n");
    return SWI_FUNCTION_RETCODE_SUCCESS;
}
