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

static void loop_for(unsigned long usec, std::function<void()>&& fcn) {
	using hrc = std::chrono::high_resolution_clock;
	auto start = hrc::now();
	auto end = start + std::chrono::microseconds(usec);
	while (hrc::now() < end) {
		fcn();
	}
}

static volatile int flag = 0;
static volatile int error_count = 0;
static lock_t flag_lock = {};

static void run(int id, int iterations) {
	for (int i = 0; i < iterations; i++) {
		lock(flag_lock, id);
		flag = id + 1;
		loop_for(1UL, [=]() {
			if (flag != id + 1) {
				error_count++;
			}});
		unlock(flag_lock, id);
	}
}

int main(int argc, char **argv) {
	int iterations = std::atoi(argc > 1 ? argv[1] : "0");
	if (!iterations) {
		iterations = 1000*1000*10;
	}
	std::thread t0{run, 0, iterations};
	std::thread t1{run, 1, iterations};
	t0.join();
	t1.join();
	std::printf("iterations: %d, errors: %d\n", iterations, error_count);
	return 0;
}
