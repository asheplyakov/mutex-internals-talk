/* BIG RED WARNING: this code is correct but extremely unfair.
 * A simple spinlock based on atomic compare-and-swap instruction.
 * A CPU which have just released a lock owns the cache line and has
 * an advantage to reacquire the lock.
 */
void unfair_enter(int *lock) {
    while (__sync_val_compare_and_swap((volatile int*)lock, 0, 1) != 0) { }
}

void unfair_leave(int *lock) {
    *lock = 0;
}
