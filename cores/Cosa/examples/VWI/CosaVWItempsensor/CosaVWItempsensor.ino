/**
 * @file CosaVWItempsensor.ino
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
 * Demonstration sending temperature readings from 1-Wire DS18B20
 * devices over the Virtual Wire Interface (VWI). Uses the VWI 
 * enhanced mode with addressing and message numbering.
 *
 * @section Note
 * This sketch is designed for an ATtiny85 running on the internal 
 * 8 MHz clock. Receive measurements with the CosaVWItempmonitor sketch.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC 
 * GND. Connect 1-Wire digital thermometer to D2 with pullup resistor.
 * The pullup resistor (4K7) may be connected to D3 to allow power
 * control. This sketch supports parasite powered DS18B20 devices.
 * Connect the DS18B20 VCC to GND. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/OWI.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"

// Connect to one-wire device; Assuming there are two sensors
OWI owi(Board::D2);
OutputPin pw(Board::D3, 0);
DS18B20 indoors(&owi);
DS18B20 outdoors(&owi);

// Connect RF433 transmitter to ATtiny/D1 alt. Arduino/D9
VirtualWireCodec codec;
#if defined(__ARDUINO_TINY__)
VWI::Transmitter tx(Board::D1, &codec);
const uint16_t ADDR = 0xC051;
#else
VWI::Transmitter tx(Board::D9, &codec);
const uint16_t ADDR = 0xC052;
#endif
const uint16_t SPEED = 4000;

void setup()
{
  // Set up watchdog for power down sleep
  Watchdog::begin(1024, SLEEP_MODE_PWR_DOWN);

  // Start the Virtual Wire Interface/Transmitter
  VWI::begin(ADDR, SPEED);
  tx.begin();

  // Connect to the temperature sensor
  pw.on();
  indoors.connect(0);
  outdoors.connect(1);
  pw.off();
  
  // Disable hardware
  VWI::disable();
  Power::adc_disable();
  Power::timer0_disable();
  Power::timer1_disable();
#if defined(__ARDUINO_TINY__)
  Power::usi_disable();
#else
  Power::spi_disable();
  Power::twi_disable();
  Power::timer2_disable();
  Power::usart0_disable();
#endif
}

// Message from the device; temperature and voltage reading
const uint8_t SAMPLE_CMD = 42;
struct sample_t {
  int16_t temperature[2];
  uint16_t voltage;
};

void loop()
{
  sample_t msg;
  
  // Make a conversion request and read the temperature (scratchpad)
  pw.on();
  DS18B20::convert_request(&owi, 12, true);
  indoors.read_scratchpad();
  outdoors.read_scratchpad();
  pw.off();
  
  // Turn on necessary hardware modules
  Power::timer1_enable();
  Power::adc_enable();

  // Initiate the message with measurements
  msg.temperature[0] = indoors.get_temperature();
  msg.temperature[1] = outdoors.get_temperature();
  msg.voltage = AnalogPin::bandgap(1100);

  // Enable wireless transmitter and send. Wait completion and disable
  VWI::enable();
  tx.send(&msg, sizeof(msg), SAMPLE_CMD);
  tx.await();
  VWI::disable();

  // Turn off hardware and sleep until next sample (period 2 s)
  Power::timer1_disable();
  Power::adc_disable();
  SLEEP(2);
}
