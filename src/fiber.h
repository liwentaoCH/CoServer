#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include <ucontext.h>
#include <functional>
#include "thread.h"

 namespace coserver {
 // 协程解决的是异步事件，关注并发而不是并行
 class Scheduler;
 class Fiber : public std::enable_shared_from_this<Fiber>{
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        // 初始状态
        INIT,
        HOLD,
        // 运行状态
        EXEC,
        // 终止状态
        TERM,
        // 就绪状态
        READY,
        EXCEPT
    };
private:
     Fiber();

public:
     Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
     ~Fiber();

    // 重置协程函数，并重置状态
    // INIT，TERM
    void reset(std::function<void()> cb); // 节约内存分配
    // 切换到当前协程执行
    void swapIn();
    // 切换到后台执行
    void swapOut();

    void call();

    void back();

    uint64_t getId() const { return m_id; }
    static uint64_t GetFiberId();

    State getState() const { return m_state; }

 public:
     static void SetThis(Fiber* f);
    //返回当前执行点的协程
     static Fiber::ptr GetThis();
    //协程切换到后台，并且设置为Ready状态
    static void YieldToReady();
    //协程切换到后台，并且设置为Hold状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();
    //协程执行函数
     static void MainFunc();
    static void CallerMainFunc();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;
};

}
#endif