
#ifndef WORKTHREAD_H_
#define WORKTHREAD_H_
#include "Thread.h"
#include <string>

class ThreadPool;

class WorkThread : public Thread
{
    public:
        void run();
        bool registThreadPool(ThreadPool *p_pool);
    private:
        ThreadPool *p_pool_;
};
#endif
