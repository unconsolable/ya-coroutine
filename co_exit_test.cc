#include "coroutine.h"
#include <iostream>
#include <cassert>

void *foo(schedule *sc, void *arg)
{
    int sum = 0;
    for (int i = 0; i < 5; ++i)
    {
        sum += i;
        if (i % 2)
            sc->coroutineYield((void *)sum);
    }
    return (void *)sum;
}

void *bar(schedule *sc, void *arg)
{
    int sum = 0;
    for (int i = 0; i < 5; ++i)
    {
        sum += i + 100;
        if (i % 2)
            sc->coroutineYield((void *)sum);
    }
    return (void *)sum;
}

int main()
{
    schedule sc;
    int cid1 = sc.coroutineAdd(foo), cid2 = sc.coroutineAdd(bar);
    while (sc.coroutineState(cid1) != ZOMBIE && sc.coroutineState(cid2) != ZOMBIE) {
        sc.coroutineResume(cid1);
        std::cout << (int64_t)sc.coroutineReturn() << std::endl;
        sc.coroutineResume(cid2);
        std::cout << (int64_t)sc.coroutineReturn() << std::endl;
    }
    sc.coroutineDelete(cid1);
    sc.coroutineDelete(cid2);
    int cid3 = sc.coroutineAdd(foo);
    assert(cid3 == 0);
    sc.coroutineDelete(cid3);
}