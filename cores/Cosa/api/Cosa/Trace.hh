/**
 * @file Cosa/Trace.hh
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

#ifndef __COSA_TRACE_HH__
#define __COSA_TRACE_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Basic trace support class. Combind IOStream with UART for trace
 * output.
 */
class Trace : public IOStream {
public:
  /**
   * Construct Trace IOStream object and initiate with null device.
   * Use begin() to set the trace device. The Trace class is actually 
   * a singleton, trace, as the trace macro set depends on the variable.
   */
  Trace() : IOStream() {}

  /**
   * Start trace stream over given iostream device.
   * @param[in] dev iostream device.
   * @param[in] banner trace begin message.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(IOStream::Device* dev, const char* banner = NULL);

  /**
   * Stop trace stream over current device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    set_device(0);
    return (true);
  }

  /**
   * Support function for ASSERT failure. Prints give function name,
   * func, line number and expression that could not be validated 
   * to trace output device before calling exit().
   * @param[in] expr program memory string with expression.
   * @param[in] line number.
   * @param[in] func name of function.
   */
  void fatal_P(const char* expr, int line, const char* func) 
    __attribute__((noreturn));
};

/**
 * Log priorities.
 */
#define	LOG_EMERG	0	/* System is unusable */
#define	LOG_ALERT	1	/* Action must be taken immediately */
#define	LOG_CRIT	2	/* Critical conditions */
#define	LOG_ERR		3	/* Error conditions */
#define	LOG_WARNING	4	/* Warning conditions */
#define	LOG_NOTICE	5	/* Normal but significant condition */
#define	LOG_INFO	6	/* Informational */
#define	LOG_DEBUG	7	/* Debug-level messages */

/**
 * Macros to generate mask for the trace log priorities (LOG_EMERG..
 * LOG_DEBUG) setting of trace_log_mask.
 * LOG_MASK(prio) bit mask corresponding to the given priority.
 * LOG_UPTO(prio) bit mask for all priorities including the given.
 */
#define LOG_MASK(prio) (1 << (prio))
#define LOG_UPTO(prio) (LOG_MASK((prio) + 1) - 1)
extern uint8_t trace_log_mask;

/**
 * Prints given message with current function name and line number
 * to trace device and the program will terminate. The message 
 * string is stored in program memory.
 * @param[in] msg message string to print.
 */
#define FATAL(msg) trace.fatal_P(PSTR(msg), __LINE__, __func__)

#ifndef NDEBUG

/**
 * Support macro to check that an expression is valid. The expression
 * is used as a string and evaluated. If false a message is printed
 * to the trace device and the program will terminate.
 * @param[in] expr expression to validate.
 */
# define ASSERT(expr)							\
  do {									\
    if (!(expr)) FATAL("assert:" #expr);				\
  } while (0)

/**
 * Support macro for trace of a string in program memory.
 * @param[in] str string literal
 */
# define TRACE_PSTR(str) trace.print_P(PSTR(str))

/**
 * Support macro for trace of an expression. The expression
 * is used as a string and evaluated.
 * @param[in] expr expression.
 */
# define TRACE(expr)							\
  do {									\
    trace.print_P(PSTR(#expr " = "));					\
    trace.print(expr);							\
    trace.println();							\
  } while (0)

/**
 * Support macro for trace of a log message with line number and
 * function name prefix.
 * @param[in] msg log message.
 */
# define TRACE_LOG(msg, ...)						\
  trace.printf_P(PSTR("%d:%s:" msg "\n"),				\
		 __LINE__, __func__, __VA_ARGS__)
# define IS_LOG_PRIO(prio) (trace_log_mask & LOG_MASK(prio))
# define EMERG(msg, ...)						\
  if (IS_LOG_PRIO(LOG_EMERG)) TRACE_LOG("emerg:" msg, __VA_ARGS__)
# define ALERT(msg, ...)						\
  if (IS_LOG_PRIO(LOG_ALERT)) TRACE_LOG("alert:" msg, __VA_ARGS__)
# define CRIT(msg, ...)							\
  if (IS_LOG_PRIO(LOG_CRIT)) TRACE_LOG("crit:" msg, __VA_ARGS__)
# define ERR(msg, ...)							\
  if (IS_LOG_PRIO(LOG_ERR)) TRACE_LOG("err:" msg, __VA_ARGS__)
# define WARNING(msg, ...)						\
  if (IS_LOG_PRIO(LOG_WARNING)) TRACE_LOG("warning:" msg, __VA_ARGS__)
# define NOTICE(msg, ...)						\
  if (IS_LOG_PRIO(LOG_NOTICE)) TRACE_LOG("notice:" msg, __VA_ARGS__)
# define INFO(msg, ...)							\
  if (IS_LOG_PRIO(LOG_INFO)) TRACE_LOG("info:" msg, __VA_ARGS__)
# define DEBUG(msg, ...)						\
  if (IS_LOG_PRIO(LOG_DEBUG)) TRACE_LOG("debug:" msg, __VA_ARGS__)
#else
# define ASSERT(expr)
# define TRACE_PSTR(str)
# define TRACE(expr)
# define TRACE_LOG(msg, ...)
# define EMERG(msg, ...)
# define ALERT(msg, ...)
# define CRIT(msg, ...)
# define ERR(msg, ...)
# define WARNING(msg, ...)
# define NOTICE(msg, ...)
# define INFO(msg, ...)
# define DEBUG(msg, ...)
#endif

/**
 * The Trace class singleton. 
 */
extern Trace trace  __attribute__ ((weak));

#endif
