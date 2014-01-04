/**
 * @file Cosa/ExternalInterrupt.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/ExternalInterrupt.hh"

#if defined(__ARDUINO_STANDARD__)

ExternalInterrupt::
ExternalInterrupt(Board::ExternalInterruptPin pin, 
		  InterruptMode mode, 
		  bool pullup) :
  IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
{
  m_ix = pin - Board::EXT0;
  ext[m_ix] = this;
  uint8_t ix = (m_ix << 1);
  bit_field_set(EICRA, 0b11 << ix, mode << ix);
}

#elif defined(__ARDUINO_MEGA__)

ExternalInterrupt::
ExternalInterrupt(Board::ExternalInterruptPin pin, 
		  InterruptMode mode, 
		  bool pullup) :
  IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
{
  if (pin <= Board::EXT5) {
    m_ix = pin - Board::EXT4;
    uint8_t ix = (m_ix << 1);
    bit_field_set(EICRB, 0b11 << ix, mode << ix);
    m_ix += 4;
  }
  else {
    m_ix = pin - Board::EXT0;
    uint8_t ix = (m_ix << 1);
    bit_field_set(EICRA, 0b11 << ix, mode << ix);
  } 
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_MIGHTY__)

ExternalInterrupt::
ExternalInterrupt(Board::ExternalInterruptPin pin, 
		  InterruptMode mode, 
		  bool pullup) :
  IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
{
  if (pin == Board::EXT2) {
    m_ix = 2;
  } else {
    m_ix = pin - Board::EXT0;
  } 
  uint8_t ix = (m_ix << 1);
  bit_field_set(EICRA, 0b11 << ix, mode << ix);
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_TINYX61__)

ExternalInterrupt::
ExternalInterrupt(Board::ExternalInterruptPin pin, 
		  InterruptMode mode, 
		  bool pullup) :
  IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
{
  m_ix = (pin == Board::EXT1);
  ext[m_ix] = this;
  bit_field_set(MCUCR, 0b11, mode);
}

void 
ExternalInterrupt::enable() 
{ 
  synchronized {
    bit_clear(GIFR, INTF0 + m_ix); 
    bit_set(GIMSK, INT0 + m_ix); 
  }
}

void 
ExternalInterrupt::disable() 
{ 
  synchronized {
    bit_clear(GIMSK, INT0 + m_ix);
  }
}

#elif defined(__ARDUINO_TINY__)

ExternalInterrupt::
ExternalInterrupt(Board::ExternalInterruptPin pin, 
		  InterruptMode mode, 
		  bool pullup) :
  IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
{
  m_ix = 0;
  ext[m_ix] = this;
  bit_field_set(MCUCR, 0b11, mode);
}

void 
ExternalInterrupt::enable() 
{ 
  synchronized {
    bit_clear(GIFR, INTF0); 
    bit_set(GIMSK, INT0); 
  }
}

void 
ExternalInterrupt::disable() 
{ 
  synchronized {
    bit_clear(GIMSK, INT0);
  }
}

#endif

#if !defined(__ARDUINO_TINY__)
void 
ExternalInterrupt::enable() 
{ 
  synchronized {
    bit_clear(EIFR, m_ix); 
    bit_set(EIMSK, m_ix); 
  }
}

void 
ExternalInterrupt::disable() 
{ 
  synchronized {
    bit_clear(EIMSK, m_ix); 
  }
}
#endif

ExternalInterrupt* ExternalInterrupt::ext[Board::EXT_MAX] = { NULL };

#define INT_ISR(nr)					\
ISR(INT ## nr ## _vect)					\
{							\
  if (ExternalInterrupt::ext[nr] != NULL)		\
    ExternalInterrupt::ext[nr]->on_interrupt();		\
}

INT_ISR(0)
#if defined(INT1_vect)
INT_ISR(1)
#if defined(INT2_vect)
INT_ISR(2)
#if defined(INT3_vect)
INT_ISR(3)
INT_ISR(4)
INT_ISR(5)
#endif
#endif
#endif
