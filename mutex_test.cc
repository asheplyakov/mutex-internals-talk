#include <cstdio>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include "peterson.h"

#ifdef USE_STD_MUTEX
typedef std::mutex lock_t;
static void lock(std::mutex& lk, int /* id */) {
	lk.lock();
}

static void unlock(std::mutex& lk, int /* id */) {
	lk.unlock();
}
#else
typedef peterson_spinlock lock_t;
static void lock(peterson_spinlock& lk, int id) {
	peterson_lock(&lk, id);
}

static void unlock(peterson_spinlock& lk, int id) {
	peterson_unlock(&lk, id);
}
#endif

static volatile int flag = -1;
static volatile int error_count = 0;
static lock_t flag_lock = {};

static void run(int id, int iterations, int busyloop_nsecs) {
	using rolex = std::chrono::high_resolution_clock;
	for (int i = 0; i < iterations; i++) {
		lock(flag_lock, id);
		flag = id;
		auto end = rolex::now() + std::chrono::nanoseconds(busyloop_nsecs);
		do {
			if (flag != id) {
				error_count++;
			}
		} while (rolex::now() < end);
		unlock(flag_lock, id);
	}
}

int main(int argc, char **argv) {
	int iterations = std::atoi(argc > 1 ? argv[1] : "0");
	if (!iterations) {
		iterations = 1000*1000*100;
	}
	int busyloop_nsecs = std::atoi(argc > 2 ? argv[2] : "0");
	if (busyloop_nsecs < 10) {
		busyloop_nsecs = 10;
	}
	std::thread t0{run, 0, iterations, busyloop_nsecs};
	std::thread t1{run, 1, iterations, busyloop_nsecs};
	t0.join();
	t1.join();
	if (error_count != 0) {
		std::printf("iterations: %d, errors: %d, busy loop duration %d nsec\n",
			iterations, error_count, busyloop_nsecs);
	}
	return error_count != 0;
}
