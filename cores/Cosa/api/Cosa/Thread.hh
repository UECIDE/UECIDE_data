/**
 * @file Cosa/Thread.hh
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

#ifndef __COSA_THREAD_HH__
#define __COSA_THREAD_HH__

#include "Cosa/Event.hh"
#include "Cosa/Linkage.hh"

/**
 * Cosa implementation of protothreads; A protothread is a
 * low-overhead mechanism for concurrent programming. Protothreads
 * function as stackless, lightweight threads providing a blocking
 * context using minimal memory per protothread. Cosa/Thread supports
 * event to thread mapping and timers. The size of Cosa/Thread is 
 * 9 bytes (3 bytes for state and continuation, 4 bytes for Link and 
 * 2 bytes for virtual table pointer). 
 *
 * @section Limitations
 * The thread macro set should only be used within the Thread::run()
 * function. 
 *
 * @section Acknowledgements
 * Inspired by research and prototype by Adam Dunkels, Oliver Schmidt,
 * Thiermo Voigt, Muneeb Ali, and the protothread library by Larry
 * Ruane. 
 *
 * @section References
 * [1] Adam Dunkels et al, Protothreads: Simplifying Event-Driven
 * Programming of Memory-Constrained Embedded Systems, SenSys'06,
 * November 1-3, 2006, Boulder, Colorado, USA.<br>
 * [2] Larry Ruane, protothread: An extremely lightweight thread
 * library for GCC, http://code.google.com/p/protothread/<br>
 * [3] http://en.wikipedia.org/wiki/Protothreads<br>
 */
class Thread : public Link {
protected:
  static Head runq;
  uint8_t m_state;
  void* m_ip;

  /**
   * @override Event::Handler
   * The first level event handler. Filters timeout events and
   * run the thread action function.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
  
public:
  /**
   * Thread states.
   */
  enum {
    INITIATED = 0,		// Constructor
    READY,			// In run queue
    WAITING,			// In timer queue
    TIMEOUT,			// Timeout received and running
    RUNNING,			// Dispatched and running
    SLEEPING,			// Detached. Need wakeup call
    TERMINATED = 0xff,		// Removed from all queues
  } __attribute__((packed));

  /**
   * Construct thread, initiate state and continuation. Does not 
   * schedule the thread. This is done with begin().
   */
  Thread() : 
    Link(),
    m_state(INITIATED),
    m_ip(0)
  {}
  
  /**
   * Start the thread. Must be in INITIATED state to be allowed to be 
   * scheduled. Returns false if not in correct state.
   * @return bool true if scheduled otherwise false.
   */
  bool begin()
  {
    if (m_state != INITIATED) return (false);
    schedule(this);
    return (true);
  }
  
  /**
   * End the thread. Mark as terminated and remove from any queue.
   * Use macro THREAD_END() in thread body (run function).
   */
  void end()
  {
    m_state = TERMINATED;
    detach();
  }
  
  /**
   * Get current thread state.
   * @return state.
   */
  uint8_t get_state()
  {
    return (m_state);
  }

  /**
   * Set timer and enqueue thread to receive timeout event.
   * If the timer expires the thread is put in TIMEOUT state.
   * @param[in] ms timeout period.
   */
  void set_timer(uint16_t ms);

  /**
   * Cancel timer and dequeue thread from timer queue.
   */
  void cancel_timer()
  {
    detach();
  }

  /**
   * Check if the timer expired; i.e., the thread is in TIMEOUT
   * state. 
   */
  bool timer_expired()
  {
    return (m_state == TIMEOUT);
  }

  /**
   * @override Thread
   * Thread activity. Must be overridden. Use the thread macro set in
   * the following format:
   * {
   *   THREAD_BEGIN();
   *   while (1) {
   *     ...
   *     THREAD_AWAIT(condition);
   *     ...
   *   }
   *   THREAD_END();
   * }
   * Additional macros are THREAD_YIELD(), THREAD_SLEEP(), THREAD_WAKE(),
   * and THREAD_DELAY().
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void run(uint8_t type = Event::RUN_TYPE, uint16_t value = 0) = 0;

  /**
   * Run threads in the run queue. If given flag is true events will
   * be processes. Returns number of dispatched threads and events.
   * The run queue is only iterated once per call to dispatch to allow
   * user defined outer loop, i.e., arduino loop() function.
   * @param[in] flag process events if non zero.
   * @return number of dispatched threads and events.
   */
  static uint16_t dispatch(bool flag = true);

  /**
   * Add the given thread to the run queue (last). A terminated thread
   * may be restarted. 
   * @param[in] thread to enqueue.
   */
  static void schedule(Thread* thread);
};

/**
 * First statement in the thread body, run(). Last statement should be
 * THREAD_END();
 */
#define THREAD_BEGIN()					\
  if (m_ip != 0) goto *m_ip

/**
 * Yield execution to other threads ane event handlers. Remains in the
 * run queue. 
 */
#define THREAD_YIELD() \
  do {							\
    __label__ next;					\
    m_ip = &&next;					\
    return;						\
  next: ;						\
  } while (0)

/**
 * Yield execution and detach from the run queue. Must be activated
 * with THREAD_WAkE().
 */
#define THREAD_SLEEP()					\
  do {							\
    m_state = SLEEPING;					\
    detach();						\
    THREAD_YIELD();					\
  } while (0)

/**
 * Schedule the given thread if SLEEPING.
 * @param[in] thread to wake.
 */
#define THREAD_WAKE(thread)				\
  do {							\
    if (thread->m_state == SLEEPING)			\
      Thread::schedule(thread);				\
  } while (0)

/**
 * Check if the given condition is true(1). If not the thread will
 * yield. The condition is rechecked when the thread is activated
 * again. 
 * @param[in] condition to evaluate.
 */
#define THREAD_AWAIT(condition)				\
  do {							\
    __label__ next;					\
  next:							\
    if (!(condition)) {					\
      m_ip = &&next;					\
      return;						\
    }							\
  } while (0)

/**
 * Delay the thread for the given ms time period. This is a short form
 * for set_timer() and THREAD_AWAIT(timer_expired());
 * @param[in] ms milli-seconds to delay.
 */
#define THREAD_DELAY(ms)				\
  do {							\
    set_timer(ms);					\
    THREAD_AWAIT(timer_expired());			\
  } while (0)

/**
 * Marks the running thread as TERMINATED and detach from any queue. 
 * Should be the last statement in the thread run() function.
 */
#define THREAD_END()					\
  do {							\
    Thread::end();					\
    return;						\
  } while (0)

#endif

