#include "compiler.h"
#include "peterson.h"

NOINLINE void peterson_lock(struct peterson_spinlock *lk, int id) {
	int other = 1 - id;
	lk->wants_to_enter[id] = 1;
	lk->looser = id;

	while (lk->wants_to_enter[other] && (lk->looser == id)) { /* wait */ }
}

NOINLINE void peterson_unlock(struct peterson_spinlock *lk, int id) {
	lk->wants_to_enter[id] = 0;
}
