// Semaphore Class:	
// James Kayes ©2016 
////////////////////////////////
#include "semaphore.h"

Semaphore::Semaphore(): count_(0), count_initial_(0)
{
}

Semaphore::Semaphore(int count): count_(count), count_initial_(count)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::Signal()
{
	std::unique_lock<std::mutex> lock(semaphore_mutex_);
	count_++;
	semapore_cv_.notify_one(); // Notify one thread waiting on the condition variable.
}

void Semaphore::Wait()
{
	std::unique_lock<std::mutex> lock(semaphore_mutex_);
	if (count_ == 0)
	{
		semapore_cv_.wait(lock);
		if (count_ > 0)
		{
			count_--;
		}
	}
	else
	{
		count_--;
	}
}

void Semaphore::ResetCount()
{
	int temp = count_initial_; // Cannot assign an atomic type to another directly.
	count_ = temp; 
}

int Semaphore::GetCount()
{
	return count_;
}
