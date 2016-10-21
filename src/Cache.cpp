#include "Cache.h"
#include <iostream>
#include <stdlib.h>

Cache* Cache::mp_instance;
MutexLock Cache::m_lock;

/**
 * @brief increase task's segment start position by *offset* bytes
 *        should be called by thread's write_callback function
 * @param task target task
 * @param offset data(bytes) that download recently
 */
void Cache::IncreaseOffset(Task& task,unsigned long long offset){
    m_lock.lock();
    m_file_segment_list[task.m_file_id][task.m_segment_id].first+=offset-1;
    m_lock.unlock();
}


Cache::Cache():
    m_task_queue(),
    m_live_file_list(),
    m_file_segment_list()
{
#ifdef DEBUG
    cout << " Cache ctor() called " << endl;
#endif
}

Cache* Cache::GetInstance(){
    if(mp_instance==NULL){
        m_lock.lock();
        if(mp_instance==NULL){
            mp_instance = new Cache(); 
        }
        m_lock.unlock();
    }
    return mp_instance;
}

unsigned int Cache::GetFileListSize(){
    m_lock.lock();
    unsigned int size = m_live_file_list.size();
    m_lock.unlock();
    return size;
}


Segments Cache::GetSegments(FileId fid){
    m_lock.lock();
    Segments seg = m_file_segment_list[fid];
    m_lock.unlock();
    return seg;
}

Task Cache::GetTask(){
    m_lock.lock();
    Task res = m_task_queue.front();
    m_task_queue.pop_front();
    m_lock.unlock();
    return res;
}

void Cache::AddTask(Task task){
    m_lock.lock();
#ifdef DEBUG
    cout << "Cache::AddTask , task fileid : " << task.m_file_id << " task segment id " << task.m_segment_id << endl;
#endif
    m_task_queue.push_back(task);
    m_lock.unlock();
}

File Cache::GetFile(FileId idx){
    m_lock.lock();
    File f = m_live_file_list[idx];
    m_lock.unlock();
    return f;
}

void Cache::AddFile(File file){
    m_lock.lock();
#ifdef DEBUG
    cout << __FILE__ << __LINE__ << "AddFile: " << file.m_url << " " << file.m_tasks.size() << endl;
#endif
    m_live_file_list.push_back(file);
    m_lock.unlock();
}

SegmentRange Cache::GetSegmentRange(FileId fid,SegmentId sid){
    m_lock.lock();
    Segments seg = m_file_segment_list[fid];
    if(seg.size()!=0){
        SegmentRange range = seg[sid];
        m_lock.unlock();
        return range;
    }
    else{
        m_lock.unlock();
        cout << " in Cache::GetSegmentRange m_file_segment_list[fid] size==0" << endl;
        exit(1);
    }
}

void Cache::AddFileSegments(FileId fid,Segments segments){
    m_lock.lock();
#ifdef DEBUG
    cout << "----Add File segments---" <<endl;
#endif
    m_file_segment_list[fid] = segments;
    m_lock.unlock();
}

