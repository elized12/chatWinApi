#pragma once
#include <iostream>
#include <vector>

template<typename T>
class PreemptiveArray 
{
public:
    PreemptiveArray(size_t max_size) : max_size_(max_size), head_(0), tail_(0), count_(0)
    {
        buffer_.resize(max_size);
    }
public:
    void push(const T& value) 
    {
        buffer_[head_] = value;
        head_ = (head_ + 1) % max_size_;

        if (count_ < max_size_) 
        {
            ++count_;
        }
        else 
        {
            tail_ = (tail_ + 1) % max_size_;
        }
    }

    void pop() 
    {
        if (count_ > 0) 
        {
            tail_ = (tail_ + 1) % max_size_;
            --count_;
        }
    }

    T& front() 
    {
        return buffer_[tail_];
    }

    const T& front() const 
    {
        return buffer_[tail_];
    }

    T& back() 
    {
        return buffer_[(head_ + max_size_ - 1) % max_size_];
    }

    const T& back() const 
    {
        return buffer_[(head_ + max_size_ - 1) % max_size_];
    }

    bool empty() const 
    {
        return count_ == 0;
    }

    size_t size() const 
    {
        return count_;
    }

    std::vector<T> getData() const
    {
        return buffer_;
    }

    void clear()
    {
        buffer_.clear();
        head_ = 0; 
        tail_ = 0;
        count_ = 0;
    }

private:
    std::vector<T> buffer_;
    size_t max_size_;
    size_t head_;
    size_t tail_;
    size_t count_;
};


