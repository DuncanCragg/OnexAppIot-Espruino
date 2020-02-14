
import pinutils;

info = {
 'name' : "Adafruit Clue",
 'link' :  [ "https://www.adafruit.com/clue" ],
 'default_console' : "EV_BLUETOOTH",
 'variables' : 2100,
 'bootloader' : 1,
 'binary_name' : 'espruino_%v_cluejs.hex',
 'build' : {
   'optimizeflags' : '-Os',
   'libraries' : [
     'BLUETOOTH',
   ],
   'makefile' : [
     'DEFINES+=-DCONFIG_GPIO_AS_PINRESET',
     'DEFINES+=-DBLUETOOTH_NAME_PREFIX=\'"Clue"\'',
     'DFU_PRIVATE_KEY=targets/nrf5x_dfu/dfu_private_key.pem',
     'DFU_SETTINGS=--application-version 0xff --hw-version 52 --sd-req 0xA9',
     'NRF_SDK15=1'
   ]
 }
};

chip = {
  'part' : "NRF52840",
  'family' : "NRF52",
  'package' : "QFN48",
  'ram' : 256,
  'flash' : 1024,
  'speed' : 64,
  'usart' : 2,
  'spi' : 3,
  'i2c' : 2,
  'adc' : 1,
  'dac' : 0,
  'saved_code' : {
    'address' : ((223 - 10) * 4096),
    'page_size' : 4096,
    'pages' : 10,
    'flash_available' : 1024 - ((38 + 8 + 2 + 10)*4)
  },
};

devices = {
  'BTN1' : { 'pin' : 'D5', 'pinstate' : 'IN_PULLDOWN' },
};

board = {
  'left' : [],
  'right' : [],
  '_notes' : {
  }
};
board["_css"] = """
""";

def get_pins():
  pins = [
   { "name":"PD5", "sortingname":"D05", "port":"D", "num":"34", "functions":{}, "csv":{} }, # P1.02 = 34
  ];
  pinutils.findpin(pins, "PD5", True)["functions"]["NEGATED"]=0;
  # everything is non-5v tolerant
  for pin in pins:
    pin["functions"]["3.3"]=0;
  return pins
