//	ThreadManager Class:
//	A class for dividing tasks among threads.
//	James Kayes ©2016		
/////////////////////////////////////////////
#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <boost/thread/barrier.hpp> // Using boost barrier to save time.
#include "task.h"
#include "semaphore.h"

using std::vector;
using std::queue;

class ThreadManager
{
private:
	vector<std::thread*> threads_;
	queue<Task*> tasks_; 
	std::mutex tasks_mutex_; // Makes sure only one thread can access tasks_ at the same time.
	Semaphore run_tasks_semaphore_;
	boost::barrier* update_barrier_;
	bool stopping_;

public:
	ThreadManager(boost::barrier* update_barrier, uint32_t num_threads);
	~ThreadManager();

	vector<std::thread*>& GetThreadsVector(); 
	int GetNumberOfTasks();
	int GetSemaphoreCount();
	void RunTasks();
	void AddTask(Task* task);
	void WaitForTask();
	void Signal();

};

