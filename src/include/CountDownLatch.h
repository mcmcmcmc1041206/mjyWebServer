#pragma once

#include "condition.h"
#include "Mutexlock.h"

#include "noncopyable.h"

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};