#pragma once

static inline void memory_barrier(void) {
#if defined(__x86_64__) || defined(__i386__)
	asm volatile("mfence" ::: "memory");
#elif defined(__aarch64__)
	asm volatile("dmb ish" ::: "memory");
#else
#error memory barrier not defined for this architecture
#endif
}
