/**
 * @file Cosa/LCD/Driver/HD44780_IO_ERM1602_5.cpp
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

#include "Cosa/LCD/Driver/HD44780.hh"

bool
HD44780::ERM1602_5::setup()
{
  return (true);
}

void 
HD44780::ERM1602_5::write4b(uint8_t data)
{
  write8b(data);
}

void 
HD44780::ERM1602_5::write8b(uint8_t data)
{
  m_en.clear();
  if (m_dirty) {
    uint8_t func = FUNCTION_SET | DATA_LENGTH_8BITS | NR_LINES_2;
    if (m_rs) func |= EXTENDED_SET;
    m_sda.write(func, m_scl);
    DELAY(SHORT_EXEC_TIME);
    m_dirty = false;
  }
  if (m_rs) {
    m_sda.write(SET_DDATA_LENGTH, m_scl);
    DELAY(SHORT_EXEC_TIME);
  }
  m_sda.write(data, m_scl);
  m_en.set();
  DELAY(SHORT_EXEC_TIME);
}

void 
HD44780::ERM1602_5::write8n(void* buf, size_t size)
{
  if (size == 0) return;
  m_en.clear();
  if (m_dirty) {
    uint8_t func = FUNCTION_SET | DATA_LENGTH_8BITS | NR_LINES_2;
    if (m_rs) func |= EXTENDED_SET;
    m_sda.write(func, m_scl);
    DELAY(SHORT_EXEC_TIME);
    m_dirty = false;
  }
  if (m_rs) {
    m_sda.write(SET_DDATA_LENGTH | ((size - 1) & SET_DDATA_MASK), m_scl);
    DELAY(SHORT_EXEC_TIME);
  }
  uint8_t* bp = (uint8_t*) buf;
  while (size--) {
    m_sda.write(*bp++, m_scl);
    if (size)
      DELAY(SHORT_EXEC_TIME);
  }
  m_en.set();
  DELAY(SHORT_EXEC_TIME);
}

void 
HD44780::ERM1602_5::set_mode(uint8_t flag)
{
  if (m_rs == flag) return;
  m_rs = flag;
  m_dirty = !m_dirty;
}

void 
HD44780::ERM1602_5::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}
