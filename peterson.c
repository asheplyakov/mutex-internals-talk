#include "peterson.h"
#include "compiler.h"

static inline void memory_barrier(void) {
#ifndef SKIP_MFENCE
#if defined(__x86_64__) || defined(__i386__)
	asm volatile("mfence" ::: "memory");
#elif defined(__aarch64__)
	asm volatile("dmb ish" ::: "memory");
#else
#error memory barrier not defined for this architecture
#endif
#endif
}


NOINLINE void peterson_lock(struct peterson_spinlock *lk, int id) {
	int other = 1 - id;
	lk->wants_to_enter[id] = 1;
	lk->looser = id;

	memory_barrier();

	while (lk->wants_to_enter[other] && (lk->looser == id)) { /* wait */ }
}

NOINLINE void peterson_unlock(struct peterson_spinlock *lk, int id) {
	lk->wants_to_enter[id] = 0;
}
