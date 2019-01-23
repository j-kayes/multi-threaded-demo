// Semaphore Class:	
// James Kayes ©2016 
/////////////////////
#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>

class Semaphore
{
private:
	std::mutex count_mutex_;
	std::mutex semaphore_cv_mutex_;
	std::mutex notified_mutex_;

	std::condition_variable semaphore_cv_;
	std::atomic<bool> cv_notified_;
	std::atomic<int> count_;
	int count_initial_;

public:
	Semaphore();
	Semaphore(int count);
	~Semaphore();

	// Increments the count
	void Signal();
	// Blocks the thread if the count is 0
	void Wait();
	// Resets the threads count
	void Reset();
	// Returns the current count
	int GetCount();
};

