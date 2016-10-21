#ifndef __TASK_H__
#define __TASK_H__

#include <string>

class Cache;

class Task{
public: 

    Task(unsigned int fid,unsigned int sid);
    Task();
 
    Cache* mp_cache;
    unsigned int m_file_id;
    unsigned int m_segment_id;
    bool CheckFinish();
    std::string GetDataFilePath();
};

#endif
