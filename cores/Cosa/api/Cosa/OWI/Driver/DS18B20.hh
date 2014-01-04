/**
 * @file Cosa/OWI/Driver/DS18B20.hh
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

#ifndef __COSA_OWI_DRIVER_DS18B20_HH__
#define __COSA_OWI_DRIVER_DS18B20_HH__

#include "Cosa/OWI.hh"
#include "Cosa/IOStream.hh"

/**
 * Driver for the DS18B20 Programmable Resolution 1-Write Digital
 * Thermometer.  
 * 
 * @section See Also
 * Maxim Integrated product description (REV: 042208) 
 */
class DS18B20 : public OWI::Driver {
private:
  /**
   * DS18B20 Function Commands (Table 3, pp. 12)
   */
  enum {
    FAMILY_CODE = 0x28,
    CONVERT_T = 0x44,
    READ_SCRATCHPAD = 0xBE,
    WRITE_SCRATCHPAD = 0x4E,
    COPY_SCRATCHPAD = 0x48,
    RECALL_E = 0xB8,
    READ_POWER_SUPPLY = 0xB4
  } __attribute__((packed));

  /**
   * DS18B20 Memory Map (Figure 7, pp. 7)
   */
  struct scratchpad_t {
    int16_t temperature;
    int8_t high_trigger;
    int8_t low_trigger;
    uint8_t configuration;
    uint8_t reserved[3];
    uint8_t crc;
  };
  scratchpad_t m_scratchpad;

  /** Size of configuration; high/low trigger and configuration byte */
  static const uint8_t CONFIG_MAX = 3;

  /** Parasite power mode */
  uint8_t m_parasite;

  /** Watchdog millis on convert_request() */
  uint32_t m_start;

  /** Convert request pending */
  uint8_t m_converting;

  /** Max conversion time for 12-bit conversion in milli-seconds */
  static const uint16_t MAX_CONVERSION_TIME = 750;

  /** Min copy configuration time in milli-seconds (parasite mode) */
  static const uint16_t MIN_COPY_PULLUP = 10;

  /**
   * Turn power off if the device is parasite powered. Call
   * connect() or read_power_supply() to set the parasite mode for the
   * device.
   */
  void power_off()
  {
    if (m_parasite) m_pin->power_off();
  }
public:
  /**
   * Alarm search support class.
   */
  class Search : public OWI::Search {
  public:
    /**
     * Construct an alarm search iterator for the thermometer
     * device family.
     * @param[in] owi one-wire pin to search.
     */
    Search(OWI* owi) : OWI::Search(owi, FAMILY_CODE) {}
    
    /**
     * Get the next thermometer with active alarm since latest
     * convert request. The temperature value that triggered the alarm
     * is read.
     * @return pointer to driver or null(0).
     */
    DS18B20* next();
  };

  /**
   * Construct a DS18B20 device connected to the given 1-Wire bus.
   * Use connect() to lookup, set power supply mode and
   * configuration. Alternatively use read_power_supply() and
   * read_scratchpad() directly if rom address is given.
   * @param[in] pin one wire bus pin.
   * @param[in] name of device.
   */
  DS18B20(OWI* pin, const char* name = 0) : 
    OWI::Driver(pin, name),
    m_parasite(0),
    m_start(0L),
    m_converting(false)
  {}

  /**
   * Construct a DS18B20 device connected to the given 1-Wire bus
   * and device identity (in EEPROM). Use connect() to lookup, set
   * power supply mode and configuration. Alternatively use
   * read_power_supply() and read_scratchpad() directly if rom address
   * is given.
   * @param[in] pin one wire bus pin.
   * @param[in] rom device identity (default null).
   */
  DS18B20(OWI* pin, const uint8_t* rom, const char* name = 0) :
    OWI::Driver(pin, rom, name),
    m_parasite(0),
    m_start(0L),
    m_converting(false)
  {}

  /**
   * Connect to DS18B20 device with given index. Reads configuration,
   * scratchpad, and power supply setting.
   * @param[in] index device order.
   * @return true(1) if successful otherwise false(0).
   */
  bool connect(uint8_t index);

  /**
   * Set conversion resolution from 9..12 bits. Use write_scratchpad()
   * and copy_scratchpad() to update device. 
   * @param[in] bits resolution.
   */
  void set_resolution(uint8_t bits);

  /**
   * Set alarm trigger values; high and low threshold values.
   * Use write_scratchpad() and copy_scratchpad() to update device.
   * @param[in] high threshold. 
   * @param[in] low threshold.
   */
  void set_trigger(int8_t high, int8_t low)
  {
    m_scratchpad.high_trigger = high;
    m_scratchpad.low_trigger = low;
  }

  /**
   * Get the latest temperature reading from the local memory scratchpad.
   * Call convert_request() and read_scratchpad() before accessing the
   * scratchpad. Returns at highest resolution a fixed point<12,4>
   * point number. For 11-bit resolution, bit 0 is undefined, 10-bits
   * bit 1 and 0, and so on (LSB).
   * @return temperature
   */
  int16_t get_temperature()
  {
    return (m_scratchpad.temperature);
  }
  
  /**
   * Get conversion resolution. Use connect(), or read_scratchpad() to
   * read values from device before calling this method. 
   * @return number of bits.
   */
  uint8_t get_resolution()
  {
    if (m_rom[0] == 0) return (0);
    return (9 + (m_scratchpad.configuration >> 5));
  }

  /**
   * Get alarm trigger values; high and low threshold values.
   * Use connect(), or read_scratchpad() to read values from device.
   * @param[out] high threshold. 
   * @param[out] low threshold.
   */
  void get_trigger(int8_t& high, int8_t& low)
  {
    high = m_scratchpad.high_trigger;
    low = m_scratchpad.low_trigger;
  }

  /**
   * Initiate a single temperature conversion. With the default
   * setting 12-bit resolution the max conversion time is 750 ms,
   * MAX_CONVERSION_TIME. 
   * @return true(1) if successful otherwise false(0).
   */
  bool convert_request();

  /**
   * Initiate connected devices to perform a simple temperature
   * conversion (boardcast). With the default setting 12-bit
   * resolution the max conversion time is 750 ms,
   * MAX_CONVERSION_TIME. Give zero(0) as resolution to omit
   * conversion wait. Parasite power by passing true for the mode
   * flag. Returns true(1) if successful otherwise false(0).
   * @param[in] owi one-wire interface pin.
   * @param[in] resolution of conversion.
   * @param[in] parasite power mode flag.
   * @return true(1) if successful otherwise false(0).
   */
  static bool convert_request(OWI* owi, 
			      uint8_t resolution = 12, 
			      bool parasite = false);

  /**
   * Write the contents of the scratchpad triggers and configuration
   * (3 bytes) to device. 
   * @return true(1) if successful otherwise false(0).
   */
  bool write_scratchpad();

  /**
   * Read the contents of the scratchpad to local memory. A internal 
   * delay will occur if a convert_request() is pending. The delay is
   * at most max conversion time (750 ms). The flag parameter may be
   * used when a device has been addressed with search (alarm).
   * @param[in] flag to reset before reading (default true).
   * @return true(1) if successful otherwise false(0).
   */
  bool read_scratchpad(bool flag = true);

  /**
   * Copy device scratchpad triggers and configuration data 
   * to device EEPROM. An internal delay is issued to allow the data
   * to be written.
   * @return true(1) if successful otherwise false(0).
   */
  bool copy_scratchpad();

  /**
   * Recall the alarm triggers and configuration from device EEPROM. 
   * @return true(1) if successful otherwise false(0).
   */
  bool recall();

  /**
   * Read power supply. Determine if the device requires parasite 
   * powering. The information is cached and used in read_scratchpad()
   * and copy_scratchpad() for internal delay timing. The function
   * connect() will call this function automatically.
   * @return true(1) if parasite power is required otherwise false(0).
   */
  bool read_power_supply();

  /**
   * Print temperature reading (fixed point value) to given output
   * stream. 
   * @param[in] outs output stream.
   * @param[in] temp temperature fixed point number.
   */
  static void print(IOStream& outs, int16_t temp);

  friend IOStream& operator<<(IOStream& outs, DS18B20& thermometer);
};

/**
 * Print the name of the thermometer and latest temperature reading
 * with two decimals to given output stream. The temperature is in
 * Celcius. 
 * @param[in] outs stream to print device information to.
 * @param[in] thermometer device.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, DS18B20& thermometer);

#endif
