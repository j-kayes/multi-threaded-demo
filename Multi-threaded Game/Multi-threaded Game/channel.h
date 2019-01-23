// Channel Class:	
// James Kayes ©2016 
////////////////////////////
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
	std::atomic<int> read_count;
	std::atomic<int> write_count;
	std::list<T> buffer_;
	std::mutex buffer_mutex_;
	Semaphore semaphore_;

public:
	Channel(): read_count(0), write_count(0)
	{
	};
	~Channel()
	{
	};


	void Write(T data)
	{
		buffer_mutex_.lock();
		buffer_.push_back(data);
		semaphore_.Signal(); // One more item now available
		write_count++;
		buffer_mutex_.unlock();
	};
	
	T Read()
	{
		semaphore_.Wait(); // Block until count > 0, then reduces it
		buffer_mutex_.lock();
		T item = buffer_.front();
		buffer_.pop_front();
		read_count++;
		buffer_mutex_.unlock();
		return item;
	};
};

