#pragma once
#include <stdexcept>
#include <atomic>
#include <mutex>
class Barrier
{
private:
	unsigned int max_count_;
	std::mutex cv_mutex_;
	std::condition_variable barrier_cv_;
	//std::atomic<bool> cv_notified_;
	std::atomic<unsigned int> count_;


public:
	Barrier(unsigned int count);
	~Barrier();

	// Causes the thread to wait until all threads are at this point
	void Wait();
	// Returns the number of waiting threads
	int GetNumberWaiting();
	// Allows any waiting threads through
	void Signal();
};

