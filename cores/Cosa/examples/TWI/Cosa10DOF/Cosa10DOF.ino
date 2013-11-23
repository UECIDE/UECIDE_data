/**
 * @file Cosa10DOF.ino
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
 * Cosa demonstration of 10 DOF module (GY-80) with ADXL345, BMP085,
 * HMC5883L, and L3G4200D; 3-axis acceleratometer, thermometer, barometer,
 * 3-axis compass and 3-axis gyroscope. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/ADXL345.hh"
#include "Cosa/TWI/Driver/BMP085.hh"
#include "Cosa/TWI/Driver/HMC5883L.hh"
#include "Cosa/TWI/Driver/L3G4200D.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Digital acceleratometer with alternative address
ADXL345 acceleratometer(1);

// Digital temperature and pressure sensor
BMP085 bmp;

// The 3-Axis Digital Compass
HMC5883L compass;

// Digital Gyroscope using alternative address
L3G4200D gyroscope(1);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("Cosa10DOF: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(ADXL345));
  TRACE(sizeof(BMP085));
  TRACE(sizeof(HMC5883L));
  TRACE(sizeof(L3G4200D));

  // Start the watchdog ticks and the sensors
  Watchdog::begin();
  TRACE(acceleratometer.begin());
  TRACE(bmp.begin(BMP085::ULTRA_LOW_POWER));
  TRACE(gyroscope.begin());

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.set_output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.set_samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.set_range(HMC5883L::RANGE_4_0_GA);
  compass.set_mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);
  TRACE(compass.begin());

  SLEEP(1);
}

void loop()
{
  // Sample and print measurement to output stream
  trace << acceleratometer << endl;

  // Sample sensor and print temperature and pressure
  bmp.sample();
  trace << bmp << endl;

  // Read the heading, scale to milli gauss and print the data
  compass.read_heading();
  compass.to_milli_gauss();
  trace << compass << endl;

  // Periodically sample the printout the gyroscope reading
  trace << gyroscope << endl;
  trace << endl;
  SLEEP(2);
}
