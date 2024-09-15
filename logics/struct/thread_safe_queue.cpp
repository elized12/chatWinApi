#include "thread_safe_queue.h"

const char* empty_queue::what() const throw()
{
    return "queue is empty";
}
