#include "barrier.h"

Barrier::Barrier(unsigned int count): max_count_(count), count_(count)
{
	if (count == 0) {
		throw std::invalid_argument("Barrier count must be greater than 0.");
	}
}

Barrier::~Barrier()
{
}

// Returns the number of threads waiting on the barrier.
int Barrier::GetNumberWaiting()
{
	return (max_count_ - count_);
}

void Barrier::Signal()
{
	barrier_cv_.notify_all();
}

void Barrier::Wait()
{
	if (--count_ != 0) {
		// Block thread until notified:
		barrier_cv_.wait(std::unique_lock<std::mutex>(cv_mutex_));

	}
	else {
		count_ = max_count_;
		barrier_cv_.notify_all();

	}
}

