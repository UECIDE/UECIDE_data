/**
 * @file CosaTWIscanner.ino
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
 * Cosa TWI bus tool; scanner for connected devices.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOStream cout(&uart);

void setup()
{
  uart.begin(9600);
  for (uint8_t addr = 3; addr < 128; addr++) {
    TWI::Driver dev(addr);
    twi.begin(&dev);
    uint8_t data;
    int count = twi.read(&data, sizeof(data));
    twi.end();
    if (count == sizeof(data)) {
      cout << PSTR("device = ") << hex << addr 
	   << PSTR(":group = ") << (addr >> 3) << '.' << (addr & 0x07)
	   << endl;
    }
  }
}

void loop()
{
}
