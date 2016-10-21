#include <sys/types.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <map>
#include <utility>
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <cstring> // memcpy
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h> // -lcurl
#include <pthread.h>

#include "util.hpp"
#include "Task.h"
#include "WorkThread.h"
#include "ThreadPool.h"
#include "Cache.h"
#include "typedefs.h"

#include <glog/logging.h>

using namespace std;

//TODO: write_data
//

extern int errno;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *info)
{
    //pthread_mutex_lock(&mylock);
    Task* task= static_cast<Task*>(info);

    string data_file_path = task->GetDataFilePath();

    FILE* p_data_file = fopen(task->GetDataFilePath().c_str(),"w+");

    if(!p_data_file){
        LOG(INFO) << " can not open data file  : " << data_file_path << endl;
        //perror(strerr(errno)); 
    }

    // read beg from task manager's m_file_segment_list

    BegPos beg = task->mp_cache->GetSegmentRange(task->m_file_id,task->m_segment_id).first;

    LOG(INFO) <<  " Begin : " << beg << endl;

    // fseek
    fseek(p_data_file,beg,SEEK_SET);
    size_t written = fwrite(ptr, size, nmemb, p_data_file);
    // increase segment offset 
    task->mp_cache->IncreaseOffset(*task,beg+size*nmemb-1);

    //cout << written << " " << task->m_range.second-task->m_range.first << endl;

    fclose(p_data_file);

    //pthread_mutex_unlock(&mylock);
    return written;
}


void* get_part_of_file(void *data)
{
    Task* task = static_cast<Task*>(data);
#ifdef DEBUG
    cout << " WorkThread begin get_part_of_file .. " << endl;
    SegmentRange range = task->mp_cache->GetSegmentRange(task->m_file_id,task->m_segment_id);
    cout << " task id " << task->m_file_id << " segment id " << task->m_segment_id <<   " threadid : " << pthread_self() % 1000<<" range: " << range.first << " -- " << range.second << endl;
#endif

    CURL* curl = curl_easy_init(); 
    curl_slist* headers = NULL;
    if(curl){
        // set url
        curl_easy_setopt(curl, CURLOPT_URL, task->mp_cache->GetFile(task->m_file_id).m_url.c_str());
        // set headers
        headers = curl_slist_append(headers, "Accept: image/webp,*/*;q=0.8");
        // set range to be download by this thread 
        // Range:byte=0-100
        headers = curl_slist_append(headers, 
                Util::GenRangeHeaderStr(task->mp_cache->GetSegmentRange(task->m_file_id,task->m_segment_id)).c_str()
                );

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, task);

        CURLcode res = curl_easy_perform(curl);
        if(res!=CURLE_OK){
            cout << "error" << endl; 
        }
        curl_easy_cleanup(curl);

        curl_slist_free_all(headers); /* free the list again */    
    }
    return NULL;
}

void WorkThread::run()
{
    while (true)
    {
#ifdef DEBUG
        cout << "workthread get task from queue " << endl;
#endif
        Task t = p_pool_->getTaskFromQueue();
#ifdef DEBUG
        cout << "workthread start download file" << endl;
#endif
        get_part_of_file(&t);
    }
}

bool WorkThread::registThreadPool(ThreadPool *p_pool)
{
    // regist Pool to tell thread get task from where
    if (!p_pool)
    {
        throw runtime_error("can not regeditThreadPool ");
        return false;
    }
    p_pool_ = p_pool;
    return true;
}
