#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "MutexLock.h"
#include "Condition.h"
#include "WorkThread.h"
#include "MutexSafeGuard.h"
#include "typedefs.h"
#include "Task.h"

#include <vector>
#include <queue>

class Task;

class ThreadPool{
    friend class TaskManager;
    public:

        ~ThreadPool();

        bool AddTaskToQueue(Task task);

        Task getTaskFromQueue();

        bool isPoolOpen()const;

        std::queue<Task>::size_type getTaskQueuSize()const;

        bool start();

        bool stop();

        std::vector<WorkThread>::size_type getThreadQuantity(){
            return total_thread_quantity_;
        }

        //ThreadPool* GetInstance(std::vector<WorkThread>::size_type);

        ThreadPool(std::vector<WorkThread>::size_type);

    private:

        //static ThreadPool *mp_instance;

        std::vector<WorkThread>::size_type total_thread_quantity_;

        std::vector<WorkThread> thread_pool_;

        std::queue<Task> task_queue_;

        mutable MutexLock locker_;
        mutable MutexLock mutex_;
        mutable Condition cond_;

        bool is_pool_open_;
};

#endif
