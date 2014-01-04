/**
 * @file Cosa/IOStream.hh
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

#ifndef __COSA_IOSTREAM_HH__
#define __COSA_IOSTREAM_HH__

#include "Cosa/Types.h"

/**
 * Basic in-/output stream support class. Requires implementation of
 * Stream::Device and/or Stream::Filter.
 */
class IOStream {
public:
  /** End Of File, returned when device operations fails (empty or full) */
  static const int EOF = -1;

  /**
   * Device for in/output of characters or strings.
   */
  class Device {
  protected:
    static const uint8_t NON_BLOCKING = 255;

    /** Sleep mode if blocking otherwise NON_BLOCKING */
    uint8_t m_mode;

  public:
    /**
     * Default constructor for IOStream devices.
     * @param[in] mode sleep mode (Default SLEEP_MODE_IDLE)
     */
    Device(uint8_t mode = SLEEP_MODE_IDLE) : m_mode(mode) {}

    /**
     * Set non-blocking mode.
     */
    void set_non_blocking()
    {
      m_mode = NON_BLOCKING;
    }

    /**
     * Set blocking mode with given sleep mode.
     * @param[in] mode sleep mode.
     */
    void set_blocking(uint8_t mode)
    {
      m_mode = mode;
    }

    /**
     * @override IOStream::Device
     * Number of bytes available.
     * @return bytes.
     */
    virtual int available();

    /**
     * @override IOStream::Device
     * Number of bytes room.
     * @return bytes.
     */
    virtual int room();

    /**
     * @override IOStream::Device
     * Write character to device.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c);

    /**
     * @override IOStream::Device
     * Write null terminated string to device. Terminating
     * null is not written.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(const char* s);

    /**
     * @override IOStream::Device
     * Write null terminated string from program memory to device.
     * Terminating null is not written.
     * @param[in] s string in program memory to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts_P(const char* s);

    /**
     * @override IOStream::Device
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const void* buf, size_t size);
    
    /**
     * @override IOStream::Device
     * Write data from buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const iovec_t* vec);

    /**
     * @override IOStream::Device
     * Peek at the next character from device.
     * @return character or EOF(-1).
     */
    virtual int peekchar();
    
    /**
     * @override IOStream::Device
     * Peek for the given character in device buffer. Return number 
     * of characters or EOF(-1).
     * @param[in] c character to peek for.
     * @return available or EOF(-1).
     */
    virtual int peekchar(char c);
    
    /**
     * @override IOStream::Device
     * Read character from device.
     * @return character or EOF(-1).
     */
    virtual int getchar();
    
    /**
     * @override IOStream::Device
     * Read string terminated by new-line or until size into given
     * string buffer. Returns pointer to string or NULL if empty line.
     * @param[in] s string buffer to read into.
     * @param[in] count max number of bytes to read.
     * @return string pointer or NULL.
     */
    virtual char* gets(char *s, size_t count);

    /**
     * @override IOStream::Device
     * Read data to given buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size);

    /**
     * @override IOStream::Device
     * Read data to given buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to read into.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(iovec_t* vec);

    /**
     * @override IOStream::Device
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush();

    /**
     * The default implementation of device; null device.
     */
    static Device null;
  };

  /**
   * Filter for device (decorator). Default implementation is a 
   * pass through filter.
   */
  class Filter : public Device {
  protected:
    Device* m_dev;

  public:
    Filter(Device* dev);
    Filter();

    /**
     * @override IOStream::Device
     * Number of bytes available.
     * @return bytes.
     */
    virtual int available()
    {
      return (m_dev->available());
    }

    /**
     * @override IOStream::Device
     * Number of bytes room.
     * @return bytes.
     */
    virtual int room()
    {
      return (m_dev->room());
    }

    /**
     * @override IOStream::Device
     * Write character to device.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c) 
    { 
      return (m_dev->putchar(c)); 
    }

    /**
     * @override IOStream::Device
     * Write null terminated string to device.
     * Terminating character is not written.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(const char* s)
    {
      return (m_dev->puts(s));
    }

    /**
     * @override IOStream::Device
     * Write null terminated string from program memory to device.
     * Terminating character is not written.
     * @param[in] s string in program memory to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts_P(const char* s)
    {
      return (m_dev->puts_P(s));
    }

    /**
     * @override IOStream::Device
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(void* buf, size_t size)
    {
      return (m_dev->write(buf, size));
    }

    /**
     * @override IOStream::Device
     * Write data from buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const iovec_t* vec)
    {
      return (m_dev->write(vec));
    }

    /**
     * @override IOStream::Device
     * Peek at the next character from device.
     * @return character or EOF(-1).
     */
    virtual int peekchar()
    {
      return (m_dev->peekchar());
    }
    
    /**
     * @override IOStream::Device
     * Read character from device.
     * @return character or EOF(-1).
     */
    virtual int getchar()
    {
      return (m_dev->getchar());
    }

    /**
     * @override IOStream::Device
     * Read string terminated by new-line or until size into given
     * string buffer. Returns pointer to string or NULL if empty line.
     * @param[in] s string buffer to read into.
     * @param[in] count max number of bytes to read.
     * @return string pointer or NULL.
     */
    virtual char* gets(char *s, size_t count)
    {
      return (m_dev->gets(s, count));
    }

    /**
     * @override IOStream::Device
     * Read data to buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size)
    {
      return (m_dev->read(buf, size));
    }

    /**
     * @override IOStream::Device
     * Read data to given buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to read into.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(iovec_t* vec)
    {
      return (m_dev->read(vec));
    }

    /**
     * @override IOStream::Device
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush()
    {
      return (m_dev->flush());
    }
  };
  
  /**
   * Base conversion.
   */
  enum Base {
    bcd = 0,
    bin = 2,
    oct = 8,
    dec = 10,
    hex = 16
  } __attribute__((packed));

  /**
   * Construct stream with given device. Default is the null device.
   * @param[in] dev stream device.
   */
  IOStream(Device* dev);
  IOStream();
  
  /**
   * Get current device.
   * @return device.
   */
  Device* get_device() 
  { 
    return (m_dev); 
  }

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   * @return previous device.
   */
  Device* set_device(Device* dev);

  /**
   * Print integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(int value, Base base = dec);

  /**
   * Print long integer 32-bit value in given base torace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(long int value, Base base = dec);

  /**
   * Print unsigned integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned int value, Base base = dec);

  /**
   * Print unsigned long integer 32-bit value in given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned long int value, Base base = dec);

  /**
   * Print buffer contents in given base to stream.
   * @param[in] ptr pointer to data memory.
   * @param[in] size number of bytes.
   * @param[in] base to represent value in (default 10).
   * @param[in] max number of numbers per line (default 16).
   */
  void print(const void *ptr, size_t size, Base base = dec, uint8_t max = 16);

  /**
   * Print pointer as a hexadecimal number to stream.
   * @param[in] ptr pointer to data memory.
   */
  void print(void *ptr) 
  { 
    print((unsigned int) ptr, hex); 
  }

  /**
   * Print pointer to program memory as a hexadecimal number to 
   * stream.
   * @param[in] ptr pointer to program memory.
   */
  void print(const void *ptr) 
  { 
    print((unsigned int) ptr, hex); 
  }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   */
  void print(char c) 
  { 
    m_dev->putchar(c); 
  }

  /**
   * Print string in data memory to stream.
   * @param[in] s pointer to data memory string.
   */
  void print(const char* s) 
  { 
    m_dev->puts(s); 
  }

  /**
   * Print string in program memory to stream. Use macro PSTR() to generate 
   * a string constants in program memory.
   * @param[in] s pointer to program memory string.
   */
  void print_P(const char* s) 
  { 
    m_dev->puts_P(s); 
  }

  /**
   * Print end of line to stream.
   */
  void println() 
  { 
    m_dev->putchar('\n'); 
  }

  /**
   * Format print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] args variable argument list.
   */
  void vprintf_P(const char* format, va_list args);

  /**
   * Formated print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] ... variable argument list.
   */
  void printf_P(const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    vprintf_P(format, args);
    va_end(args);
  }

  /**
   * Print contents of iostream to stream.
   * @param[in] buffer input/output buffer.
   */
  void print(IOStream::Device* buffer);

  /**
   * Scan next token from the input stream. Returns pointer to string
   * or NULL if not stream is empty. 
   * @param[in] s string buffer to read into.
   * @param[in] count max number of bytes to read.
   * @return string pointer or NULL.
   */
  char* scan(char *s, size_t count);
  
  /**
   * Stream manipulator function prototype. To allow implementation
   * of base change and end of line.
   * @param[in] iostream.
   * @return iostream.
   */
  typedef IOStream& (*Manipulator)(IOStream&);

  /**
   * Output operator for stream manipulator. Apply function.
   * @param[in] func iostream manipulator.
   * @return iostream.
   */
  IOStream& operator<<(Manipulator func)
  { 
    return (func(*this)); 
  }

  /**
   * Print integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(int n) 
  { 
    print(n, m_base); 
    m_base = dec;
    return (*this);
  }

  /**
   * Print long integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(long int n)
  { 
    print(n, m_base); 
    m_base = dec;
    return (*this); 
  }

  /**
   * Print unsigned integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(unsigned int n)
  { 
    print(n, m_base); 
    m_base = dec;
    return (*this); 
  }

  /**
   * Print unsigned long integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(unsigned long int n)
  { 
    print(n, m_base); 
    m_base = dec;
    return (*this); 
  }

  /**
   * Print pointer as a hexadecimal number string to stream.
   * @param[in] ptr pointer to print.
   * @return iostream.
   */
  IOStream& operator<<(void* ptr) 
  { 
    print(ptr); 
    return (*this); 
  }

  /**
   * Print program memory pointer as a hexadecimal number 
   * string to stream.
   * @param[in] ptr program memory pointer to print.
   * @return iostream.
   */
  IOStream& operator<<(const void* ptr) 
  { 
    print(ptr); 
    return (*this); 
  }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   * @return iostream.
   */
  IOStream& operator<<(char c) 
  { 
    print(c); 
    return (*this); 
  }

  /**
   * Print null terminated string to stream.
   * @param[in] s string to print.
   * @return iostream.
   */
  IOStream& operator<<(char* s) 
  { 
    print(s); 
    return (*this); 
  }

  /**
   * Print null terminated string in program memory to stream.
   * @param[in] s string in program memory to print.
   * @return iostream.
   */
  IOStream& operator<<(const char* s) 
  { 
    print_P(s); 
    return (*this); 
  }

  /**
   * Print contents of iobuffer to stream.
   * @param[in] buffer input/output buffer.
   * @return iostream.
   */
  IOStream& operator<<(IOStream& buffer) 
  { 
    print(&buffer);
    return (*this); 
  }

  friend IOStream& bcd(IOStream& outs);
  friend IOStream& bin(IOStream& outs);
  friend IOStream& oct(IOStream& outs);
  friend IOStream& dec(IOStream& outs);
  friend IOStream& hex(IOStream& outs);
  friend IOStream& flush(IOStream& outs);

 private:
  Device* m_dev;		/**< IOStream Device */
  Base m_base;			/**< Base for next output operator */

  /**
   * Print number prefix for non decimal base.
   * @param[in] base representation.
   */
  void print_prefix(Base base);
};

/**
 * Set current base to bcd for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
bcd(IOStream& outs)
{
  outs.m_base = IOStream::bcd;
  return (outs);
}

/**
 * Set current base to binary(2) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
bin(IOStream& outs)
{
  outs.m_base = IOStream::bin;
  return (outs);
}

/**
 * Set current base to octal(8) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
oct(IOStream& outs)
{
  outs.m_base = IOStream::oct;
  return (outs);
}

/**
 * Set current base to deciaml(10) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
dec(IOStream& outs)
{
  outs.m_base = IOStream::dec;
  return (outs);
}

/**
 * Set current base to hexadecimal(16) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
inline IOStream& 
hex(IOStream& outs)
{
  outs.m_base = IOStream::hex;
  return (outs);
}

/**
 * Print horizontal tab '\t'.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
tab(IOStream& outs)
{
  outs.print('\t');
  return (outs);
}

/**
 * Print end of line '\n'; carriage-return-line-feed.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
endl(IOStream& outs)
{
  outs.print('\n');
  return (outs);
}

/**
 * Print end of string '\0'; null character
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
ends(IOStream& outs)
{
  outs.print('\0');
  return (outs);
}

/**
 * Print form feed '\f'; new page/clear screen
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
clear(IOStream& outs)
{
  outs.print('\f');
  return (outs);
}

/**
 * Flush buffer to device.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream& 
flush(IOStream& outs)
{
  outs.m_dev->flush();
  return (outs);
}
#endif
