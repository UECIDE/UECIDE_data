/**
 * @file color16.h
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
 * Evaluate different methods of bit-field access using macro, enum,
 * shift and mask, and bit-fields struct definitions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COLOR16_H__
#define __COLOR16_H__

#include "Cosa/Types.h"

union color16_t {
  uint16_t rgb;
  struct {
    unsigned int blue:5;
    unsigned int green:6;
    unsigned int red:5;
  };
};

uint16_t color16a(uint8_t red, uint8_t green, uint8_t blue);
uint16_t color16b(uint8_t red, uint8_t green, uint8_t blue);
#endif
