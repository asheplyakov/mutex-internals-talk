#include <cstdio>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include "peterson.h"

static volatile int flag = 0;
static volatile int error_count = 0;
#ifdef USE_STD_MUTEX
static std::mutex flag_mutex = {};
#endif

#ifdef USE_STD_MUTEX
static inline void flag_lock(int /* id */) {
	flag_mutex.lock();
}
static inline void flag_unlock(int /* id */) {
	flag_mutex.unlock();
}
#else
static inline void flag_lock(int id) {
	cs_enter(id);
}
static inline void flag_unlock(int id) {
	cs_leave(id);
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

static void run(int id, int iterations) {
	for (int i = 0; i < iterations; i++) {
		flag_lock(id);
		flag = id + 1;
		loop_for(1UL, [=]() {
			if (flag != id + 1) {
				error_count++;
			}});
		flag_unlock(id);
	}
}

int main(int argc, char **argv) {
	int iterations = std::atoi(argc > 1 ? argv[1] : "0");
	if (!iterations) {
		iterations = 1000*1000*10;
	}
	std::vector<std::thread> threads;
	threads.emplace_back(run, 0, iterations);
	threads.emplace_back(run, 1, iterations);

	for (auto& t: threads) {
		t.join();
	}
	std::printf("iterations: %d, errors: %d\n", iterations, error_count);
	return 0;
}
