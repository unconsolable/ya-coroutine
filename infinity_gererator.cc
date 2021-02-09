#include "coroutine.h"
#include <iostream>
#include <unistd.h>

void* gen(schedule *sc, void * param)
{
    param = nullptr;
    int64_t i = 1;
    while (1)
    {
        sc->coroutineYield(reinterpret_cast<void *>(i++));
    }
    return nullptr;
}

int main()
{
    schedule sc;
    int cid = sc.coroutineAdd(gen, nullptr, 512);
    while (1)
    {
        sc.coroutineResume(cid);
        void *ret = sc.coroutineReturn();
        std::cout << reinterpret_cast<int64_t>(ret) << '\n';
        sleep(1);
    }
    
}