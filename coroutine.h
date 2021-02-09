#ifndef COROUTINE_C
#define COROUTINE_C

#include <ucontext.h>
#include <inttypes.h>
#include <vector>

#define KiB *1024
#define Mib Kib * 1024

enum coroutine_state
{
    UNUSED = 0,
    EMBRYO,
    RUNNING,
    YIELDED,
    ZOMBIE
};

class schedule;
class coroutine;

typedef void *(*coroutine_func)(schedule *, void *);

class schedule
{
    friend void coroutineWrapper(schedule *sc, int cid, void *arg);

public:
    // Add a coroutine to the schedule, return cid
    int coroutineAdd(coroutine_func _func, void *_args = nullptr, uint32_t _stacksz = 512 KiB);
    // Yield the running coroutine
    void coroutineYield(void *_ret);
    // Run a coroutine from EMBRYO/YIELDED state
    void coroutineResume(int cid);
    // Delect the coroutine
    void coroutineDelete(int cid);
    // Return the running cid
    int coroutineRunningCid();
    // Value of coroutine (by yield or return)
    void *coroutineReturn();
    // Return the status of the coroutine
    coroutine_state coroutineState(int cid);

private:
    std::vector<coroutine *> coroutines;
    // cid of the current running coroutine
    // -1 for no running coroutine
    int curRunning = -1;
    void *ret = nullptr;
    ucontext_t caller;
};

class coroutine
{
    friend void coroutineWrapper(schedule *sc, int cid, void *arg);
    friend class schedule;

public:
    coroutine(int _cid, coroutine_func _func, void *_args, uint32_t _stacksz);
    ~coroutine();

private:
    ucontext_t ctx;
    int cid;
    coroutine_func func = nullptr;
    void *arg = nullptr;
    char *stack = nullptr;
    uint32_t stacksz = 0;
    coroutine_state state;
};

#endif