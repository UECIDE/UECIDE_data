/**
 * @file Cosa/Board/TinyX61.hh
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

#ifndef __COSA_BOARD_TINYX61_HH__
#define __COSA_BOARD_TINYX61_HH__

/**
 * Cosa TINYX61 Board pin symbol definitions for the ATtinyX61
 * processors. Cosa does not use pin numbers as Arduino/Wiring,
 * instead strong data type is used (enum types) for the specific pin
 * classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers are only symbolically mapped, i.e. a pin
 * number/digit will not work, symbols must be used, Board::D2.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 */
class Board {
  friend class Pin;
private:
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  Board() {}

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (pin < 8 ? &PINA : &PINB);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    return (pin < 8  ? &PCMSK0 : &PCMSK1);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin < 8 ? pin : pin - 8);
  }
  
public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,
    D14,
    D15,
    LED = D5
  } __attribute__((packed));

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5, 
    A6,
    A7,
    A8,
    A9,
    A10
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D9,
    PWM1 = D11,
    PWM2 = D13
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D14,
    EXT1 = D2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,
    PCI1 = D1,
    PCI2 = D2,
    PCI3 = D3,
    PCI4 = D4,
    PCI5 = D5,
    PCI6 = D6,
    PCI7 = D7,
    PCI8 = D8,
    PCI9 = D9,
    PCI10 = D10,
    PCI11 = D11,
    PCI12 = D12,
    PCI13 = D13,
    PCI14 = D14,
    PCI15 = D15
  } __attribute__((packed));

  /**
   * Pins used for TWI interface.
   */
  enum TWIPin {
    SDA = D8,
    SCL = D10
  } __attribute__((packed));
  
  /**
   * Pins used for SPI interface (in Port A)
   */
  enum SPIPin {
    MOSI = 1,
    MISO = 0,
    SCK = 2,
    SS = 3
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    EXT_MAX = 2,
    PCINT_MAX = 2,
    PIN_MAX = D15
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define ANALOG_COMP_vect ANA_COMP_vect
#define PCINT0_vect PCINT_vect
#define ACSR ACSRB
#define WGM01 WGM00
#define TIMSK0 TIMSK
#define TIMSK1 TIMSK
#define TCNT0 TCNT0L
#define TIFR0 TIFR
#define WDTCSR WDTCR

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void INT1_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void TIMER0_CAPT_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USI_START_vect(void) __attribute__ ((signal));
  void USI_OVF_vect(void) __attribute__ ((signal));
}
#endif
