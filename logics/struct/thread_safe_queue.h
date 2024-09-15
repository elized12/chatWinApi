#pragma once
#include <memory>
#include <exception>
#include <mutex>
#include <condition_variable>

class empty_queue : public std::exception
{
	const char* what() const throw();
};

template<class T>
class thread_safe_queue
{
private:
	class node
	{
	public:
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	std::unique_ptr<node> head;
	node* tail;

	mutable std::mutex headLock;
	mutable std::mutex tailLock;
	std::condition_variable signalQueueNotEmpty;
	


	node* get_tail() const
	{
		std::lock_guard<std::mutex> lockT(tailLock);
		return tail;
	}

public:
	thread_safe_queue() : head(std::make_unique<node>()), tail(head.get()) {}
	thread_safe_queue(const thread_safe_queue&) = delete;
	thread_safe_queue& operator=(const thread_safe_queue&) = delete;
public:
	void push(T value);
	std::shared_ptr<T> pop();
	std::shared_ptr<T> wait_pop();
};

template<class T>
inline void thread_safe_queue<T>::push(T value)
{
	std::unique_ptr<node> new_node = std::make_unique<node>();
	node* new_tail = new_node.get();

	{
		std::lock_guard<std::mutex> lock(tailLock);
		tail->data = std::make_shared<T>(std::move(value));
		tail->next = std::move(new_node);
		tail = new_tail;
	}

	signalQueueNotEmpty.notify_one();
}

template<class T>
inline std::shared_ptr<T> thread_safe_queue<T>::pop()
{
	std::lock_guard<std::mutex> lockH(headLock);

	if (head.get() == get_tail())
	{
		return nullptr;
	}

	std::shared_ptr<T> return_data = std::move(head->data);
	std::unique_ptr<node> old_head = std::move(head);
	head = std::move(old_head->next);

	return return_data;
}

template<class T>
inline std::shared_ptr<T> thread_safe_queue<T>::wait_pop()
{
	std::unique_lock<std::mutex> lockH(headLock);

	signalQueueNotEmpty.wait(lockH, [this] { return head.get() != get_tail(); });

	std::shared_ptr<T> return_data = std::move(head->data);
	std::unique_ptr<node> old_head = std::move(head);
	head = std::move(old_head->next);

	return return_data;
}