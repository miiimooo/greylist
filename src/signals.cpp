//
// C++ Implementation: signals
//
// Description:
//
//
// Author: Michael Moritz <mimo@restoel.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "signals.h"

Event_Handler *Signal_Handler::signal_handlers_[NSIG];
Signal_Handler* Signal_Handler::instance_ = 0;// initialize pointer
Signal_Handler* Signal_Handler::instance() {

  if (instance_ == 0) {
    instance_ = new Signal_Handler; // create sole instance
  }
  return instance_; // address of sole instance
}
Event_Handler *
Signal_Handler::register_handler (int signum,
    Event_Handler *eh) {
  // Copy the <old_eh> from the <signum> slot in
  // the <signal_handlers_> table.
  Event_Handler *old_eh = Signal_Handler::signal_handlers_[signum];

  // Store <eh> into the <signum> slot in the
  // <signal_handlers_> table.
  Signal_Handler::signal_handlers_[signum] = eh;

  // Register the <dispatcher> to handle this
  // <signum>.
  struct sigaction sa;
  sa.sa_handler = Signal_Handler::dispatcher;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction (signum, &sa, 0);

  return old_eh;
}

void Signal_Handler::dispatcher (int signum) {
  // Perform a sanity check...
  if (Signal_Handler::signal_handlers_[signum] != 0) {
    // Dispatch the handler's hook method.
    Signal_Handler::signal_handlers_[signum]->handle_signal (signum);
  }
}

