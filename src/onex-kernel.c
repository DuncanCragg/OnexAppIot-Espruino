
#include <platform_config.h>
#include <jshardware.h>
#include <jsinteractive.h>

#include <onex-kernel/time.h>
#include <onex-kernel/serial.h>
#include <onex-kernel/log.h>
#include <onex-kernel/gpio.h>
#include <onex-kernel/random.h>

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

static serial_recv_cb recv_cb;

void serial_on_recv(uint32_t ch)
{
  if(recv_cb) ch? recv_cb(&ch): recv_cb(0);
}

bool serial_init(serial_recv_cb cb, uint32_t baudrate)
{
  if(serial_initialised) return true;
  recv_cb = cb;
  serial_initialised=true;
  return true;
}

void serial_cb(serial_recv_cb cb)
{
  recv_cb = cb;
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




#if defined(BOARD_MICROBIT)

#include "jswrap_microbit.h"

JsVar* onled  = 0;
JsVar* offled = 0;

#endif

static bool gpio_initialised=false;

void gpio_init()
{
#if defined(BOARD_MICROBIT)
  if(!gpio_initialised){
    onled  = jsvNewFromString("            #            ");
    offled = jsvNewFromString("                         ");
    jswrap_microbit_show(onled);
    gpio_initialised = true;
  }
#endif
}

void gpio_loop(){}

void gpio_mode(uint32_t pin, uint32_t mode)
{
#if defined(BOARD_MICROBIT)
  gpio_init();
#else
  jshPinSetState(pin, mode);
#endif
}

void gpio_mode_cb(uint32_t pin, uint32_t mode, gpio_pin_cb cb)
{
  gpio_mode(pin, mode);

  if(!(jshIsPinValid(pin) && (jsiIsWatchingPin(pin) || jshCanWatch(pin)))){ log_write("gpio_mode_cb pin %d cannot be watched\n", pin); return; }

  int edge = 0;
  bool repeat = true;
  JsVarFloat debounce = IS_PIN_A_BUTTON(pin)? 25: 0;

  JsVar *watchPtr = jsvNewObject();
  if (!watchPtr) { log_write("gpio_mode_cb pin %d run out of space\n", pin); return; }

  ;              jsvObjectSetChildAndUnLock(watchPtr, "pin",      jsvNewFromPin(pin));
  if(edge)       jsvObjectSetChildAndUnLock(watchPtr, "edge",     jsvNewFromInteger(edge));
  if(repeat)     jsvObjectSetChildAndUnLock(watchPtr, "recur",    jsvNewFromBool(repeat));
  if(debounce>0) jsvObjectSetChildAndUnLock(watchPtr, "debounce", jsvNewFromInteger((JsVarInt)jshGetTimeFromMilliseconds(debounce)));
  ;              jsvObjectSetChild(         watchPtr, "callback", jsvNewNativeFunction(cb,0));

  IOEventFlags exti = EV_NONE;
  if(!jsiIsWatchingPin(pin)) exti = jshPinWatch(pin, true);
  if(!exti){ log_write("gpio_mode_cb pin %d either already watching or cannot watch\n", pin); return; }
  jshSetEventCallback(exti, 0);

  JsVar* watchArrayPtr = jsvLock(watchArray);
  JsVarInt itemIndex = jsvArrayAddToEnd(watchArrayPtr, watchPtr, 1);
  jsvUnLock2(watchArrayPtr, watchPtr);
}

int gpio_get(uint32_t pin)
{
  return jshPinGetValue(pin)? 1: 0;
}

void gpio_set(uint32_t pin, uint32_t value)
{
#if defined(BOARD_MICROBIT)
  jswrap_microbit_show(value? onled: offled);
#else
  jshPinSetValue(pin, value);
#endif
}

void gpio_toggle(uint32_t pin)
{
}



static bool random_initialised=false;

uint8_t random_byte()
{
  return jshGetRandomNumber() & 0xFF;
}

uint8_t random_ish_byte(){
  if(!random_initialised){
    srand(jshGetRandomNumber());
    random_initialised=true;
  }
  return rand() & 0xFF;
}
