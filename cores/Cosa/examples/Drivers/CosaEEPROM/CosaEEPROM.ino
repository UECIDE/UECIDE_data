/**
 * @file CosaEEPROM.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demo of the default EEPROM device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/EEPROM.hh"
#include "Cosa/Pins.hh"

// EEPROM access object
EEPROM eeprom;

// Simple configuration struct in EEPROM
static const int NAME_MAX = 16;
struct config_t {
  int mode;
  int speed;
  char name[NAME_MAX];
};
config_t config EEMEM;

// Sensor log in EEPROM
static const int DATA_MAX = 8;
uint16_t data[DATA_MAX] EEMEM;

// Analog input pin
AnalogPin sensor(Board::A0);

void setup()
{
  // Use serial as output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaEEPROM: started"));
  TRACE(free_memory());
  Watchdog::begin();
  
  // Initiate data vector with index
  for (uint8_t i = 0; i < DATA_MAX; i++) 
    ASSERT(eeprom.write(&data[i], (uint16_t) 0xffff) == sizeof(uint16_t));

  // Initiate configuration struct
  config_t init;
  init.mode = 17;
  init.speed = 9600;
  strcpy_P(init.name, PSTR(".EEPROM"));
  ASSERT(eeprom.write(&config, &init, sizeof(config)) == sizeof(config));

  // Read the configuration and print
  ASSERT(eeprom.read(&init, &config, sizeof(init)) == sizeof(init));
  trace << PSTR("init(mode = ") << init.mode
	<< PSTR(", speed = ") << init.speed
	<< PSTR(", name = \"") << init.name
	<< PSTR("\")\n");
}

void loop()
{
  static int i = 0;
  uint16_t x;
  
  // Print sensor samples
  if (i == 0) {
    for (i = 0; i < membersof(data); i++) {
      ASSERT(eeprom.read(&x, &data[i]) == sizeof(x));
      trace << PSTR("data[") << i << PSTR("] = ") << x << endl;
    }
    i = 0;
    trace << endl;
  }

  // Update data element and write back
  x = sensor.sample();
  ASSERT(eeprom.write(&data[i], x) == sizeof(x));

  // Iterate to the next data element
  i += 1;
  if (i == membersof(data)) i = 0;

  // Take a nap
  SLEEP(2);
}
