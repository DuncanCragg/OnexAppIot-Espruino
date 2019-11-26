/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Platform Specific entry point
 * ----------------------------------------------------------------------------
 */
 
#include "platform_config.h"

#include "jshardware.h"
#include "jswrap_microbit.h"
#include "jsinteractive.h"

#include <onex-kernel/time.h>
#include <onex-kernel/serial.h>

static volatile uint32_t speed = 128;

static void serial_received(char* ch)
{
  if(*ch=='o') speed/=2;
  if(*ch=='i') speed*=2;
  if(!speed)  speed=1;
}

int main()
{
  serial_init(serial_received, 0);
  time_init();

  jshInit();
  jsvInit(0);
  jsiInit(false);

  serial_printf("Type 'o' or 'i'\n");

  JsVar* onled  = jsvNewFromString("            #            ");
  JsVar* offled = jsvNewFromString("                         ");

  while (1) 
  {
    jsiLoop();

    serial_printf("%d\n", speed);
    jswrap_microbit_show(onled);
    time_delay_ms(speed);
    jswrap_microbit_show(offled);
    time_delay_ms(speed);
  }
}

// --------------------------------------------------------------------
