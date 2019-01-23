//	ThreadManager Class:
//	A class for dividing tasks among threads.
//	James Kayes ©2016		
/////////////////////////////////////////////
#include "thread_manager.h"

// Initialises the thread manager with the passed in number of threads:
ThreadManager::ThreadManager(Barrier* update_barrier, std::condition_variable& ready_cv, std::atomic<bool>& ready, uint32_t num_threads): update_barrier_(update_barrier),
	first_update_(true), stopping_(false), ready_cv_(ready_cv), ready_(ready)
{ 
	for (uint32_t thread_num = 0; thread_num < num_threads; thread_num++)
	{
		threads_.push_back(new std::thread(std::mem_fn(&ThreadManager::WaitForTask), this)); // Starts new threads, each running WaitForTask.
	}
	timings_stream_.open("timings.txt"); // Open timings file to later write to if needed.
}

ThreadManager::~ThreadManager()
{
	stopping_ = true;
	run_tasks_semaphore_.Signal(); // Allows threads to return.
	for (auto thread : threads_)
	{
		thread->join();
		delete thread;
	}
	int tasks_left = GetNumberOfTasks();
	for (int i = 0; i < tasks_left; i++)
	{
		delete tasks_.front();
		tasks_.pop();
	}
	timings_stream_.close();
}

// Debug fuction for recording time to complete tasks:
void ThreadManager::write_time()
{
	// Returns the elapsed time since run tasks was called and records it to Timings.txt (in ms).
	timings_stream_ << performance_timer_.getElapsedTime().asSeconds() * 1000.0f << " " << std::flush;
}

vector<std::thread*>& ThreadManager::GetThreadsVector()
{
	return threads_;
}

int ThreadManager::GetNumberOfTasks()
{
	std::unique_lock<std::mutex> lock(tasks_mutex_); // Makes sure only one thread can access tasks_ at once.
	return tasks_.size();
}

int ThreadManager::GetSemaphoreCount()
{
	return run_tasks_semaphore_.GetCount();
}

void ThreadManager::RunTasks()
{
	performance_timer_.restart(); 
	std::unique_lock<std::mutex> tasks_lock(tasks_mutex_);
	// This will cause the threads to start grabbing tasks to run:
	run_tasks_semaphore_.Signal(); // Allows a waiting thread to grab a task.
	first_update_ = false;
}

void ThreadManager::AddTask(Task* task)
{
	std::unique_lock<std::mutex> lock(tasks_mutex_); // Makes sure only one thread can access tasks_ at once.
	tasks_.push(task);
}

void ThreadManager::WaitForTask()
{
	while (true)
	{
		// Signals once for each thread waiting on the barrier to prevent the semaphore causing a deadlock:
		for (uint32_t waiting_thread = 0; waiting_thread < update_barrier_->GetNumberWaiting(); waiting_thread++) {
			run_tasks_semaphore_.Signal();
		}
		run_tasks_semaphore_.Wait(); 
		if (stopping_)
		{
			run_tasks_semaphore_.Signal(); // Allows other threads to exit.
			return; // Allows thread to exit.
		}
		std::unique_lock<std::mutex> tasks_lock(tasks_mutex_); // Locks mutex, to prevent multiple threads accessing the tasks_ queue.
		if (!tasks_.empty())
		{
			// Get the first task in the queue:
			if (Task* task = tasks_.front())
			{
				tasks_.pop();
				run_tasks_semaphore_.Signal(); // Allows another thread to find a task to run.
				tasks_lock.unlock();
				task->Run();
				delete task;
			}
		}
		else
		{
			tasks_lock.unlock();
			run_tasks_semaphore_.Signal(); // Ensures all threads can end.
			update_barrier_->Wait(); // Wait for all threads to call wait.
			tasks_lock.lock();
			// The main thread will sometimes finish adding tasks for the next update and attempt to read from the channel before this happens:
			run_tasks_semaphore_.Reset(); 
			ready_ = true; 
			ready_cv_.notify_one(); // This is why this was added also.
		}
	}
}

void ThreadManager::Signal()
{
	run_tasks_semaphore_.Signal();
}
