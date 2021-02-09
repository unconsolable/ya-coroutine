#include "coroutine.h"

int schedule::coroutineAdd(coroutine_func _func, void *_arg, uint32_t _stacksz)
{
    int i = 0;
    for (; i < coroutines.size(); ++i) 
    {
        if (coroutines[i] == nullptr) 
            break;
    }
    if (i == coroutines.size())
    {
        coroutines.push_back(new coroutine(i, _func, _arg, _stacksz));
    }
    else
    {
        coroutines[i] = new coroutine(i, _func, _arg, _stacksz);
    }
    return i;
}

void schedule::coroutineResume(int cid) 
{
    if (cid < 0 || cid >= coroutines.size() || !coroutines[cid])
        return;
    if (coroutines[cid]->state == EMBRYO)
    {
        // Define the successor
        coroutines[cid]->ctx.uc_link = &caller;
        // Set the current running coroutine
        makecontext(&coroutines[cid]->ctx, reinterpret_cast<void (*)()>(coroutines[cid]->func), 3, this, coroutines[cid]->arg);
        curRunning = cid;
        coroutines[cid]->state = RUNNING;
        // Switch context
        swapcontext(&caller, &coroutines[cid]->ctx);
        // Control reach here via coroutineYield
        return;
    }
    if (coroutines[cid]->state == YIELDED)
    {
        // Define the successor
        coroutines[cid]->ctx.uc_link = &caller;
        // Set the current running coroutine
        curRunning = cid;
        coroutines[cid]->state = RUNNING;
        // Switch context
        swapcontext(&caller, &coroutines[cid]->ctx);
        // Control reach here via coroutineYield
        return;
    }
}

void schedule::coroutineYield()
{
    // Set to yield state
    int cur = curRunning;
    curRunning = -1;
    coroutines[cur]->state = YIELDED;
    // Resume to caller
    swapcontext(&coroutines[cur]->ctx, &caller);
}

coroutine_state schedule::coroutineState(int cid)
{
    if (cid < 0 || cid >= coroutines.size() || !coroutines[cid])
        return UNUSED;
    return coroutines[cid]->state;
}

void schedule::coroutineDelete(int cid)
{
    delete coroutines[cid];
    coroutines[cid] = nullptr;
}

coroutine::coroutine(int _cid, coroutine_func _func, void *_arg, uint32_t _stacksz) :
    cid(_cid), func(_func), arg(_arg), stacksz(_stacksz), state(EMBRYO)
{
    // set context to the constructor
    getcontext(&ctx);
    // allocate a stack for the coroutine
    stack = new char[stacksz];
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = stacksz;
    ctx.uc_stack.ss_flags = 0;
}

coroutine::~coroutine()
{
    // deallocate the stack
    state = UNUSED;
    delete []stack;
}