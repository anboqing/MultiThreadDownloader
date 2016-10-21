#include <stdlib.h>
#include <iostream>

#include "ThreadPool.h"
#include "stdio.h"


ThreadPool::ThreadPool(std::vector<WorkThread>::size_type num):
    total_thread_quantity_(num),
    thread_pool_(total_thread_quantity_),
    locker_(),
    mutex_(),
    cond_(&locker_),
    is_pool_open_(false),
    task_queue_()
{
    //regedit all the work thread;
    for (std::vector<WorkThread>::iterator iter = thread_pool_.begin();
            iter != thread_pool_.end(); iter++)
    {
        iter -> registThreadPool(this);
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

bool ThreadPool::start()
{
    if (isPoolOpen())
    {
        return true;
    }

#ifdef DEBUG
    cout << "--------------------thread pool start ... " << endl;
#endif

    mutex_.lock();
    is_pool_open_ = true;
    mutex_.unlock();

    for (std::vector<WorkThread>::iterator iter = thread_pool_.begin();
            iter != thread_pool_.end(); iter++)
    {
        iter->start();
    }

    return true;
}

bool ThreadPool::stop()
{
    if (!isPoolOpen()) //already stopped
    {
        return true;
    }

    mutex_.lock();
    is_pool_open_ = false;
    mutex_.unlock();

    cond_.notify_all();

    for (std::vector<WorkThread>::iterator iter = thread_pool_.begin();
            iter != thread_pool_.end(); iter++)
    {
        iter->join();
    }

    return true;
}

bool ThreadPool::AddTaskToQueue(Task task){
#ifdef DEBUG
    cout << "ThreadPool::AddTaskToQueue, task fileid : " << task.m_file_id << " task segment id " << task.m_segment_id << endl;
#endif
    MutexSafeGuarde g(locker_);
    task_queue_.push(task);
    cond_.notify();
    return true;
}

Task ThreadPool::getTaskFromQueue(){
    MutexSafeGuarde g(locker_);

    while(isPoolOpen() && getTaskQueuSize()==0){
#ifdef DEBUG
        cout << __FUNCTION__ << " cond_.wait() " << endl;
#endif
        cond_.wait();
    }

    if(!isPoolOpen()){
        locker_.unlock();
        cout << " thread pool is not open ! " << endl;
        exit(1);
    }

    Task task = task_queue_.front();

#ifdef DEBUG
    cout << pthread_self() <<  task.m_file_id << " : " << task.m_segment_id << endl;
#endif
    task_queue_.pop();
    return task;
}

bool ThreadPool::isPoolOpen() const
{
    // MutexSafeGuarde g(mutex_);
    mutex_.lock();
    bool ret = is_pool_open_;
    mutex_.unlock();
    return ret;
}

std::queue<Task>::size_type ThreadPool::getTaskQueuSize()const
{
    MutexSafeGuarde g(mutex_);
    std::queue<Task>::size_type size = task_queue_.size();
    return size;
}
