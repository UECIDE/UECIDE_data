/**
 * @file Cosa/LCD/Driver/HD44780.hh
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

#ifndef __COSA_LCD_DRIVER_HD44780_HH__
#define __COSA_LCD_DRIVER_HD44780_HH__

#include "Cosa/TWI/Driver/PCF8574.hh"
#include "Cosa/SPI.hh"
#include "Cosa/LCD.hh"
#include "Cosa/Pins.hh"

/**
 * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
 * for LCD/IOStream access. Binding to trace, etc. Supports simple text 
 * scroll, cursor, and handling of special characters such as carriage-
 * return, form-feed, back-space, horizontal tab and new-line. 
 *
 * @section See Also
 * For furter details see Product Specification, Hitachi, HD4478U,
 * ADE-207-272(Z), '99.9, Rev. 0.0.
 */
class HD44780 : public LCD::Device {
protected:
  /**
   * Abstract HD44780 LCD IO adapter to isolate communication specific
   * functions and allow access over parallel and serial interfaces;
   * Ports, SR and I2C/TWI.
   */
  class IO {
  public:
    /**
     * @override HD44780::IO
     * Initiate IO port. Called by HD44780::begin(). Should return true(1)
     * for 8-bit mode otherwise false for 4-bit mode.
     * @return bool.
     */
    virtual bool setup() = 0;

    /**
     * @override HD44780::IO
     * Write LSB nibble (4bit) to display.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data) = 0;

    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(void* buf, size_t size);
    
    /**
     * @override HD44780::IO
     * Set data/command mode; zero(0) for command, 
     * non-zero(1) for data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag) = 0;

    /**
     * @override HD44780::IO
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag) = 0;
  };

  /**
   * Bus Timing Characteristics (in micro-seconds), fig. 25, pp. 50
   */
  static const uint16_t LONG_EXEC_TIME = 1600;
  static const uint16_t POWER_ON_TIME = 48;
  static const uint16_t INIT0_TIME = 4500;
  static const uint16_t INIT1_TIME = 150;

  /**
   * Instructions (Table 6, pp. 24), RS(0), RW(0)
   */
  enum {
    CLEAR_DISPLAY = 0x01,    	// Clears entrire display and return home
    RETURN_HOME = 0x02,	     	// Sets DDRAM 0 in address counter
    ENTRY_MODE_SET = 0x04,	// Sets cursor move direction and display shift
    CONTROL_SET = 0x08,	 	// Set display, cursor and blinking controls
    SHIFT_SET = 0x10,		// Set cursor and shifts display 
    FUNCTION_SET = 0x20,	// Sets interface data length, line and font.
    SET_CGRAM_ADDR = 0x40,	// Sets CGRAM address
    SET_CGRAM_MASK = 0x3f,	// - Mask (6-bit)
    SET_DDRAM_ADDR = 0x80,	// Sets DDRAM address
    SET_DDRAM_MASK = 0x7f,	// - Mask (7-bit)
    BIAS_RESISTOR_SET = 0x04,	// Bias resistor select
    BIAS_RESISTOR_MASK = 0x03,	// - Mask (2-bit)
    COM_SEG_SET = 0x40,		// COM SEG direction select
    COM_SET_MASK = 0x0f,	// - mask (4 bit)
    SET_DDATA_LENGTH = 0x80,	// Set display data length
    SET_DDATA_MASK = 0x7f	// - mask (7 bit, 0..79 => 1..80)
  } __attribute__((packed));

  /**
   * ENTRY_MODE_SET attributes
   */
  enum { 
    DISPLAY_SHIFT = 0x01,	// Shift the entire display not cursor
    INCREMENT = 0x02,		// Increment (right) on write
    DECREMENT = 0x00		// Decrement (left) on write
  } __attribute__((packed));

  /**
   * CONTROL_SET attributes
   */
  enum {
    BLINK_ON = 0x01,		// The character indicated by cursor blinks
    CURSOR_ON = 0x02,		// The cursor is displayed
    DISPLAY_ON = 0x04,		// The display is on
  } __attribute__((packed));

  /**
   * SHIFT_SET attributes
   */
  enum {
    MOVE_LEFT = 0x00,		// Moves cursor and shifts display
    MOVE_RIGHT = 0x04,		// without changing DDRAM contents
    CURSOR_MODE = 0x00,
    DISPLAY_MOVE = 0x08
  } __attribute__((packed));

  /**
   * FUNCTION_SET attributes
   */
  enum {
    DATA_LENGTH_4BITS = 0x00,	// Sets the interface data length, 4-bit or
    DATA_LENGTH_8BITS = 0x10,	// - 8-bit
    NR_LINES_1 = 0x00,		// Sets the number of display lines, 1 or
    NR_LINES_2 = 0x08,		// - 2.
    FONT_5X8DOTS = 0x00,	// Sets the character font, 5X8 dots or
    FONT_5X10DOTS = 0x04,	// - 5X10 dots
    BASIC_SET = 0x00,		// Sets basic instruction set
    EXTENDED_SET = 0x04		// - extended instruction set
  } __attribute__((packed));

  // Row offset tables for display dimensions (16X1, 16X2, 16X4, 20X4)
  static const uint8_t offset0[] PROGMEM;
  static const uint8_t offset1[] PROGMEM;

  // Display pins and state (mirror of device registers)
  IO* m_io;			// IO port handler
  uint8_t m_mode;		// Entry mode
  uint8_t m_cntl;		// Control
  uint8_t m_func;		// Function set
  const uint8_t* m_offset;	// Row offset table
  
  /**
   * @override
   * Write data or command to display.
   * @param[in] data to write.
   */
  void write(uint8_t data)
  {
    m_io->write8b(data);
  }

  /**
   * Set display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void set(uint8_t& cmd, uint8_t mask) 
  { 
    m_io->write8b(cmd |= mask); 
  }

  /**
   * Clear display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void clear(uint8_t& cmd, uint8_t mask) 
  { 
    m_io->write8b(cmd &= ~mask); 
  }

  /**
   * Set communication in data stream mode.
   */
  void set_data_mode()
  {
    m_io->set_mode(1);
  }

  /**
   * Set communication in instruction stream mode.
   */
  void set_instruction_mode()
  {
    m_io->set_mode(0);
  }

public:
  // Max size of custom character font bitmap
  static const uint8_t BITMAP_MAX = 8;
  
  // Display width (characters per line) and height (lines)
  const uint8_t WIDTH;
  const uint8_t HEIGHT;

  /**
   * Construct HD44780 LCD connected to given io port handler. The
   * display is initiated when calling begin(). 
   * @param[in] io handler.
   * @param[in] width of display, characters per line (Default 16).
   * @param[in] height of display, number of lines (Default 2).
   */
  HD44780(IO* io, uint8_t width = 16, uint8_t height = 2) :
    LCD::Device(),
    m_io(io),
    m_mode(ENTRY_MODE_SET | INCREMENT),
    m_cntl(CONTROL_SET),
    m_func(FUNCTION_SET | DATA_LENGTH_4BITS | NR_LINES_2 | FONT_5X8DOTS),
    m_offset((height == 4) && (width == 16) ? offset1 : offset0),
    WIDTH(width),
    HEIGHT(height)
  {
  }
  
  /**
   * @override LCD::Device
   * Start display for text output. Returns true if successful
   * otherwise false.
   * @return boolean.
   */
  virtual bool begin();

  /**
   * @override LCD::Device
   * Stop display and power down. Returns true if successful 
   * otherwise false.
   */
  virtual bool end();

  /**
   * @override LCD::Device
   * Turn display backlight on. 
   */
  virtual void backlight_on();

  /**
   * @override LCD::Device
   * Turn display backlight off. 
   */
  virtual void backlight_off();

  /**
   * @override LCD::Device
   * Turn display on. 
   */
  virtual void display_on();

  /**
   * @override LCD::Device
   * Turn display off. 
   */
  virtual void display_off();

  /**
   * Set display scrolling left.
   */
  void display_scroll_left() 
  { 
    write(SHIFT_SET | DISPLAY_MOVE | MOVE_LEFT); 
  }

  /**
   * Set display scrolling right.
   */
  void display_scroll_right() 
  { 
    write(SHIFT_SET | DISPLAY_MOVE |  MOVE_RIGHT); 
  }
  
  /**
   * @override LCD::Device
   * Clear display and move cursor to home(0, 0).
   */
  virtual void display_clear();

  /**
   * Move cursor to home position(0, 0) .
   */
  void cursor_home();

  /**
   * Turn underline cursor on.
   */
  void cursor_underline_on() 
  { 
    set(m_cntl, CURSOR_ON);  
  }

  /**
   * Turn underline cursor off.
   */
  void cursor_underline_off() 
  { 
    clear(m_cntl, CURSOR_ON);  
  }

  /**
   * Turn cursor blink on.
   */
  void cursor_blink_on() 
  { 
    set(m_cntl, BLINK_ON); 
  }

  /**
   * Turn cursor blink off.
   */
  void cursor_blink_off() 
  { 
    clear(m_cntl, BLINK_ON); 
  }

  /**
   * @override LCD::Device
   * Set cursor position to given position.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

  /**
   * Set text flow left-to-right.
   */
  void text_flow_left_to_right() 
  { 
    set(m_mode, INCREMENT);
  }

  /**
   * Set text flow right-to-left.
   */
  void text_flow_right_to_left() 
  { 
    clear(m_mode, INCREMENT); 
  }

  /**
   * Set text scroll left adjust.
   */
  void text_scroll_left_adjust() 
  { 
    set(m_mode, DISPLAY_SHIFT); 
  }

  /**
   * Set text scroll right adjust.
   */
  void text_scroll_right_adjust() 
  { 
    clear(m_mode, DISPLAY_SHIFT); 
  }

  /**
   * Set custom character bitmap for given identity (0..7). 
   * @param[in] id character.
   * @param[in] bitmap pointer to bitmap.
   */
  void set_custom_char(uint8_t id, const uint8_t* bitmap);

  /**
   * Set custom character bitmap to given identity (0..7). 
   * The bitmap should be stored in program memory.
   * @param[in] id character.
   * @param[in] bitmap pointer to program memory bitmap.
   */
  void set_custom_char_P(uint8_t id, const uint8_t* bitmap);

  /**
   * @override IOStream::Device
   * Write character to display. Handles carriage-return-line-feed, back-
   * space, alert, horizontal tab and form-feed. Returns character or EOF 
   * on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override IOStream::Device
   * Write data from buffer with given size to device.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write(void* buf, size_t size);

#if !defined(__ARDUINO_TINYX5__)
  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * IO Port. Arduino pins directly to LCD in 4-bit mode. Data port is 
   * implicitly defined (D4..D7/Arduino, D10..D13/Mega).
   *
   * @section Circuit
   *   D4..D7 (Arduino), D0..D3 (Tiny), D10..D13 (Mega) => LCD:D4..D7
   *   D8 (Arduino) => LCD:RS
   *   D9 (Arduino) => LCD:EN
   *   D10 (Arduino) => BT
   *
   * @section Limitations
   * Requires too many pins for ATtinyX5.
   */
  class Port4b : public IO {
  protected:
    /** Execution time delay (us) */
    static const uint16_t SHORT_EXEC_TIME = 34;

    OutputPin m_rs;		/**< Register select (0/instruction, 1/data) */
    OutputPin m_en;		/**< Starts data read/write */
    OutputPin m_bt;		/**< Back-light control (0/on, 1/off) */
    
  public:
    /**
     * Construct HD44780 4-bit parallel port connected to given command,
     * enable and backlight pin. Data pins are implicit; D4..D7 for Arduino
     * Standard and Mighty. D10..D13 for Arduino/Mega. D0..D3 for ATtinyX4.
     * Connect to LCD pins D4..D7.  
     * @param[in] rs command/data select pin (Default D8).
     * @param[in] en enable pin (Default D9).
     * @param[in] bt backlight pin (Default D10).
     */
    Port4b(Board::DigitalPin rs = Board::D8, 
	   Board::DigitalPin en = Board::D9,
	   Board::DigitalPin bt = Board::D10) :
      m_rs(rs, 0),
      m_en(en, 0),
      m_bt(bt, 1)
    {
    }

    /**
     * @override HD44780::IO
     * Initiate 4-bit parallel port (D4..D7). Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write LSB nibble to display using parallel port (D4..D7).
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };
#endif

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 3-Wire Port (SR3W), 74HC595 (SR[pin]), with digital 
   * output pins.
   *
   * @section Circuit
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D5)------------1-|Q1    VCC|-16-+
   * (LCD D6)------------2-|Q2     Q0|-15-----------(LCD D4)
   * (LCD D7)------------3-|Q3    SER|-14-----------(SDA)
   * (LCD RS)------------4-|Q4    /OE|-13-----------(GND)
   * (LCD BT)------------5-|Q5   RCLK|-12-----------(EN)
   *                     6-|Q6   SCLK|-11-----------(SCL)
   *                     7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|--9
   *                   |   +---------+
   *                   |      0.1uF          
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN)
   * (LCD RW)---------------------------------------(GND)
   *
   *   SDA (Arduino:D7/Tiny:D1) => SR:SER[14]
   *   SCL (Arduino:D6/Tiny:D2) => SR:SCLK[11]
   *   EN (Arduino:D5/Tiny:D3) => SR:RCLK[12]
   *   VCC => SR:/MR[10]
   *   GND => SR:/OE[13]
   *
   *   SR:Q0..Q3[15,1..3] => LCD:D4..D7
   *   SR:Q4[4] => LCD:RS
   *   SR:Q5[5] => LCD:BT (Backlight)
   *   EN (Arduino:D5/Tiny:D3) => LCD:EN
   *   GND => LCD:RW
   *
   * @section Performance
   * The LSB of the shift register is used to allow reduction
   * of number of shift operations (i.e. 6-bit shift). 
   */
  class SR3W : public IO {
  private:
    /** Execution time delay (us) */
    static const uint16_t SHORT_EXEC_TIME = 8;

    /** Shift register port bit fields; little endian */
    union port_t {
      uint8_t as_uint8;		/**< Unsigned byte access */
      struct {
	uint8_t data:4;		/**< Data port (Q0..Q3) */
	uint8_t rs:1;		/**< Command/Data select (Q4) */
	uint8_t bt:1;		/**< Back-light control (Q5) */
	uint8_t app2:1;		/**< Application bit#2 (Q6) */
	uint8_t app1:1;		/**< Application bit#1 (Q7) */
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		/**< Port setting */
    OutputPin m_sda;		/**< Serial data output */
    OutputPin m_scl;		/**< Serial clock */
    OutputPin m_en;		/**< Starts data read/write */
    
  public:
    /**
     * Construct HD44780 3-wire serial port connected to given serial
     * data, clock and enable pulse pin. 
     * @param[in] sda serial data pin (Default D7, Tiny/D1)
     * @param[in] scl serial clock pin (Default D6, Tiny/D2)
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     */
#if !defined(__ARDUINO_TINY__)
    SR3W(Board::DigitalPin sda = Board::D7, 
	 Board::DigitalPin scl = Board::D6,
	 Board::DigitalPin en = Board::D5) :
      m_port(),
      m_sda(sda),
      m_scl(scl),
      m_en(en)
    {
    }
#else
    SR3W(Board::DigitalPin sda = Board::D1, 
	 Board::DigitalPin scl = Board::D2,
	 Board::DigitalPin en = Board::D3) :
      m_port(),
      m_sda(sda),
      m_scl(scl),
      m_en(en)
    {
    }
#endif

    /**
     * @override HD44780::IO
     * Initiate serial port. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 3-Wire Port using SPI (SR3WSPI), 74HC595 (SR[pin]).
   * The enable pulse pin acts as the SPI chip select.
   *
   * @section Circuit
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D5)------------1-|Q1    VCC|-16-+
   * (LCD D6)------------2-|Q2     Q0|-15-----------(LCD D4)
   * (LCD D7)------------3-|Q3    SER|-14-----------(MISO)
   * (LCD RS)------------4-|Q4    /OE|-13-----------(GND)
   * (LCD BT)------------5-|Q5   RCLK|-12-----------(EN)
   *                     6-|Q6   SCLK|-11-----------(SCK)
   *                     7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|--9
   *                   |   +---------+
   *                   |      0.1uF          
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN)
   * (LCD RW)---------------------------------------(GND)
   *
   *   MOSI (Arduino:D11/TinyX4:D5/TinyX5:D0) => SR:SER[14]
   *   SCK (Arduino:D13/TinyX4:D5/TinyX5:D2) => SR:SCLK[11]
   *   EN (Arduino:D5/Tiny:D3) => SR:RCLK[12]
   *   VCC => SR:/MR[10]
   *
   *   SR:Q0..Q3[15,1..3] => LCD:D4..D7
   *   SR:Q4[4] => LCD:RS
   *   SR:Q5[5] => LCD:BT (Backlight)
   *   EN (Arduino:D5/Tiny:D3) => LCD:EN
   *   GND => LCD:RW
   *
   * @section Performance
   * The SPI transfer is so fast that a longer delay is required.
   */
  class SR3WSPI : public IO, public SPI::Driver {
  private:
    /** Execution time delay (us) */
    static const uint16_t SHORT_EXEC_TIME = 20;

    /** Shift register port bit fields; little endian */
    union port_t {
      uint8_t as_uint8;		/**< Unsigned byte access */
      struct {
	uint8_t data:4;		/**< Data port (Q0..Q3) */
	uint8_t rs:1;		/**< Command/Data select (Q4) */
	uint8_t bt:1;		/**< Back-light control (Q5) */
	uint8_t app2:1;		/**< Application bit#2 (Q6) */
	uint8_t app1:1;		/**< Application bit#1 (Q7) */
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		/**< Port setting */
    
  public:
    /**
     * Construct HD44780 4-wire serial port connected to given enable 
     * and chip select pin. Uses the SPI::MOSI(D11) and SPI:SCK(D13) pins.
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     */
#if !defined(__ARDUINO_TINY__)
    SR3WSPI(Board::DigitalPin en = Board::D5) : 
#else
    SR3WSPI(Board::DigitalPin en = Board::D3) : 
#endif
      SPI::Driver(en, 2),
      m_port()
    {
    }

    /**
     * @override HD44780::IO
     * Initiate serial port. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 4-Wire/8-bit Port, 74HC595 (SR[pin]), with digital 
   * output pins.
   *
   * @section Circuit
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D1)------------1-|Q1    VCC|-16-+
   * (LCD D2)------------2-|Q2     Q0|-15-----------(LCD D0)
   * (LCD D3)------------3-|Q3    /OE|-13-----------(GND)  
   * (LCD D4)------------4-|Q4    SER|-14-----------(SDA)
   * (LCD D5)------------5-|Q5   RCLK|-12-----------(EN)
   * (LCD D6)------------6-|Q6   SCLK|-11-----------(SCL)
   * (LCD D7)------------7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|-9
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   * (LCD RS)---------------------------------------(SDA)
   * (LCD EN)---------------------------------------(EN)
   * (LCD BT)---------------------------------------(BT)
   * (LCD RW)---------------------------------------(GND)
   *
   *   SDA (Arduino:D7/Tiny:D1) => SR:SER[14]
   *   SCL (Arduino:D6/Tiny:D2) => SR:SCLK[11]
   *   EN (Arduino:D5/Tiny:D3) => SR:RCLK[12] 
   *   VCC => SR:/MR[10]
   *   GND => SR:/OE[13]
   *
   *   SR:Q0..Q7[15,1..7] => LCD:D0..D7
   *   SDA (Arduino:D7) => LCD::RS
   *   EN (Arduino:D5/Tiny:D3) => LCD:EN
   *   BT (Arduino:D4) => LCD:BT (Backlight)
   *   GND => LCD:RW
   *
   * @section Performance
   * Delay required even when using Cosa serial write. No
   * need for SPI. SCL/SDA can still be connected to other
   * inputs. The Backlight control pin (BT) can be removed
   * if always on.
   *
   * @section Acknowledgement
   * Inspired by AVR2LCD, a solution by Frank Henriquez. The original
   * 74HC164 based design is by Stefan Heinzmann and Marc Simons.  
   * http://frank.bol.ucla.edu/avr2lcd.htm
   * http://web.archive.org/web/20100210142839/
   * http://home.iae.nl/users/pouweha/lcd/lcd_examp.shtml#_3
   */
  class SR4W : public IO {
  private:
    /** Execution time delay (us) */
    static const uint16_t SHORT_EXEC_TIME = 8;

    OutputPin m_sda;		/**< Serial data output */
    OutputPin m_scl;		/**< Serial clock */
    OutputPin m_en;		/**< Starts data read/write */
    OutputPin m_bt;		/**< Backlight control */
    uint8_t m_rs;		/**< Command/Data select */
    
  public:
    /**
     * Construct HD44780 4-wire/8-bit serial port connected to given 
     * data, clock, enable and backlight control pins.
     * @param[in] sda serial data pin (Default D7, Tiny/D1)
     * @param[in] scl serial clock pin (Default D6, Tiny/D2)
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     * @param[in] bt backlight control (Default D4, Tiny/D4)
     */
#if !defined(__ARDUINO_TINY__)
    SR4W(Board::DigitalPin sda = Board::D7, 
	 Board::DigitalPin scl = Board::D6,
	 Board::DigitalPin en = Board::D5,
	 Board::DigitalPin bt = Board::D4) :
      m_sda(sda),
      m_scl(scl),
      m_en(en),
      m_bt(bt, 1),
      m_rs(0)
    {
    }
#else
    SR4W(Board::DigitalPin sda = Board::D1, 
	 Board::DigitalPin scl = Board::D2,
	 Board::DigitalPin en = Board::D3,
	 Board::DigitalPin bt = Board::D4) :
      m_sda(sda),
      m_scl(scl),
      m_en(en),
      m_bt(bt, 1),
      m_rs(0)
    {
    }
#endif

    /**
     * @override HD44780::IO
     * Initiate port for 8-bit serial mode. Returns true(1).
     * @return true(1).
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(void* buf, size_t size);
    
    /**
     * @override HD44780::IO
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
   * Controller/Driver when using the MJKDZ IO expander board based on
   * PCF8574 I2C IO expander device driver. 
   */
  class MJKDZ : public IO, private PCF8574 {
  private:
    // Max size of temporary buffer for TWI message (8 encoded bytes)
    static const uint8_t TMP_MAX = 32;
    
    /** Expander port bit fields; little endian */
    union port_t {
      uint8_t as_uint8;		/**< Unsigned byte access */
      struct {
	uint8_t data:4;		/**< Data port (P0..P3) */
	uint8_t en:1;		/**< Enable pulse (P4) */
	uint8_t rw:1;		/**< Read/Write (P5) */
	uint8_t rs:1;		/**< Command/Data select (P6) */
	uint8_t bt:1;		/**< Back-light (P7) */
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		/**< Port setting */

  public:
    /**
     * Construct HD44780 IO port handler using the MJKDZ I2C/TWI
     * I/O expander with given sub-address (A0..A2).
     * @param[in] subaddr sub-address (0..7, default 7).
     */
    MJKDZ(uint8_t subaddr = 7) : PCF8574(subaddr), m_port() {}
    
    /**
     * @override HD44780::IO
     * Initiate TWI interface. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write nibble to display using TWI interface.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(void* buf, size_t size);

    /**
     * @override HD44780::IO
     * Set instruction/data mode; zero for instruction, 
     * non-zero for data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
   * Controller/Driver when using the GYIICLCD IO expander board based 
   * on PCF8574 I2C IO expander device driver. Has the same port
   * connection as MJKDZ. The difference is the default TWI
   * sub-address. 
   */
  class GYIICLCD : public MJKDZ {
  public:
    /**
     * Construct HD44780 IO port handler using the GY-IICLCD I2C/TWI
     * I/O expander with given sub-address (A0..A2). 
     * @param[in] subaddr sub-address (0..7, default 0).
     */
    GYIICLCD(uint8_t subaddr = 0) : MJKDZ(subaddr) {}
  };

  /**
   * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
   * Controller/Driver when using the DFRobot IO expander board based 
   * on PCF8574 I2C IO expander device driver. 
   */
  class DFRobot : public IO, private PCF8574 {
  private:
    // Max size of temporary buffer for TWI message
    static const uint8_t TMP_MAX = 32;
    
    /** Expander port bit fields; little endian */
    union port_t {
      uint8_t as_uint8;		/**< Unsigned byte access */
      struct {
	uint8_t rs:1;		/**< Command/Data select (P0) */
	uint8_t rw:1;		/**< Read/Write (P1) */
	uint8_t en:1;		/**< Enable pulse (P2) */
	uint8_t bt:1;		/**< Back-light (P3) */
	uint8_t data:4;		/**< Data port (P4..P7) */
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		/**< Port setting */

  public:
    /**
     * Construct HD44780 IO port handler using the DFRobot I2C/TWI
     * I/O expander with given sub-address (A0..A2).
     * @param[in] subaddr sub-address (0..7, default 7).
     */
    DFRobot(uint8_t subaddr = 7) : PCF8574(subaddr), m_port() {}
    
    /**
     * @override HD44780::IO
     * Initiate TWI interface. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write nibble to display using TWI interface.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(void* buf, size_t size);

    /**
     * @override HD44780::IO
     * Set instruction/data mode; zero for instruction, 
     * non-zero for data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * ERM1602-5 Series, Character Display Module, adapter with digital 
   * output pins.
   *
   * @section Circuit
   *   SDA (Arduino:D7/Tiny:D1) => J2:DI[6]
   *   SCL (Arduino:D6/Tiny:D2) => J2:SCL[5]
   *   EN (Arduino:D5/Tiny:D3) => J2:CS[4]
   *   BT (Arduino:D4/Tiny:D4) => BT
   */
  class ERM1602_5 : public IO {
  private:
    /** Execution time delay (us) */
    static const uint16_t SHORT_EXEC_TIME = 12;

    OutputPin m_sda;		/**< Serial data output */
    OutputPin m_scl;		/**< Serial clock */
    OutputPin m_en;		/**< Starts data read/write */
    OutputPin m_bt;		/**< Back-light control (0/on, 1/off) */
    uint8_t m_rs;		/**< Register select (0/instruction, 1/data) */
    uint8_t m_dirty;		/**< Mark register select change required */
  public:
    /**
     * Construct ERM1602-5 3-wire serial port connected to given serial
     * data, clock, enable pulse and backlight control pin. 
     * @param[in] sda serial data pin (Default D7)
     * @param[in] scl serial clock pin (Default D6)
     * @param[in] en enable pulse (Default D5)
     * @param[in] bt backlight control (Default D4)
     */
#if !defined(__ARDUINO_TINY__)
    ERM1602_5(Board::DigitalPin sda = Board::D7, 
	      Board::DigitalPin scl = Board::D6,
	      Board::DigitalPin en = Board::D5,
	      Board::DigitalPin bt = Board::D4) :
      m_sda(sda),
      m_scl(scl, 1),
      m_en(en, 1),
      m_bt(bt, 1),
      m_rs(0),
      m_dirty(false)
    {
    }
#else
    ERM1602_5(Board::DigitalPin sda = Board::D1, 
	      Board::DigitalPin scl = Board::D2,
	      Board::DigitalPin en = Board::D3,
	      Board::DigitalPin bt = Board::D4) :
      m_sda(sda),
      m_scl(scl, 1),
      m_en(en, 1),
      m_bt(bt, 1),
      m_rs(0),
      m_dirty(false)
    {
    }
#endif

    /**
     * @override HD44780::IO
     * Initiate serial port. Returns true.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override HD44780::IO
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override HD44780::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override HD44780::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(void* buf, size_t size);

    /**
     * @override HD44780::IO
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override HD44780::IO
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };
};
#endif
