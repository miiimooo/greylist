//
// C++ Interface: signals
//
// Description:
//
//
// Author: Michael Moritz <mimo@restoel.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __SIGNALS_H
#define __SIGNALS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <csignal>

class Event_Handler {
public:
  // Hook method for the signal hook method.
  virtual int handle_signal (int signum) = 0;
  virtual ~Event_Handler() {}
  // ... other hook methods for other types of
  // events such as timers, I/O, and
  // synchronization objects.
};
class Signal_Handler {
public:
// Entry point.
  static Signal_Handler *instance (void);

// Register an event handler <eh> for <signum>
// and return a pointer to any existing <Event_Handler>
// that was previously registered to handle <signum>.
  Event_Handler *register_handler (int signum,
      Event_Handler *eh);

// Remove the <Event_Handler> for <signum>
// by setting the slot in the <signal_handlers_>
// table to NULL.
  int remove_handler (int signum);

private:
// Ensure we're a Singleton.
  Signal_Handler (void) {};

// Singleton pointer.
  static Signal_Handler *instance_;

// Entry point adapter installed into <sigaction>
// (must be a static method or a stand-alone
// extern "C" function).
  static void dispatcher (int signum);

// Table of pointers to concrete <Event_Handler>s
// registered by applications.  NSIG is the number of
// signals defined in </usr/include/sys/signal.h>.
  static Event_Handler *signal_handlers_[NSIG];
};
class Signal {
  int number_;
  Signal(); // Not allowed
public:
  int number() const { return number_; }
  explicit Signal (int signo) : number_ (signo) {}
};

#endif
