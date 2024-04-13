#ifndef MUTEXLOCK_H_
#define MUTEXLOCK_H_
#include <pthread.h>

class MutexLock
{
public:
    MutexLock(){
        pthread_mutex_init(&m_mutex,NULL);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_t* get()
    {
        return &m_mutex;
    }
private:
    pthread_mutex_t m_mutex;
};


class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock& mutex):mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }
private:
    MutexLock mutex_;
};
#endif