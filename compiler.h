#pragma once

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE /* */
#endif
