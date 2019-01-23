//	ThreadManager Class:
//	A class for dividing tasks among threads.
//	James Kayes ©2016		
/////////////////////////////////////////////
#include "thread_manager.h"

// Initialises the thread manager with the passed in number of threads:
ThreadManager::ThreadManager(boost::barrier* update_barrier, uint32_t num_threads) : update_barrier_(update_barrier), stopping_(false)
{ 
	for (uint32_t thread_num = 0; thread_num < num_threads; thread_num++)
	{
		threads_.push_back(new std::thread(std::mem_fn(&ThreadManager::WaitForTask), this)); // Starts new threads, each running WaitForTask.
	}
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
	// This will cause the threads to start grabbing tasks to run:
	run_tasks_semaphore_.Signal(); // Alows a waiting thread to grab a task.
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
		run_tasks_semaphore_.Wait(); // This waits until this semaphores count is above 0.
		if (stopping_)
		{
			run_tasks_semaphore_.Signal(); // Allows other threads to exit.
			return; // Allows thread to exit.
		}
		tasks_mutex_.lock(); // Locks mutex, to prevent multiple threads accessing the tasks_ queue.
		if (!tasks_.empty())
		{
			// Get the first task in the queue:
			if (Task* task = tasks_.front())
			{
				tasks_.pop();
				run_tasks_semaphore_.Signal(); // Allows another thread to find a task to run.
				tasks_mutex_.unlock();
				task->Run();
				delete task;
			}
		}
		else
		{
			tasks_mutex_.unlock();
			run_tasks_semaphore_.Signal(); // Ensures all threads can end.
			update_barrier_->wait(); // Wait for all threads to call wait.
			run_tasks_semaphore_.ResetCount(); // Resets count to 0.
		}
	}
}

void ThreadManager::Signal()
{
	run_tasks_semaphore_.Signal();
}
