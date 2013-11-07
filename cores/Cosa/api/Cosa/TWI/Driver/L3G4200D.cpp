/**
 * @file Cosa/TWI/Driver/L3G4200D.cpp
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

#include "Cosa/TWI/Driver/L3G4200D.hh"

bool 
L3G4200D::begin()
{
  ctrl_reg1_t reg;
  reg.Xen = 1;
  reg.Yen = 1;
  reg.Zen = 1;
  reg.PD = 1;
  write(CTRL_REG1, reg);
  return (true);
}

bool 
L3G4200D::end()
{
  write(CTRL_REG1, 0);
  return (true);
}

void 
L3G4200D::write(Register reg, uint8_t value)
{
  twi.begin(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.end();
}

void 
L3G4200D::write(Register reg, void* buffer, uint8_t count)
{
  twi.begin(this);
  twi.write((uint8_t) (reg | AUTO_INC), buffer, count);
  twi.end();
}

uint8_t 
L3G4200D::read(Register reg)
{
  uint8_t res;
  twi.begin(this);
  twi.write((uint8_t) reg);
  twi.read(&res, sizeof(res));
  twi.end();
  return (res);
}

void 
L3G4200D::read(Register reg, void* buffer, uint8_t count)
{
  twi.begin(this);
  twi.write((uint8_t) (reg | AUTO_INC)); 
  twi.read(buffer, count);
  twi.end();
}

IOStream& 
operator<<(IOStream& outs, L3G4200D& gyroscope)
{
  L3G4200D::sample_t value;
  gyroscope.sample(value);
  outs << PSTR("L3G4200D(x = ") << value.x
       << PSTR(", y = ") << value.y
       << PSTR(", z = ") << value.z
       << PSTR(")");
  return (outs);
}

