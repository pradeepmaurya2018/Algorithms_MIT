#include "serial.h"
#include <stdint.h>

#define COM1 0x3F8

#define SERIAL_DATA       (COM1 + 0)
#define SERIAL_IER        (COM1 + 1)
#define SERIAL_FCR        (COM1 + 2)
#define SERIAL_LCR        (COM1 + 3)
#define SERIAL_MCR        (COM1 + 4)
#define SERIAL_LSR        (COM1 + 5)

/* Write one byte to an x86 I/O port. */
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

/* Read one byte from an x86 I/O port. */
static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

void serial_init(void)
{
    /* Disable UART interrupts. */
    outb(SERIAL_IER, 0x00);

    /* Enable divisor latch access. */
    outb(SERIAL_LCR, 0x80);

    /* Set baud rate to 38400 (115200 / 3). */
    outb(SERIAL_DATA, 0x03);
    outb(SERIAL_IER, 0x00);

    /* 8 data bits, no parity, one stop bit. */
    outb(SERIAL_LCR, 0x03);

    /* Enable FIFO, clear buffers. */
    outb(SERIAL_FCR, 0xC7);

    /* Enable DTR, RTS, and OUT2. */
    outb(SERIAL_MCR, 0x0B);
}

static void serial_putc(char c)
{
    /* Wait until the transmitter holding register is empty. */
    while ((inb(SERIAL_LSR) & 0x20) == 0) {
        __asm__ volatile ("pause");
    }

    outb(SERIAL_DATA, (uint8_t)c);
}

void serial_write(const char *str)
{
    while (*str != '\0') {
        if (*str == '\n') {
            serial_putc('\r');
        }

        serial_putc(*str++);
    }
}
