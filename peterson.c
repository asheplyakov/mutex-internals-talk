#include "compiler.h"

static volatile int wants_to_enter[2] = { 0, 0 };
static volatile int looser = 0;

NOINLINE void cs_enter(int id) {
	int other = 1 - id;
	wants_to_enter[id] = 1;
	looser = id;
#ifndef SKIP_MFENCE
	asm volatile ("mfence" ::: "memory");
#endif
	while (wants_to_enter[other] && (looser == id)) { /* wait */ }
}

NOINLINE void cs_leave(int id) {
	wants_to_enter[id] = 0;
}
