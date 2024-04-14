#ifndef COROUTINE_H_
#define COROUTINE_H_

#include <cstdint>
#include <functional>
#include <string>
#include <memory>
#include <ucontext.h>
#include "noncopyable.h"

enum CoroutineState{
    RUNNABLE,
    BLOCKED,
    TERMINATED,
};

const uint32_t kStackSize = 1024*512;

class Coroutine : public noncopyable , public std::enable_shared_from_this<Coroutine>
{
public:
    using Func = std::function<void()>;
    using Ptr = std::shared_ptr<Coroutine>;

    Coroutine(Func cb,std::string name,uint32_t stack_size = kStackSize);
    ~Coroutine();

    //切换到当前线程的主协程
    static void SwapOut();
    //执行当前协程
    void SwapIn();
    Coroutine::Func getCallback();
    std::string getname();

    void setState(CoroutineState state){state_ = state;}
    CoroutineState getState() {return state_;}

    static uint64_t GetCid();
    static Coroutine::Ptr& GetCurrentCoroutine();
	static Coroutine::Ptr GetMainCoroutine();

private:
    Coroutine();
    static void RunInCoroutine();
    uint64_t c_id_;
    std::string name_;

    ucontext_t context_;
    Func func_;

    uint32_t stack_size_;
    void* stack_;

    CoroutineState state_;
};

#endif