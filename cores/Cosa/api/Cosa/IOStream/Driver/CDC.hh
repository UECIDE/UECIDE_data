/**
 * @file Cosa/IOStream/Driver/CDC.hh
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

#ifndef __COSA_IOSTREAM_DRIVER_CDC_HH__
#define __COSA_IOSTREAM_DRIVER_CDC_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

#if defined(USBCON)

class CDC : public IOStream::Device {
  friend void USB_GEN_vect(void);

protected:
  /** Input buffering */
  IOStream::Device* m_ibuf;

  /**
   * Common CDC receive interrupt handler.
   */
  void accept();

public:
  // Default buffer size
  static const uint8_t BUFFER_MAX = 64;

  // Note: this is to maintain the same interface as UART
  // Serial formats; DATA + PARITY + STOP
  enum {
    DATA5 = 0,
    DATA6 = _BV(UCSZ00),
    DATA7 = _BV(UCSZ01),
    DATA8 = _BV(UCSZ01) | _BV(UCSZ00),
    DATA9 = _BV(UCSZ02) | _BV(UCSZ01) | _BV(UCSZ00),
    NO_PARITY = 0,
    EVEN_PARITY = _BV(UPM01),
    ODD_PARITY = _BV(UPM01) | _BV(UPM00),
    STOP1 = 0,
    STOP2 = _BV(USBS0)
  } __attribute__((packed));

  /**
   * Construct serial port handler for given usb serial interface. 
   * @param[in] ibuf input stream buffer.
   */
  CDC(IOStream::Device* ibuf) : 
    IOStream::Device(),
    m_ibuf(ibuf)
  {
  }

  /**
   * @override IOStream::Device
   * Number of bytes available in input buffer.
   * @return bytes.
   */
  virtual int available()
  {
    return (m_ibuf->available());
  }

  /**
   * @override IOStream::Device
   * Write character to serial port output buffer.
   * Returns character if successful otherwise on error or buffer full
   * returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override IOStream::Device
   * Peek next character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int peekchar()
  {
    return (m_ibuf->peekchar());
  }

  /**
   * @override IOStream::Device
   * Peek for given character from serial port input buffer.
   * @param[in] c character to peek for.
   * @return available or EOF(-1).
   */
  virtual int peekchar(char c)
  {
    return (m_ibuf->peekchar(c));
  }
    
  /**
   * @override IOStream::Device
   * Read character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int getchar()
  {
    return (m_ibuf->getchar());
  }

  /**
   * @override IOStream::Device
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush();

  /**
   * Start CDC device driver. Note: the parameters are not used. They
   * are added to to maintain the same interface as UART.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default async, 8data, 2stop bit)
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600, uint8_t format = DATA8 + STOP2 + NO_PARITY);

  /**
   * Stop CDC device driver.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (true);
  }
};

/**
 * Default usb serial (cdc). May be redefined by application. Weakly defined.
 */
extern CDC cdc;
#endif
#endif
