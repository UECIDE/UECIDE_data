/**
 * @file Cosa/TWI/Driver/DS1307.hh
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

#ifndef __COSA_TWI_DRIVER_DS1307_HH__
#define __COSA_TWI_DRIVER_DS1307_HH__

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Time.hh"

/**
 * Driver for the DS1307, 64 X 8, Serial I2C Real-Time Clock,
 * a low-power, full binary-coded decimal (BCD) clock/calendar plus 
 * 56 bytes of NV SRAM. 
 *
 * For further details see Maxim Integrated product description; 
 * http://datasheets.maximintegrated.com/en/ds/DS1307.pdf
 */
class DS1307 : private TWI::Driver {
public:
  /**
   * The Timekeeper Control Register bitfields (pp. 9)
   */
  union control_t {
    uint8_t as_uint8;		/**< Unsigned byte access */
    struct {
      uint8_t rs:2;		/**< Rate Select */
      uint8_t reserved1:2;	/**< Reserved/1 */
      uint8_t sqwe:1;		/**< Square-Ware Enable */
      uint8_t reserved2:2;	/**< Reserved/2 */
      uint8_t out:1;		/**< Output Control */
    };
  };

  /**
   * Rate Selection (pp. 9)
   */
  enum {
    RS_1_HZ,
    RS_4_096_KHZ,
    RS_8_192_KHZ,
    RS_32_768_KHZ
  } __attribute__((packed));

  /**
   * The Timekeeper Registers (Table 2, pp. 8)
   */
  struct timekeeper_t {
    time_t clock;		/**< Time/Date in BCD */
    control_t control;		/**< Timekeeper control register */
  };

  /** Start of application RAM */
  const static uint8_t RAM_START = sizeof(timekeeper_t);

  /** End of application RAM */
  const static uint8_t RAM_END = 0x37;

  /** Max size of application RAM (56 bytes) */
  const static uint8_t RAM_MAX = RAM_END - RAM_START + 1;

  /** 
   * Construct DS1307 device driver with bus address(0x68).
   */
  DS1307() : TWI::Driver(0x68) {}

  /**
   * Read ram block with the given size into the buffer from the position.
   * Return number of bytes read or negative error code.
   * @param[in] ram buffer to read from ram.
   * @param[in] size number of bytes to read.
   * @param[in] pos address in ram to read from.
   * @return number of bytes or negative error code.
   */
  int read(void* ram, uint8_t size = sizeof(time_t), uint8_t pos = 0);

  /**
   * Write ram block at given position with the contents from buffer.
   * Return number of bytes written or negative error code.
   * @param[in] buf buffer to write to ram.
   * @param[in] size number of bytes to write.
   * @param[in] pos address in ram to read write to.
   * @return number of bytes or negative error code.
   */
  int write(void* ram, uint8_t size = sizeof(time_t), uint8_t pos = 0);

  /**
   * Read current time from real-time clock. Return true(1)
   * if successful otherwise false(0).
   * @param[out] now time structure return value.
   * @return boolean.
   */
  bool get_time(time_t& now) 
  {
    return (read(&now) == sizeof(now));
  }

  /**
   * Set the current time from real-time clock with the given
   * time. Return true(1) if successful otherwise false(0).
   * @param[in] now time structure to set.
   * @return boolean.
   */
  bool set_time(time_t& now)
  {
    return (write(&now) == sizeof(now));
  }
};

#endif
