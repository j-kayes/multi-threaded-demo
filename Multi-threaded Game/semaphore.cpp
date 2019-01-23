// Semaphore Class:	
// James Kayes ©2016 
////////////////////////////////
#include "semaphore.h"

Semaphore::Semaphore(): count_(0), count_initial_(0), cv_notified_(false)
{
}

Semaphore::Semaphore(int count): count_(count), count_initial_(count), cv_notified_(false)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::Signal()
{
	std::unique_lock<std::mutex> semaphore_lock(count_mutex_);
	count_++;
	std::unique_lock<std::mutex> notified_lock(notified_mutex_);
	cv_notified_ = true; // Helps with the condition variable being notified before other threads get a chance to call wait.
	semaphore_cv_.notify_one(); // Notify one thread waiting on the condition variable.
	// Ensures they unlock in the correct order:
	notified_lock.unlock();
	semaphore_lock.unlock();
}

void Semaphore::Wait()
{
	std::unique_lock<std::mutex> count_lock(count_mutex_);
	if (count_ == 0)
	{
		count_lock.unlock();
		// This will wait for up to 1 milliseconds, then checks the cv_notified bool again (helps prevents a deadlock if notified before it starts waiting):
		while (!cv_notified_)
		{
			semaphore_cv_.wait_for(std::unique_lock<std::mutex>(semaphore_cv_mutex_), std::chrono::milliseconds(1));
		}
		count_lock.lock(); // Lock this again to protect semaphore count operations.
		if (count_ > 0)
		{
			count_--; 
		}
	}
	else
	{
		count_--;
	}
	cv_notified_ = false;
}

void Semaphore::Reset()
{
	std::unique_lock<std::mutex> semaphore_lock(count_mutex_);
	std::unique_lock<std::mutex> notified_lock(notified_mutex_);
	count_ = count_initial_;
	cv_notified_ = false;
}

int Semaphore::GetCount()
{
	std::unique_lock<std::mutex> semaphore_lock(count_mutex_);
	return count_;
}
