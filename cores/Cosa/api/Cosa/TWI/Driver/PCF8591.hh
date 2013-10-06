/**
 * @file Cosa/TWI/Driver/PCF8591.hh
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

#ifndef __COSA_TWI_DRIVER_PCF8591_HH__
#define __COSA_TWI_DRIVER_PCF8591_HH__

#include "Cosa/TWI.hh"

/**
 * Driver for the PCF8591 2-Wire 8-bit A/D and D/A converter.
 *
 * @section See Also
 * Philips Semiconductor product description (Rev. 2003 Jan 27).
 */
class PCF8591 : private TWI::Driver {
private:
  /** Shadow copy of control register */
  uint8_t m_cntl;

public:
  /**
   * Control byte; selection of input channel and mode of operation
   * Fig. 5 Control byte, pp. 6.
   */
  enum {
    AIN0 = 0x00,
    AIN1 = 0x01,
    AIN2 = 0x02,
    AIN3 = 0x03,
    CHANNEL_MASK = 0x03,
    AUTO_INCREMENT = 0x04,
    FOUR_INPUTS = 0x00,
    THREE_DIFF_INPUTS = 0x10,
    TWO_MIXED_INPUTS = 0x20,
    TWO_DIFF_INPUTS = 0x30,
    OUTPUT_ENABLE = 0x40
  } __attribute__((packed));

  /**
   * Construct PCF8591 TWI A/D, D/A converter device access to given
   * chip address.
   * @param[in] addr chip address (0..7)
   */
  PCF8591(uint8_t addr = 0) : 
    TWI::Driver(0x48 | (addr & 0x7)),
    m_cntl(0) 
  {}

  /**
   * Begin a sampling sequence for the channel given by the control
   * parameter. Return true(1) if successful otherwise false(0).
   * @param[in] cntl control byte
   * @return bool
   */
  bool begin(uint8_t cntl);
  
  /**
   * End sampling sequence.
   */
  void end()
  {
    twi.end();
  }
  
  /**
   * Sample the channel defined by the latest begin() call.
   * Return sample value.
   * @return sample
   */
  uint8_t sample()
  {
    uint8_t res;
    twi.read(&res, 1);
    return (res);
  }

  /**
   * Sample the given channel and return converted value.
   * @param[in] cntl control byte
   * @return sample
   */
  uint8_t sample(uint8_t cntl)
  {
    begin(cntl);
    uint8_t res = sample();
    end();
    return (res);
  }

  /**
   * Read a sequence of samples the channel defined by the latest
   * begin() call. 
   * @param[in] buf sample buffer.
   * @param[in] size of sample buffer.
   * @return count or negative error code.
   */
  int sample(uint8_t* buf, uint8_t size)
  {
    return (twi.read(buf, size));
  }

  /**
   * Convert given value to analog output (voltage).
   * Return true(1) if successful otherwise false(0).
   * @param[in] value output value.
   * @return bool
   */
  bool convert(uint8_t value);
};

#endif
