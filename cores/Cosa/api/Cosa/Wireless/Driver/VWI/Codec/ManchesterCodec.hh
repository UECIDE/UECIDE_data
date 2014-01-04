/**
 * @file Cosa/Wireless/Driver/VWI/Codec/ManchesterCodec.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_WIRELESS_DRIVER_VWI_CODEC_MANCHESTERCODEC_HH__
#define __COSA_WIRELESS_DRIVER_VWI_CODEC_MANCHESTERCODEC_HH__

#include "Cosa/Wireless/Driver/VWI.hh"

/**
 * Manchester Phase 4-to-8 bit codec for the Cosa VWI (Virtual Wire
 * Interface). 
 */
class ManchesterCodec : public VWI::Codec {
private:
  /** Symbol mapping table: 4 to 8 bits */
  static const uint8_t symbols[] PROGMEM;

  /** Message header */
  static const uint8_t preamble[] PROGMEM;
  
public:
  /**
   * Construct Manchester Phase codec with given bits per symbol,
   * start symbol, and preamble size.
   */
  ManchesterCodec() : VWI::Codec(8, 0x5d55, 8) {}
  
  /**
   * @override VWI::Codec
   * Returns pointer to Manchester frame preamble in program memory.
   * @return pointer.
   */
  virtual const uint8_t* get_preamble() 
  { 
    return (preamble); 
  }
  
  /**
   * @override VWI::Codec
   * Returns symbol for given 4-bit data.
   * @param[in] nibble to encode.
   * @return 8-bit Manchester code.
   */
  virtual uint8_t encode4(uint8_t nibble) 
  { 
    return (pgm_read_byte(&symbols[nibble & 0xf]));
  };

  /**
   * @override VWI::Codec
   * Returns 4-bit data for given Manchester symbol.
   * @param[in] symbol to decode.
   * @return 4-bit data.
   */
  virtual uint8_t decode4(uint8_t symbol);
};

#endif
