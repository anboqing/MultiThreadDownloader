#ifndef __CACHE_H__
#define __CACHE_H__

#include "typedefs.h"
#include "File.h"
#include "MutexLock.h"
#include "Task.h"

#include <map>
#include <list>
#include <vector>
#include <map>
#include <string>

class Cache{

    public:
        /**
         * @brief increase task's segment start position by *offset* bytes
         *        should be called by thread's write_callback function
         * @param task target task
         * @param offset data(bytes) that download recently
         */
        void IncreaseOffset(Task& task,unsigned long long offset);
    
        unsigned int GetFileListSize();

        Task GetTask();

        void AddTask(Task task);

        File GetFile(FileId idx);

        void AddFile(File file);

        SegmentRange GetSegmentRange(FileId fid,SegmentId sid);

        Segments GetSegments(FileId fid);

        void AddFileSegments(FileId fid,Segments segments);

        static Cache* GetInstance();

    private:

        Cache();

        static Cache *mp_instance;

        static MutexLock m_lock;

        list<Task> m_task_queue;

        vector<File> m_live_file_list;

        std::map<FileId,Segments> m_file_segment_list;

};

#endif
