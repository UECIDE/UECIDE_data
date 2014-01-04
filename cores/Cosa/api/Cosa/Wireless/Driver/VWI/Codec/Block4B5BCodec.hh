/**
 * @file Cosa/Wireless/Driver/VWI/Codec/Block4B5BCodec.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel (Cosa C++ port and refactoring)
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_WIRELESS_DRIVER_VWI_CODEC_BLOCK4B5BCODEC_HH__
#define __COSA_WIRELESS_DRIVER_VWI_CODEC_BLOCK4B5BCODEC_HH__

#include "Cosa/Wireless/Driver/VWI.hh"

/**
 * Block Coding 4 to 5 bit codec for the Cosa VWI (Virtual Wire
 * Interface). 
 */
class Block4B5BCodec : public VWI::Codec {
private:
  /** Symbol mapping table: 4 to 5 bits */
  static const uint8_t symbols[] PROGMEM;

  /** Code mapping table: 5 to 4 bits */
  static const uint8_t codes[] PROGMEM;

  /** Message preamble with start symbol */
  static const uint8_t preamble[] PROGMEM;
  
public:
  /**
   * Construct block 4b5b codec with given bits per symbol,
   * start symbol, and preamble size.
   */
  Block4B5BCodec() : VWI::Codec(5, 0x238, 8) {}
  
  /**
   * @override VWI::Codec
   * Returns pointer to 4B5B frame preamble in program memory.
   * @return pointer.
   */
  virtual const uint8_t* get_preamble() 
  { 
    return (preamble); 
  }
  
  /**
   * @override VWI::Codec
   * Returns block 5-bit symbol for given 4-bit data.
   * @param[in] nibble to encode.
   * @return 5-bit bitstuffed code.
   */
  virtual uint8_t encode4(uint8_t nibble)
  { 
    return (pgm_read_byte(&symbols[nibble & 0xf]));
  };

  /**
   * @override VWI::Codec
   * Returns 4-bit data for given block 5-bit symbol.
   * @param[in] symbol to decode.
   * @return 4-bit data.
   */
  virtual uint8_t decode4(uint8_t symbol)
  {
    return (pgm_read_byte(&codes[symbol & 0x1f]));
  }
};

#endif
