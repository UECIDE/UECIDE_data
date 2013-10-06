/**
 * @file Cosa/Cipher/RC4.cpp
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

#include "Cosa/Cipher/RC4.hh"

void 
RC4::restart(const void* key, size_t len)
{
  for (uint16_t i = 0; i < sizeof(m_state); i++)
    m_state[i] = i;
    
  m_x = 0;
  m_y = 0;

  const uint8_t* buf = (const uint8_t*) key;
  uint8_t j = 0;
  for (uint16_t i = 0; i < 256; i++) {
    j = j + m_state[i] + buf[i % len];
    uint8_t tmp = m_state[i];
    m_state[i] = m_state[j];
    m_state[j] = tmp;
  }
}

