#include "coroutine.h"
#include <iostream>

void foo(schedule *sc, void *arg)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "foo\n";
        if (i == 2)
            sc->coroutineYield();
    }
}

void bar(schedule *sc, void *arg)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "bar\n";
        if (i == 2)
            sc->coroutineYield();
    }
}

int main()
{
    schedule sc;
    int cid1 = sc.coroutineAdd(foo), cid2 = sc.coroutineAdd(bar);
    sc.coroutineResume(cid1);
    std::cout << "main1\n";
    sc.coroutineResume(cid2);
    std::cout << "main2\n";
    sc.coroutineResume(cid1);
    std::cout << "main3\n";
    sc.coroutineResume(cid2);
}