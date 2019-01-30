/* Mutex implementation based on
 * - compare-and-swap atomic instruction
 * - Linux' futex system call
 */
#include <stddef.h>
#include <unistd.h>
#include <syscall.h>
#include <linux/futex.h>

typedef int *mutex_t;

void simple_mutex_lock(mutex_t mutex) {
    int c;
    if ((c = __sync_val_compare_and_swap(mutex, 0, 1)) != 0 /* A0 */) {
	/* The mutex is either already locked or somebody is trying to lock it */
        do {
            if (c == 2
		/* At A0 some threads have been waiting for a mutex (perhaps, see
		 * the comment at A2), so add the calling thread to the wait queue
		 */
		|| __sync_val_compare_and_swap(mutex, 1, 2) != 0 /* A1 */
		/* We've observed the mutex in the `no waiters` state at A0.
		 * Mark the mutex as having waiters and add the calling thread
		 * to the wait queue.
		 */
		) {
		    /* successfully marked the mutex as having waiters (or it has
		     * been already in that state), proceed to waiting in the kernel
		     */
		    syscall(SYS_futex, mutex, FUTEX_WAIT, 2, NULL, NULL, 0);
	    } else {
		/* Other thread has unlocked the mutex, hence compare-and-swap above
		 * returned 0. Try to grab the mutex once again immediately.
		 * Note that we can't just proceed to the critical section since
		 * the mutex hasn't been marked as locked
		 */
	    }
	} while (
	     /* Try to grab the mutex again. We get here either when kernel has
	      * woken up the calling thread, or we've decided to skip waiting in
	      * the kernel. In both cases there might be other waiters. This
	      * "might be" is a bit irritating, but it's better to be safe than sorry.
	      */
	     (c == __sync_val_compare_and_swap(mutex, 0, 2)) != 0 /* A2 */);
    } else {
        /* uncontested: we've locked the mutex with a single instruction
	 * (the old state is `unlocked`, and the current is `no waiters`)
	 */
    }

}

void simple_mutex_unlock(mutex_t mutex) {
    if (__sync_fetch_and_sub(mutex, 1) != 1) {
        /* there might be threads waiting for the mutex */
        *mutex = 0;
	syscall(SYS_futex, mutex, FUTEX_WAKE, 1, NULL, NULL, 0);
    } else {
        /* the calling thread is the only "user" of the mutex, do nothing */
    }
}
