#include "../src/util.hpp"
#include "../src/typedefs.h"

#include <cstring> // memcpy
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h> // -lcurl
#include <pthread.h>
using namespace std;

pthread_mutex_t mylock;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *info)
{
  
  //pthread_mutex_lock(&mylock);
  // cout << pthread_self()  << " => " << "size is : " <<  size << " nmemb is :" << nmemb << endl;
  ThreadData* thread_data= static_cast<ThreadData*>(info);

  Task* task = thread_data->mp_task;
  FILE* p_data_file = fopen(task->m_data_file_path.c_str(),"w+");
  FILE* p_config_file = fopen(task->m_config_file_path.c_str(),"r+");

  //TODO: save range line number in void*info
  // set fseek to config_file[line].first
  // and update config_file[line].first when recv data from ptr
  
  // fseek
  fseek(p_data_file,task->m_range.first,SEEK_SET);
  size_t written = fwrite(ptr, size, nmemb, p_data_file);

  cout << written << " " << task->m_range.second-task->m_range.first << endl;

  fclose(p_data_file);
  fclose(p_config_file);

  //pthread_mutex_unlock(&mylock);
  return written;
}

void* get_part_of_file(void* data){
    Task* task = static_cast<Task*>(data);
#if DEBUG
    cout << "threadid : " << pthread_self()%1000<<" range: " ;
    cout << task->m_range.first << " - " << task->m_range.second << " " << endl;
#endif

    CURL* curl = curl_easy_init(); 
    curl_slist* headers = NULL;
    if(curl){
        // set url
        curl_easy_setopt(curl, CURLOPT_URL, task->m_url.c_str());
        // set headers
        headers = curl_slist_append(headers, "Accept: image/webp,*/*;q=0.8");
        // set range to be download by this thread 
        headers = curl_slist_append(headers, Util::GenRangeHeaderStr(task->m_range).c_str());

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
}


const int POOL_SIZE = 10;

int main(int argc,char** argv){
    if(argc<2){
        cout << "usage : a.out url" << endl;
        return 0;
    }
    char* url = argv[1]; 

    pthread_mutex_init(&mylock,NULL);

    // generate data_file and fill config_file first;
    Util::InitFileDownload(url,POOL_SIZE);

    vector<Task> tasks = Util::GenerateTasks(url,POOL_SIZE);

    /* Must initialize libcurl before any threads are started */ 
    curl_global_init(CURL_GLOBAL_ALL);
    
    vector<pthread_t> pool(POOL_SIZE,0);

    for(int i = 0; i<tasks.size();++i){
         pthread_t tid;
         //cout << tasks[i].m_range.first << " " << tasks[i].m_range.second << endl;

         ThreadData tdata;
         tdata.mp_task = &tasks[i];
         tdata.m_range_id= i+3;

         pthread_create(&tid,NULL,get_part_of_file,static_cast<void*>(&tdata));
         pool.push_back(tid);
    }

    for(int i = 0; i<pool.size();++i){
        pthread_join(pool[i],NULL); // write_data is called after pthread_join 
    }

    pthread_mutex_destroy(&mylock);
}
