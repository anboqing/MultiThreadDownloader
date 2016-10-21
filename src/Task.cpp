#include "typedefs.h"
#include "Task.h"
#include "Cache.h"
#include <map>
#include <iostream>
using namespace std;

bool Task::CheckFinish(){
    SegmentRange beg_end = mp_cache->GetSegmentRange(m_file_id,m_segment_id);
    return  beg_end.first>=beg_end.second;
}

Task::Task(
            unsigned int fid,
            unsigned int sid
        ):
        m_file_id(fid),
        m_segment_id(sid){
        mp_cache = Cache::GetInstance(); 
}

Task::Task():m_file_id(0),m_segment_id(0){
#ifdef DEBUG
        cout << " Task::Task() called " << endl;
#endif
        mp_cache = Cache::GetInstance(); 
}

string Task::GetDataFilePath(){
    File file = mp_cache->GetFile(m_file_id);
    return file.m_data_file_path;
}

