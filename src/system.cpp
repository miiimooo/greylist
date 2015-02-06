/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <signal.h>
#include "./system.h"

volatile sig_atomic_t g_signal = 0;

void signal_handler(int signum) {
  if (g_signal != 0) { //let the current signal finish
    return;
  }
  g_signal = signum;
}
volatile sig_atomic_t & setupSignalHandlers() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction (SIGHUP, &sa, 0) != 0) {
    throw std::runtime_error("Failed to install signal handler SIGHUP");
  }
  if (sigaction (SIGQUIT, &sa, 0) != 0) {
    throw std::runtime_error("Failed to install signal handler SIGQUIT");
  }
  sa.sa_flags = SA_RESTART;
  if (sigaction (SIGUSR2, &sa, 0) != 0) {
    throw std::runtime_error("Failed to install signal handler SIGUSR2");
  }
  return g_signal;
}