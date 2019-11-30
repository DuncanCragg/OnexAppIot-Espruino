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
#include "jsinteractive.h"
#include "jshardware.h"

#if !defined(ONEX)

int main() {

  jshInit();

  bool buttonState = false;
#ifdef BTN1_PININDEX
  buttonState = jshPinGetValue(BTN1_PININDEX) == BTN1_ONSTATE;
#endif
  jsvInit(0);
  jsiInit(!buttonState /* load from flash by default */); // pressing USER button skips autoload

  while (1)
  {
    jsiLoop();
  }
  //jsiKill();
  //jsvKill();
  //jshKill();
}

#else

#include "jswrap_microbit.h"

#include <onex-kernel/gpio.h>
#include <onex-kernel/log.h>
#include <onf.h>
#include <onr.h>

object* button;
object* light;

bool evaluate_button_io(object* button, void* pressed);
bool evaluate_light_io(object* light, void* d);

JsVar* onled  = 0;
JsVar* offled = 0;

void* x;
#define WHERESTHEHEAP(s) x = malloc(1); log_write("heap after %s: %x\n", s, x);

int main()
{
  jshInit();
  jsvInit(0);
  jsiInit(false);

  onled  = jsvNewFromString("            #            ");
  offled = jsvNewFromString("                         ");
  jswrap_microbit_show(onled);

  onex_init("");

#define BUTTON_1 17 // !!
  gpio_mode(BUTTON_1, INPUT_PULLUP);

  onex_set_evaluators("evaluate_button", evaluate_button_io, 0);
  onex_set_evaluators("evaluate_light", evaluate_light_logic, evaluate_light_io, 0);

  char* buttonuid = "uid-1-2-3";
  button=object_new(buttonuid, "evaluate_button", "button", 4);
  light =object_new(0,         "evaluate_light",  "light", 4);
  char* lightuid=object_property(light, "UID");

  object_property_set(button, "name", "£€§");

  object_property_set(light, "light", "off");
  object_property_set(light, "button", buttonuid);

  onex_run_evaluators(lightuid, 0);

  bool button_pressed=false;

  while(1){
    jsiLoop();
    onex_loop();
    if(button_pressed != gpio_get(BUTTON_1)){
      button_pressed = gpio_get(BUTTON_1);
      onex_run_evaluators(buttonuid, (void*)button_pressed);
    }
  }
}

bool evaluate_button_io(object* button, void* pressed)
{
  char* s=(char*)(pressed? "down": "up");
  object_property_set(button, "state", s);
  return true;
}

bool evaluate_light_io(object* light, void* d)
{
  char* lightuid=object_property(light, "UID");
  log_write("Light UID %s\n", lightuid);

  if(object_property_is(light, "light", "on")){
    // move M:B-specific display stuff into display.h
    jswrap_microbit_show(onled);
    WHERESTHEHEAP("evaluate_light_io on");
  }
  else {
    jswrap_microbit_show(offled);
    WHERESTHEHEAP("evaluate_light_io off");
  }
  return true;
}

// --------------------------------------------------------------------
#endif
