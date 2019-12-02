
#include <platform_config.h>
#include <jshardware.h>

#include <onex-kernel/time.h>
#include <onex-kernel/serial.h>
#include <onex-kernel/log.h>
#include <onex-kernel/gpio.h>

static bool time_initialised=false;

void time_init()
{
  if(time_initialised) return;
  time_delay_ms(50);
  time_initialised=true;
}

uint32_t time_ms(){ return 0; }
uint32_t time_us(){ return 0*1000; }

void time_delay_us(uint32_t us)
{
  jshDelayMicroseconds(us);
}

void time_delay_ms(uint32_t ms)
{
  time_delay_us(ms*1000);
}

void time_delay_s( uint32_t s)
{
  time_delay_ms(s*1000);
}



static bool serial_initialised=false;

static uart_rx_handler_t rx_handler;

void uart_char_incoming(uint32_t ch)
{
  if(rx_handler) rx_handler(&ch);
}

void serial_init(uart_rx_handler_t cb, uint32_t baudrate)
{
  if(serial_initialised) return;
  rx_handler = cb;
  serial_initialised=true;
}

void serial_cb(uart_rx_handler_t cb)
{
  rx_handler = cb;
}

void serial_putchar(uint32_t ch)
{
  if(!serial_initialised) serial_init(0,0);
  jshTransmit(DEFAULT_ONP_DEVICE, ch);
}

static void serial_write_cb(const char* str, void* d)
{
  while(*str) jshTransmit(DEFAULT_ONP_DEVICE, (unsigned char)*(str++));
}

int serial_printf(const char* fmt, ...)
{
  if(!serial_initialised) serial_init(0,0);
  va_list args;
  va_start(args, fmt);
  vcbprintf(serial_write_cb, 0, fmt, args);
  va_end(args);
  return 0; // vcbprintf doesn't return chars
}




static bool log_initialised=false;

void log_init(uint32_t baudrate)
{
  if(log_initialised) return;
  log_initialised=true;
}

static void log_write_cb(const char* str, void* d)
{
  while(*str) jshTransmit(DEFAULT_CONSOLE_DEVICE, (unsigned char)*(str++));
}

int log_write(const char* fmt, ...)
{
  if(!log_initialised) log_init(0);
  va_list args;
  va_start(args, fmt);
  vcbprintf(log_write_cb, 0, fmt, args);
  va_end(args);
  return 0; // vcbprintf doesn't return chars
}



void gpio_mode(uint32_t pin, uint32_t mode)
{
  jshPinSetState(BTN1_PININDEX, JSHPINSTATE_GPIO_IN_PULLDOWN);
}

int  gpio_get(uint32_t pin)
{
  return jshPinGetValue(BTN1_PININDEX)? 1: 0;
}
/*
void gpio_set(uint32_t pin, uint32_t value)
{
  if (value) NRF_GPIO->OUTSET = (1 << pin);
  else       NRF_GPIO->OUTCLR = (1 << pin);
}

void gpio_toggle(uint32_t pin)
{
  uint32_t m=(1 << pin);
  uint32_t gpio_state = NRF_GPIO->OUT;
  NRF_GPIO->OUTSET = (m & ~gpio_state);
  NRF_GPIO->OUTCLR = (m &  gpio_state);
}
*/
