#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct peterson_spinlock {
	volatile int wants_to_enter[2];
	volatile int looser;
};

void peterson_lock(struct peterson_spinlock *lk, int id);
void peterson_unlock(struct peterson_spinlock *lk, int id);

#ifdef __cplusplus
} /* extern "C" */
#endif
