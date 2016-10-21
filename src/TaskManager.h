#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include "File.h"
#include "typedefs.h"
#include "MutexLock.h"
#include "Cache.h"

#include <map>
#include <vector>
#include <queue>
#include <list>


using namespace std;

class Task;
class ThreadPool;

class TaskManager{
    friend class Task;
    public:

        bool DownloadUrl(const string& url);

        bool Remove(Task& task);

        bool PauseTask(Task& task);

        bool ResumeTask(Task& task);

        void ListTask();

        void ShowTaskInfo(Task& task);

        static TaskManager* GetInstance(const char* config,ThreadPool* tp);

        static MutexLock lock;
        
    private:

        ThreadPool* mp_threadpool;

        //--------global config-----------

        void ReadGlobalConfig(const char* path);

        std::string m_default_download_path;

        Size m_max_thread_num;

        Size m_per_task_thread_num;
        //---------------------------------
        
        // ------ file manage ----------------

        
        // ------------ task manage ------------

        //list<Task> m_task_queue;

        //vector<File> m_live_file_list;
        
        //map<FileId,Segments> m_file_segment_list;            

        Cache* mp_cache;

        // read file list from global config path
        void ReadFileList();

        /**
         * @brief read config file and add segments into m_file_segment_list
         *
         * @param FileId index in m_live_file_list
         */
        void AddSegmentList(FileId fid);

        /**
         * @brief write new segments into file's config file
         *
         * @param FileId
         */
        void UpdateConfigFile(FileId);

        // --------------operation for thread -----------
        
       
        //--------- singleton ---------------

        TaskManager(const char* config,ThreadPool* tp){
            // TODO : do some initialization 
            mp_threadpool=tp;
            ReadGlobalConfig(config);
            ReadFileList();
            mp_cache = Cache::GetInstance();
        }

        static TaskManager* mp_instance;

        TaskManager(TaskManager& other){}

};

#endif
