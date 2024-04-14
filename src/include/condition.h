#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include "noncopyable.h"
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "Mutexlock.h"

class sem
{
public:
    sem()
    {
        if( sem_init( &m_sem, 0, 0 ) != 0 )
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy( &m_sem );
    }
    bool wait()
    {
        return sem_wait( &m_sem ) == 0;
    }
    bool post()
    {
        return sem_post( &m_sem ) == 0;
    }

private:
    sem_t m_sem;
};


class Condition : noncopyable
{
public:
    explicit Condition(MutexLock &_mutex):
        mutex(_mutex)
    {
        pthread_cond_init(&cond, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond);
    }
    void wait()
    {
        pthread_cond_wait(&cond, mutex.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }
private:
    MutexLock &mutex;
    pthread_cond_t cond;
};

#endif
