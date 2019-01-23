// Channel Class: Used for safely sending data to the main thread.
// James Kayes ©2016 
///////////////////////////////////////////////////////////////////
#pragma once
#include <atomic>
#include <mutex>
#include <list>
#include "semaphore.h"

typedef std::unique_lock<std::mutex> MutexLock;

template<typename T>
class Channel
{
private:
	std::atomic<int> read_count_;
	std::atomic<int> write_count_;
	std::list<T> buffer_;
	std::mutex buffer_mutex_;
	Semaphore semaphore_;

public:
	Channel() : read_count_(0), write_count_(0)
	{
	};
	~Channel()
	{
	};


	void Write(T data)
	{
		buffer_mutex_.lock();
		buffer_.push_back(data);
		semaphore_.Signal(); // One more item now available.
		write_count_++;
		buffer_mutex_.unlock();
	};
	
	T Read()
	{
		semaphore_.Wait(); // Blocks until count > 0, then reduces it.
		buffer_mutex_.lock();
		T item = buffer_.front();
		buffer_.pop_front();
		read_count_++;
		buffer_mutex_.unlock();
		return item;
	};
};

