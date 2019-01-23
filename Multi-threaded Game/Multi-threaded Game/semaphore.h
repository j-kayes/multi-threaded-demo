// Semaphore Class:	
// James Kayes ©2016 
////////////////////////////////
#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>

class Semaphore
{
private:
	std::mutex semaphore_mutex_;
	std::condition_variable semapore_cv_;
	std::atomic<int> count_;
	std::atomic<int> count_initial_;

public:
	Semaphore();
	Semaphore(int count);
	~Semaphore();

	void Signal();
	void Wait();
	void ResetCount();
	int GetCount();
};

