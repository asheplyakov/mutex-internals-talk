#ifndef PETERSON_MUTEX_H
#define PETERSON_MUTEX_H

static volatile int wants_to_enter[2] = { 0, 0 };
static volatile int looser = 0;

void enter(int id) {
    int other = 1 - id;
    wants_to_enter[id] = 1;
    looser = id;
    asm volatile ("mfence" ::: "memory");
    while (wants_to_enter[other] && (looser == id)) { /* wait */ }
}

void leave(int id) {
    wants_to_enter[id] = 0;
}

#endif /* PETERSON_MUTEX_H */
