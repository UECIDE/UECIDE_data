/**
 * @file Rotary.hh
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

#ifndef __COSA_ROTARY_HH__
#define __COSA_ROTARY_HH__

#include "Cosa/Types.h"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/RTC.hh"

/** 
 * Rotary Encoder class with support for dials (normal and
 * accelerated).
 *
 * @section Acknowledgements
 * The Rotary Encoder algorithm is based on an implementation by Ben
 * Buxton. See comment block in Cosa/Rotary.cpp for more details and
 * the blog; 
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 */
class Rotary {
public:
  /**
   * Rotary Encoder using pin change interrupts. Handles half and full
   * cycle detection. Will push an Event::CHANGE_TYPE with the
   * direction of the change.
   */
  class Encoder : public Event::Handler {
  public:
    /**
     * Rotary Encoder turn direction
     */
    enum Direction {
      NONE = 0x00,		// No direction change
      CW = 0x10,		// Clock-wise direction
      CCW = 0x20		// Anti-clock-wise direction
    } __attribute__((packed));
    
    /**
     * Rotary Encoder cycle mode
     */
    enum Mode {
      HALF_CYCLE,
      FULL_CYCLE
    }  __attribute__((packed));

  protected:
    /**
     * Rotary signal pin handler (pin change interrupt). Delegates to
     * Rotary Encoder to process new state.
     */
    class SignalPin : public PinChangeInterrupt {
    private:
      Encoder* m_encoder;

      /**
       * @override Interrupt::Handler
       * Signal pin interrupt handler. Check possible state change and will
       * push Event::CHANGE_TYPE with direction (CW or CCW).
       */
      virtual void on_interrupt(uint16_t arg);
      
    public:
      SignalPin(Board::InterruptPin pin, Encoder* encoder) : 
	PinChangeInterrupt(pin), 
	m_encoder(encoder)
      {}
    };

    // State Transition tables.
    static const uint8_t half_cycle_table[6][4] PROGMEM;
    static const uint8_t full_cycle_table[7][4] PROGMEM;

    // Signal pins, previous state and cycle mode.
    SignalPin m_clk;
    SignalPin m_dt;
    uint8_t m_state;
    Mode m_mode;
    
    /**
     * Detect Rotary Encoder state change. Reads current input pin
     * values and performs a possible state change. Return turn
     * direction or none. 
     * @return direction
     */
    Direction detect();
    
  public:
    /**
     * Create Rotary Encoder with given interrupt pins. Setup must
     * call InterruptPin::begin() to initiate handling of pins.
     * @param[in] clk pin.
     * @param[in] dt pin.
     * @param[in] mode cycle (default FULL_CYCLE).
     */
    Encoder(Board::InterruptPin clk, Board::InterruptPin dt, 
	    Mode mode = FULL_CYCLE) :
      m_clk(clk, this),
      m_dt(dt, this),
      m_state(0),
      m_mode(mode)
    {
      enable();
    }

    /**
     * Get current cycle mode.
     * @return mode.
     */
    Mode get_mode()
    {
      return (m_mode);
    }
    
    /**
     * Set cycle mode.
     * @param[in] mode cycle.
     */
    void set_mode(Mode mode)
    {
      m_mode = mode;
    }

    /**
     * Enable the encoder. 
     */
    void enable()
    {
      m_clk.enable();
      m_dt.enable();
    }

    /**
     * Disable the encoder.
     */
    void disable()
    {
      m_clk.disable();
      m_dt.disable();
    }
  };

  /**
   * Use Rotary Encoder as a simple dial (integer value). Allows a
   * dial within a given number(T) range (min, max) and a given
   * initial value. 
   * @param[in] T value type.
   */
  template<typename T>
  class Dial : public Encoder {
  protected:
    T m_value;
    T m_min;
    T m_max;
    T m_step;

    /**
     * @override Event::Handler
     * Update the dial value on change. The event value is the
     * direction (CW or CCW).
     * @param[in] type the event type.
     * @param[in] value the event value.
     */
    virtual void on_event(uint8_t type, uint16_t value)
    {
      if (value == CW) {
	if (m_value == m_max) return;
	m_value += m_step;
      }
      else {
	if (m_value == m_min) return;
	m_value -= m_step;
      }
      on_change(m_value);
    }
    
  public:
    /**
     * Construct Rotary Dial connected to given interrupt pins with given
     * mode, min, max, initial and step value. The mode is one of the
     * Rotary Encoder modes; HALF_CYCLE or FULL_CYCLE.
     * @param[in] clk interrupt pin.
     * @param[in] dt interrupt pin.
     * @param[in] mode step.
     * @param[in] initial value.
     * @param[in] min value.
     * @param[in] max value.
     * @param[in] step value.
     */
    Dial(Board::InterruptPin clk, Board::InterruptPin dt, Mode mode, 
	 T initial, T min, T max, T step) :
      Encoder(clk, dt, mode),
      m_value(initial),
      m_min(min),
      m_max(max),
      m_step(step)
    {}
    
    /**
     * Return current dial value.
     * @return value.
     */
    T get_value()
    {
      return (m_value);
    }
    
    /**
     * Get current step (increment/decrement).
     * @return step.
     */
    T get_step()
    {
      return (m_step);
    }
    
    /**
     * Set step (increment/decrement).
     * @param[in] step value.
     */
    void set_step(T step)
    {
      m_step = step;
    }

    /**
     * @override Rotary::Dial
     * Default on change function. 
     * @param[in] value.
     */
    virtual void on_change(T value) {}
  };

  /**
   * Use Rotary Encoder as an accelerated dial (integer). Allows a
   * dial within a given number(T) range (min, max) and a given
   * initial value. Two levels of step (increment/decrement) are
   * allowed and selected depending on timing.
   */
  template<typename T, uint32_t THRESHOLD>
  class AcceleratedDial : public Encoder {
  private:
    uint32_t m_latest;
    T m_value;
    T m_min;
    T m_max;
    T m_step;
    T m_steps;

    /**
     * @override Event::Handler
     * Update the accelerated dial value on change. The event value is the
     * direction (CW or CCW). If the time period between events is
     * larger than the threshold a slow step is used otherwise the
     * fast step (steps).
     * @param[in] type the event type.
     * @param[in] direction the event value.
     */
    virtual void on_event(uint8_t type, uint16_t direction)
    {
      uint32_t now = RTC::micros();
      uint32_t diff = now - m_latest;
      m_latest = now;
      if (direction == CW) {
	if (m_value == m_max) return;
	if (diff > THRESHOLD)
	  m_value += m_step;
	else
	  m_value += m_steps;
	if (m_value > m_max) m_value = m_max;
      }
      else {
	if (m_value == m_min) return;
	if (diff > THRESHOLD)
	  m_value -= m_step;
	else
	  m_value -= m_steps;
	if (m_value < m_min) m_value = m_min;
      }
      on_change(m_value);
    }

  public:
    /**
     * Construct Rotary Dial connected to given interrupt pins with given
     * mode, min, max, initial, step on slow turn and steps on fast turn.
     * The mode is one of the Rotary Encoder modes; HALF_CYCLE or FULL_CYCLE.
     * @param[in] clk interrupt pin.
     * @param[in] dt interrupt pin.
     * @param[in] mode step.
     * @param[in] initial value.
     * @param[in] min value.
     * @param[in] max value.
     * @param[in] step value.
     * @param[in] steps value.
     */
    AcceleratedDial(Board::InterruptPin clk, Board::InterruptPin dt, Mode mode,
		    T initial, T min, T max, T step, T steps) :
      Encoder(clk, dt, mode),
      m_latest(0L),
      m_value(initial),
      m_min(min),
      m_max(max),
      m_step(step),
      m_steps(steps)
    {}
    
    /**
     * Return current dial value.
     * @return value.
     */
    T get_value()
    {
      return (m_value);
    }
    
    /**
     * Get current step (slow increment/decrement).
     * @return step.
     */
    T get_step()
    {
      return (m_step);
    }
    
    /**
     * Set step (slow increment/decrement).
     * @param[in] step value.
     */
    void set_step(T step)
    {
      m_step = step;
    }

    /**
     * Get current steps (fast increment/decrement).
     * @return steps.
     */
    T get_steps()
    {
      return (m_steps);
    }
    
    /**
     * Set step (fast increment/decrement).
     * @param[in] steps value.
     */
    void set_steps(T steps)
    {
      m_steps = steps;
    }

    /**
     * @override Rotary::AcceleratedDial
     * Default on change function. 
     * @param[in] value.
     */
    virtual void on_change(T value) {}
  };
};
#endif
