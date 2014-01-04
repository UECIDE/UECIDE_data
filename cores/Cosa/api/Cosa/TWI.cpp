/**
 * @file Cosa/TWI.cpp
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

#include "Cosa/TWI.hh"

#if !defined(__ARDUINO_TINY__)

#include "Cosa/Bits.h"
#include "Cosa/Power.hh"

TWI twi  __attribute__ ((weak));

bool 
TWI::begin(TWI::Driver* dev, Event::Handler* target)
{
  // Check that the driver support is not in use
  if (m_dev != NULL) return (false);
  m_dev = dev;
  // Set up receiver of completion events
  m_target = target;
  synchronized {
    // Enable internal pullup
    bit_mask_set(PORTC, _BV(Board::SDA) | _BV(Board::SCL));
    bit_mask_clear(TWSR, _BV(TWPS0) | _BV(TWPS1));
    // Set clock prescale and bit rate
    TWBR = ((F_CPU / FREQ) - 16) / 2;
    TWCR = IDLE_CMD;
  }
  return (true);
}

bool 
TWI::end()
{
  // Check if an asynchronious read/write was issued 
  if (m_target != NULL) await_completed();
  // Put into idle state
  m_target = NULL;
  m_dev = NULL;
  TWCR = 0;
  return (true);
}

bool
TWI::request(uint8_t op)
{
  // Setup buffer pointers
  m_state = (op == READ_OP) ? MR_STATE : MT_STATE;
  m_addr = (m_dev->m_addr | op);
  m_status = NO_INFO;
  m_next = (uint8_t*) m_vec[0].buf;
  m_last = m_next + m_vec[0].size;
  m_ix = 0;
  m_count = 0;
  // And issue start command
  TWCR = START_CMD;
  return (true);
}

bool
TWI::write_request(void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

bool
TWI::write_request(uint8_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

bool
TWI::write_request(uint16_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = (header >> 8);
  m_header[1] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

bool
TWI::read_request(void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(READ_OP));
}

int
TWI::await_completed(uint8_t mode)
{
  while (m_state > IDLE_STATE) Power::sleep(mode);
  return (m_count);
}

void 
TWI::isr_start(State state, uint8_t ix)
{
  if (ix == NEXT_IX) {
    m_ix += 1;
    ix = m_ix;
  }
  else m_count = 0;
  m_next = (uint8_t*) m_vec[ix].buf;
  m_last = m_next + m_vec[ix].size;
  m_state = state;
}

void 
TWI::isr_stop(State state, uint8_t type)
{
  TWCR = TWI::STOP_CMD;
  loop_until_bit_is_clear(TWCR, TWSTO);
  if (state == TWI::ERROR_STATE) m_count = -1;
  m_state = state;
  if (type != Event::NULL_TYPE && m_target != NULL)
    Event::push(type, m_target, m_count);
}

bool
TWI::isr_write(Command cmd)
{
  if (m_next == m_last) return (false);
  TWDR = *m_next++;
  TWCR = cmd;
  m_count += 1;
  return (true);
}

bool
TWI::isr_read(Command cmd)
{
  if (m_next == m_last) return (false);
  *m_next++ = TWDR;
  m_count += 1;
  if (cmd != 0) TWCR = cmd;
  return (true);
}

ISR(TWI_vect) 
{
  twi.m_status = TWI_STATUS(TWSR);
  switch (twi.m_status) {
    /**
     * Transaction Start Mode
     */
  case TWI::START:
  case TWI::REP_START:
    // Write device address
    TWDR = twi.m_addr;
    TWCR = TWI::DATA_CMD;
    break;
  case TWI::ARB_LOST:
    // Lost arbitration
    TWCR = TWI::IDLE_CMD;
    twi.m_state = TWI::ERROR_STATE;
    twi.m_count = -1;
    break;
    
    /**
     * Master Transmitter Mode
     */
  case TWI::MT_SLA_ACK:
  case TWI::MT_DATA_ACK:
    if (twi.m_next == twi.m_last) twi.isr_start(TWI::MT_STATE, TWI::NEXT_IX);
    if (twi.isr_write(TWI::DATA_CMD)) break;
  case TWI::MT_DATA_NACK: 
    twi.isr_stop(TWI::IDLE_STATE, Event::WRITE_COMPLETED_TYPE);
    break;
  case TWI::MT_SLA_NACK: 
    twi.isr_stop(TWI::ERROR_STATE, Event::ERROR_TYPE);
    break;

    /**
     * Master Receiver Mode
     */
  case TWI::MR_DATA_ACK:
    twi.isr_read();
  case TWI::MR_SLA_ACK:
    TWCR = (twi.m_next < (twi.m_last - 1)) ? TWI::ACK_CMD : TWI::NACK_CMD;
    break; 
  case TWI::MR_DATA_NACK:
    twi.isr_read();
    twi.isr_stop(TWI::IDLE_STATE, Event::READ_COMPLETED_TYPE);
    break;
  case TWI::MR_SLA_NACK: 
    twi.isr_stop(TWI::ERROR_STATE, Event::ERROR_TYPE);
    break;

    /**
     * Slave Transmitter Mode
     */
  case TWI::ST_SLA_ACK:
  case TWI::ST_ARB_LOST_SLA_ACK:
    twi.isr_start(TWI::ST_STATE, TWI::Slave::READ_IX);
  case TWI::ST_DATA_ACK:
    if (twi.isr_write(TWI::ACK_CMD)) break;
    TWCR = TWI::NACK_CMD;
    break;
  case TWI::ST_DATA_NACK:
  case TWI::ST_LAST_DATA:
    TWCR = TWI::ACK_CMD;
    twi.m_state = TWI::IDLE_STATE;
    break;

    /**
     * Slave Receiver Mode
     */
  case TWI::SR_SLA_ACK:
  case TWI::SR_GCALL_ACK:
  case TWI::SR_ARB_LOST_SLA_ACK:
  case TWI::SR_ARB_LOST_GCALL_ACK:
    twi.isr_start(TWI::SR_STATE, TWI::Slave::WRITE_IX);
    TWCR = TWI::ACK_CMD;
    break;
  case TWI::SR_DATA_ACK:
  case TWI::SR_GCALL_DATA_ACK:
    if (twi.isr_read(TWI::ACK_CMD)) break;
  case TWI::SR_DATA_NACK:
  case TWI::SR_GCALL_DATA_NACK:
    TWCR = TWI::NACK_CMD;
    break;
  case TWI::SR_STOP:
    twi.isr_stop(TWI::IDLE_STATE, Event::WRITE_COMPLETED_TYPE);
    TWAR = 0;
    break;

  case TWI::NO_INFO:
    break;

  case TWI::BUS_ERROR: 
    twi.isr_stop(TWI::ERROR_STATE);
    break;
    
  default:     
    TWCR = TWI::IDLE_CMD; 
  }
}

bool 
TWI::Slave::begin()
{
  twi.m_target = this;
  twi.m_dev = this;
  synchronized {
    TWAR = m_addr;
    bit_mask_clear(TWSR, _BV(TWPS0) | _BV(TWPS1));
    TWBR = ((F_CPU / TWI::FREQ) - 16) / 2;
    TWCR = TWI::IDLE_CMD;
  }
  return (true);
}

void 
TWI::Slave::on_event(uint8_t type, uint16_t value)
{
  if (type != Event::WRITE_COMPLETED_TYPE) return;
  void* buf = twi.m_vec[WRITE_IX].buf;
  size_t size = value;
  on_request(buf, size);
  TWAR = twi.m_dev->m_addr;
}

void 
TWI::Slave::set_write_buf(void* buf, size_t size)
{
  twi.m_vec[WRITE_IX].buf = buf;
  twi.m_vec[WRITE_IX].size = size;
}

void 
TWI::Slave::set_read_buf(void* buf, size_t size)
{
  twi.m_vec[READ_IX].buf = buf;
  twi.m_vec[READ_IX].size = size;
}

#endif


