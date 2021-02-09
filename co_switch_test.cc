#include "coroutine.h"
#include <iostream>

void* foo(schedule *sc, void *arg)
{
    int sum = 0;
    for (int i = 0; i < (int64_t)arg; ++i)
    {
        sum += i;
        if (i == 2)
            sc->coroutineYield((void *)sum);
    }
    return (void *)sum;
}

void* bar(schedule *sc, void *arg)
{
    int sum = 0;
    for (int i = 0; i < (int64_t)arg; ++i)
    {
        sum += i + 100;
        if (i == 2)
            sc->coroutineYield((void *)sum);
    }
    return (void *)sum;
}

int main()
{
    schedule sc;
    int cid1 = sc.coroutineAdd(foo, (void *)5), cid2 = sc.coroutineAdd(bar, (void *)5);
    sc.coroutineResume(cid1);
    std::cout << (int64_t)sc.coroutineReturn() << '\n';
    sc.coroutineResume(cid2);
    std::cout << (int64_t)sc.coroutineReturn() << '\n';
    sc.coroutineResume(cid1);
    std::cout << (int64_t)sc.coroutineReturn() << '\n';
    sc.coroutineResume(cid2);
    std::cout << (int64_t)sc.coroutineReturn() << '\n';
}