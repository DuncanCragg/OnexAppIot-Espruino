#!/bin/bash

#                      openocd -s /usr/share/openocd/scripts/ -f ./scripts/openocd-stlink.cfg -f ./scripts/openocd-script.cfg
#~/Sources/openocd/src/openocd -s ~/Sources/openocd/tcl       -f ./scripts/openocd-stlink.cfg -f ./scripts/openocd-script.cfg
#~/Sources/openocd/src/openocd -s ~/Sources/openocd/tcl       -f ./scripts/openocd-stlink.cfg -c init -c "reset init" -c halt -c "nrf51 mass_erase; reset; shutdown;"
 ~/Sources/openocd/src/openocd -s ~/Sources/openocd/tcl       -f ./scripts/openocd-cmsis-dap.cfg -f ./scripts/openocd-script.cfg
#~/Sources/openocd/src/openocd -s ~/Sources/openocd/tcl       -f ./scripts/openocd-stlink.cfg
#~/Sources/openocd/src/openocd -s ~/Sources/openocd/tcl       -f ./scripts/openocd-cmsis-dap.cfg
