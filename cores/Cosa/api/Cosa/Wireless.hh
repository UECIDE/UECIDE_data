/**
 * @file Cosa/Wireless.hh
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

#ifndef __COSA_WIRELESS_HH__
#define __COSA_WIRELESS_HH__

#include "Cosa/Types.h"
#include "Cosa/Power.hh"

/**
 * Cosa Common Wireless device driver interface.
 */
class Wireless {
public:
  class Driver {
  public:
    /** 
     * Network address together with port.
     */
    struct addr_t {
      uint8_t device;		/**< device address (LSB) */
      int16_t network;		/**< network address */
      addr_t(int16_t net, uint8_t dev) 
      {
	network = net;
	device = dev;
      }
    };

    /** Broadcast device address */
    static const uint8_t BROADCAST = 0x00;
    
  protected:
    /** Current channel */
    uint8_t m_channel;

    /** Current network and device address */
    addr_t m_addr;

    /** Message available */
    volatile bool m_avail;

    /** Sleep mode on wait */
    uint8_t m_mode;

    /** Latest message destination device address */
    uint8_t m_dest;

  public:
    /**
     * Construct Wireless Driver with given network and device address.
     * @param[in] network address.
     * @param[in] device address.
     */
    Driver(int16_t network, uint8_t device) :
      m_channel(0),
      m_addr(network, device),
      m_avail(false),
      m_mode(SLEEP_MODE_IDLE),
      m_dest(0)
    {}

    /**
     * Get network address. 
     */
    int16_t get_network_address()
    {
      return (m_addr.network);
    }

    /**
     * Get device address. 
     */
    uint8_t get_device_address()
    {
      return (m_addr.device);
    }

    /**
     * Set power sleep mode during wait.
     * @param[in] mode of sleep.
     */
    void set_sleep(uint8_t mode)
    {
      m_mode = mode;
    }

    /**
     * Set network and device address. Do not use the broadcast
     * address(0). Should be used before calling begin().
     * @param[in] net network address.
     * @param[in] dev device address.
     */
    void set_address(int16_t net, uint8_t dev)
    {
      m_addr.network = net;
      m_addr.device = dev;
    }

    /**
     * Set device transmission channel. Should be used before calling
     * begin(). 
     * @param[in] channel.
     */
    void set_channel(uint8_t channel)
    {
      m_channel = channel;
    }

    /**
     * @override Wireless::Driver
     * Start the Wireless device driver. Return true(1) if successful
     * otherwise false(0).
     * @param[in] config configuration vector (default NULL)
     * @return bool
     */
    virtual bool begin(const void* config = NULL) = 0;

    /**
     * @override Wireless::Driver
     * Shut down the device driver. Return true(1) if successful
     * otherwise false(0).
     * @return bool
     */
    virtual bool end()
    {
      return (true);
    }
    
    /**
     * @override Wireless::Driver
     * Set device in power up mode. 
     */
    virtual void powerup() {}

    /**
     * @override Wireless::Driver
     * Set device in power down mode. 
     */
    virtual void powerdown() {}

    /**
     * @override Wireless::Driver
     * Set device in wakeup on radio mode. 
     */
    virtual void wakeup_on_radio() {}

    /**
     * @override Wireless::Driver
     * Return true(1) if a message is available otherwise false(0).
     * @return bool
     */
    virtual bool available()
    {
      return (m_avail);
    }

    /**
     * @override Wireless::Driver
     * Return true(1) if there is room to send on the device otherwise
     * false(0).  
     * @return bool
     */
    virtual bool room()
    {
      return (true);
    }
  
    /**
     * @override Wireless::Driver
     * Send message in given null terminated io vector. Returns number
     * of bytes sent. Returns error code(-1) if number of bytes is
     * greater than PAYLOAD_MAX. Return error code(-2) if fails to set
     * transmit mode. Note that port numbers (128 and higher are
     * reserved for system protocols).
     * @param[in] dest destination network address.
     * @param[in] port device port (or message type).
     * @param[in] vec null termianted io vector.
     * @return number of bytes send or negative error code.
     */
    virtual int send(uint8_t dest, uint8_t port, const iovec_t* vec) = 0;

    /**
     * @override Wireless::Driver
     * Send message in given buffer, with given number of bytes. Returns
     * number of bytes sent. Returns error code(-1) if number of bytes
     * is greater than PAYLOAD_MAX. Return error code(-2) if fails to
     * set transmit mode. Note that port numbers (128 and higher are
     * reserved for system protocols).
     * @param[in] dest destination network address.
     * @param[in] port device port (or message type).
     * @param[in] buf buffer to transmit.
     * @param[in] len number of bytes in buffer.
     * @return number of bytes send or negative error code.
     */
    virtual int send(uint8_t dest, uint8_t port, const void* buf, size_t len)
    {
      iovec_t vec[2];
      iovec_t* vp = vec;
      iovec_arg(vp, buf, len);
      iovec_end(vp);
      return (send(dest, port, vec));
    }

    /**
     * @override Wireless::Driver
     * Broadcast message in given null terminated io vector. Returns
     * number of bytes sent. Returns error code(-1) if number of bytes
     * is greater than PAYLOAD_MAX. Return error code(-2) if fails to
     * set transmit mode. Note that port numbers (128 and higher are
     * reserved for system protocols).
     * @param[in] port device port (or message type).
     * @param[in] vec null termianted io vector.
     * @return number of bytes send or negative error code.
     */
    virtual int broadcast(uint8_t port, const iovec_t* vec)
    {
      return (send(BROADCAST, port, vec));
    }

    /**
     * @override Wireless::Driver
     * Boardcast message in given buffer, with given number of bytes. 
     * Returns number of bytes sent. Returns error code(-1) if number
     * of bytes is greater than PAYLOAD_MAX. Return error code(-2) if
     * fails to set transmit mode. Note that port numbers (128 and
     * higher are reserved for system protocols).
     * @param[in] port device port (or message type).
     * @param[in] buf buffer to transmit.
     * @param[in] len number of bytes in buffer.
     * @return number of bytes send or negative error code.
     */
    virtual int broadcast(uint8_t port, const void* buf, size_t len)
    {
      return (send(BROADCAST, port, buf, len));
    }

    /**
     * @override Wireless::Driver
     * Receive message and store into given buffer with given maximum
     * length. The source network address is returned in the parameter src.
     * Returns error code(-2) if no message is available and/or a
     * timeout occured. Returns error code(-1) if the buffer size if to
     * small for incoming message or if the receiver fifo has overflowed. 
     * Otherwise the actual number of received bytes is returned
     * @param[out] src source network address.
     * @param[out] port device port (or message type).
     * @param[in] buf buffer to store incoming message.
     * @param[in] len maximum number of bytes to receive.
     * @param[in] ms maximum time out period.
     * @return number of bytes received or negative error code.
     */
    virtual int recv(uint8_t& src, uint8_t& port, 
		     void* buf, size_t len, 
		     uint32_t ms = 0L) = 0;

    /**
     * @override Wireless::Driver
     * Return true(1) if the latest received message was a broadcast
     * otherwise false(0). 
     */
    virtual bool is_broadcast()
    {
      return (m_dest == BROADCAST);
    }

    /**
     * @override Wireless::Driver
     * Set output power level in dBm. Default no-operation.
     * @param[in] dBm.
     */
    virtual void set_output_power_level(int8_t dBm) {}

    /**
     * @override Wireless::Driver
     * Return estimated input power level (dBm). Default zero(0).
     */
    virtual int get_input_power_level() 
    {
      return (0);
    }

    /**
     * @override Wireless::Driver
     * Return link quality indicator. Default zero(0).
     */
    int get_link_quality_indicator()
    {
      return (0);
    }
  };
};
#endif
