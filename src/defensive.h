/**   $Id: defensive.h 48 2009-10-10 00:51:03Z mimo $   **/
#ifndef DEFENSIVE_H
#define DEFENSIVE_H

#include <iostream>

#define D(a) std::cout << #a "=[" << a << "]" << std::endl;

#ifdef DEBUG
#define tracer if (0) ; else std::cout
#else
#define tracer if (1) ; else std::cout
#endif


#endif
