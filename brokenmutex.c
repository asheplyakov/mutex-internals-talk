/* BIG RED WARNING: this code is WRONG on purpose
 *
 * Mutex implementation based on
 * - fetch-and-add atomic instruction
 * - Linux' futex system call
 */

#include <stddef.h>
#include <unistd.h>
#include <syscall.h>
#include <linux/futex.h>

typedef int *mutex_t;

void broken_mutex_lock(mutex_t mutex) {
    int c;
    while ((c = __sync_fetch_and_add(mutex, 1)) != 0) {
        syscall(SYS_futex, mutex, FUTEX_WAIT, c + 1, NULL, NULL, 0);
    }
}

void broken_mutex_unlock(mutex_t mutex) {
    *mutex = 0;
    syscall(SYS_futex, mutex, FUTEX_WAKE, 1, NULL, NULL, 0);
}
