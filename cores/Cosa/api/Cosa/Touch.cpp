/**
 * @file Cosa/Touch.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Touch.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

Touch::Touch(Board::DigitalPin pin, uint16_t threshold) :
  IOPin(pin),
  Link(),
  THRESHOLD(threshold),
  m_sampling(false),
  m_touched(false)
{
  set_mode(OUTPUT_MODE);
  clear();
  Watchdog::attach(this, SAMPLE_RATE);
}

void 
Touch::on_event(uint8_t type, uint16_t value)
{
  // Check if sampling should be initiated
  if (!m_sampling) {
    set_mode(INPUT_MODE);
    m_sampling = true;
    return;
  } 

  // Sample the pin and discharge
  uint8_t state = is_clear();
  set_mode(OUTPUT_MODE);
  clear();
  m_sampling = false;

  // Was the pin discharge during the sampling period 
  if (state) {
    m_start = RTC::millis();
    if (!m_touched) {
      on_touch();
      m_touched = true;
    }
    return;
  }

  // The pin was discharge; low-pass filter pin change
  if (m_touched && RTC::since(m_start) > THRESHOLD) {
    m_touched = false;
  }
}

