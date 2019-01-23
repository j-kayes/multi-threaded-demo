//	ThreadManager Class:
//	A class for dividing tasks among threads.
//	James Kayes ©2016		
/////////////////////////////////////////////
#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <fstream>
#include "task.h"
#include "semaphore.h"
#include "barrier.h"

using std::vector;
using std::queue;

class ThreadManager
{
private:
	sf::Clock performance_timer_;
	std::ofstream timings_stream_;

	vector<std::thread*> threads_;
	queue<Task*> tasks_; 
	Semaphore run_tasks_semaphore_;
	Barrier* update_barrier_;
	std::mutex tasks_mutex_; // Makes sure only one thread can access tasks_ at the same time.
	std::mutex tasks_complete_mutex_;
	std::mutex semaphore_reset_mutex_;
	std::condition_variable semaphore_reset_cv_;
	std::condition_variable& ready_cv_; // Reference to the main games CV.
	bool first_update_;
	std::atomic<bool>& ready_; // Main thread will use this to make sure its ok to run new tasks.
	bool stopping_;

public:
	ThreadManager(Barrier* update_barrier, std::condition_variable& ready_cv, std::atomic<bool>& ready, uint32_t num_threads);
	~ThreadManager();

	void write_time(); // This is a debug method used to write the time that it takes to complete all tasks to a file.

	vector<std::thread*>& GetThreadsVector(); 
	int GetNumberOfTasks();
	int GetSemaphoreCount();
	void RunTasks();
	void AddTask(Task* task);
	void WaitForTask();
	void Signal();

};

