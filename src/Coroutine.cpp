#include "Coroutine.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <iostream>

static thread_local uint64_t t_coroutine_id{0};
static thread_local uint64_t t_total_coroutine{0};

Coroutine::Coroutine(Func cb,std::string name,uint32_t stack_size = kStackSize)
        :func_(cb),
        c_id_(t_coroutine_id++),
        name_(name+"-"+std::to_string(c_id_)),
        stack_size_(stack_size),
        state_(CoroutineState::RUNNABLE)
{
    assert(stack_size>0);

    stack_ = (char*)malloc(stack_size);
    if(!stack_)
    {
        std::cout<<"run out of memory"<<std::endl;
    }

    if(getcontext(&context_))
    {
        std::cout<<"getcontext:errno="<<errno<< " error string:" << strerror(errno);
    }
    context_.uc_link = nullptr;
    context_.uc_stack.ss_size = stack_size_;
    context_.uc_stack.ss_sp = stack_;

    makecontext(&context_, &Coroutine::RunInCoroutine, 0);
}

Coroutine::Coroutine()
    :c_id_(++t_coroutine_id),
	name_("Main-" + std::to_string(c_id_)),
	func_(nullptr),
	stack_size_(0),
	stack_(nullptr),
	state_(CoroutineState::RUNNABLE) {
	
	if (getcontext(&context_)) {
		std::cout << "getcontext: errno=" << errno
				<< " error string:" << strerror(errno);
	}
}

Coroutine::~Coroutine() {
	if (stack_) {
		free(stack_);
	}
}

//挂起当前正在执行的协程，切换到主协程执行，必须在非主协程调用
void Coroutine::SwapOut()
{
    assert(GetCurrentCoroutine() != nullptr);

    if(GetCurrentCoroutine() == GetMainCoroutine())
    {
        return;
    }

    Coroutine* old_coroutine = GetCurrentCoroutine().get();
    GetCurrentCoroutine() = GetMainCoroutine();

    if(swapcontext(&(old_coroutine->context_),&(GetCurrentCoroutine()->context_)))
    {
        std::cout << "swapcontext: errno=" << errno
				<< " error string:" << strerror(errno);
    }
}

//挂起主协程，执行当前协程，只能在主协程调用,类似libco中的resume
void Coroutine::SwapIn() {
	if (state_ == CoroutineState::TERMINATED) {
		return;
	}
	Coroutine::Ptr old_coroutine = GetMainCoroutine();
	GetCurrentCoroutine() = shared_from_this();

	if (swapcontext(&(old_coroutine->context_), &(GetCurrentCoroutine()->context_))) {
		std::cout << "swapcontext: errno=" << errno
				<< " error string:" << strerror(errno);
	}
}

Coroutine::Func Coroutine::getCallback() {
	return func_;
}

uint64_t Coroutine::GetCid() {
	assert(GetCurrentCoroutine() != nullptr);
	return GetCurrentCoroutine()->c_id_;
}

void Coroutine::RunInCoroutine() {
	GetCurrentCoroutine()->func_();

	//重新返回主协程
	GetCurrentCoroutine()->setState(CoroutineState::TERMINATED);
	Coroutine::SwapOut();
}

Coroutine::Ptr& Coroutine::GetCurrentCoroutine() {
	//第一个协程对象调用swapIn()时初始化
	static thread_local Coroutine::Ptr t_cur_coroutine;
	return t_cur_coroutine;
}

Coroutine::Ptr Coroutine::GetMainCoroutine() {
	static thread_local Coroutine::Ptr t_main_coroutine = Coroutine::Ptr(new Coroutine());
	return t_main_coroutine;
}

std::string Coroutine::getname() {
	return name_;
}
	
