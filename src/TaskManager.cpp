
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>


#include "ThreadPool.h"
#include "Task.h"
#include "util.hpp"
#include "TaskManager.h"

#include <map>
#include <vector>
using namespace std;

TaskManager* TaskManager::mp_instance;
MutexLock TaskManager::lock;

TaskManager* TaskManager::GetInstance(const char* config,ThreadPool* tp)
{
    if(mp_instance==NULL){
        lock.lock();
        if(mp_instance==NULL){
            mp_instance= new TaskManager(config,tp); 
        }
        lock.unlock();
    }
    return mp_instance;
}

static std::vector<std::pair<long long,long long> > GetRanges(Size total_len,const std::string& url,const long thread_num){
    return Util::SplitFile(total_len,thread_num);
}


static std::string GetDataFileName(const std::string& url,const string& dir){
    std::ostringstream oss;
    int pos = url.find_last_of("/");
    std::string name = url.substr(pos+1);
    oss << dir <<"/"<< name;
    oss << ".data";
    return oss.str();
}

static std::string GetConfigFileName(const std::string& url,const string& dir){
    std::ostringstream oss;
    int pos = url.find_last_of("/");
    std::string name = url.substr(pos+1);
    oss << dir << "/" << name;
    oss << ".config";
    return oss.str();
}

// init config file
void InitFileDownload(const std::string& url,Segments ranges,const string& download_dir,Size size){

    std::string data_file_path = GetDataFileName(url,download_dir); 
    std::string config_file_path = GetConfigFileName(url,download_dir);
#ifdef DEBUG
    cout << "InitFileDownload..." << endl;
    cout << " data file path : " << data_file_path << endl;
    cout << " config file path : " << config_file_path << endl;
#endif

    FILE* fd = fopen(data_file_path.c_str(),"wb+");
    FILE* fc = fopen(config_file_path.c_str(),"wb+");

    if(!fd){
        std::cout << strerror(errno) << " : " << data_file_path.c_str() << std::endl; 
        exit(1);
    }
    if(!fc){
        std::cout << strerror(errno) << " : " << config_file_path.c_str() << std::endl; 
    }

    // init config file
    std::ostringstream oss;
    // 0. is finish
    oss << 0 << std::endl;
    // 1. url
    oss << url << std::endl; 
    // 2. file size 
    oss << size << std::endl;
    // 3. file path
    //  save absolute path (global config download path)
    oss << download_dir << "/" << config_file_path <<std::endl;
    oss << download_dir << "/" << data_file_path << std::endl;
    // 4. segement num
    oss << ranges.size() << endl;
    // every segement start end
    for(int i = 0; i<ranges.size();++i){
        oss << ranges[i].first << "\t" << ranges[i].second << std::endl; 
    }
    std::string conf = oss.str();
    char* buf = (char*)malloc(conf.size());
    memcpy(buf,conf.c_str(),conf.size());

    fwrite(buf,1,conf.size(),fc);

    free(buf);
    fclose(fd);
    fclose(fc);
}


std::vector<Task> GenerateTasks(TaskManager* tm,
        Segments ranges,int thread_num,unsigned int fid){
    // split a file into Range
    std::vector<Task> tasks;
    for(unsigned int i = 0; i<ranges.size();++i){
        Task task(fid,i);
        // todo md5(url)
        tasks.push_back(task);
    }
    return tasks;
}


bool TaskManager::DownloadUrl(const string& url){

#ifdef DEBUG
    cout << __FILE__ << " line: " << __LINE__ << endl;
#endif

    // split url into (segments) tasks
    long size = Util::GetDownloadFileLength(url);

    Segments ranges = GetRanges(size,url,m_per_task_thread_num);

    // create config/data file at disk
    InitFileDownload(url,ranges,m_default_download_path,size);

    // generate File and Tasks instance
    File file;
    file.m_url = url;
    file.m_config_file_path = GetConfigFileName(url,m_default_download_path);
    file.m_data_file_path = GetDataFileName(url,m_default_download_path);
    file.m_total_size=size;
    file.m_left_size=size;
    file.m_progress = 0.0;
    file.m_avg_speed = 0.0;
    file.m_b_is_finish = false;
    file.m_tasks = GenerateTasks(this,ranges,m_per_task_thread_num,mp_cache->GetFileListSize());
    
    // add File into m_live_file_list
    mp_cache->AddFile(file);

    // add tasks into m_task_queue
    lock.lock();
    
    // add file-segments pair into m_file_segment_map
    AddSegmentList(mp_cache->GetFileListSize()-1);

    for(unsigned int i = 0; i< file.m_tasks.size();++i){
        // add tasks to shared cache
        mp_cache->AddTask(file.m_tasks[i]);
        //m_task_queue.push_back(file.m_tasks[i]);
        mp_threadpool->AddTaskToQueue(file.m_tasks[i]);
    }

    lock.unlock();
    return true;
}

bool TaskManager::Remove(Task& task){
    return false;
}

bool TaskManager::PauseTask(Task& task){
    // Save all file's segment status in m_live_file_list
    return false;
}

bool TaskManager::ResumeTask(Task& task){
    return false;
}

void TaskManager::ListTask(){

}

void TaskManager::ShowTaskInfo(Task& task){

}

void TaskManager::ReadGlobalConfig(const char* config_path){
    ifstream ifs(config_path);
#ifdef DEBUG
    cout << "TaskManager read global config file :" << config_path << endl;
#endif
    if(!ifs.is_open()){
        cout << "open file error " << endl; 
    }
    getline(ifs,m_default_download_path);
    string line;
    getline(ifs,line);
    istringstream iss(line);
    iss >> m_max_thread_num;
    getline(ifs,line);
    istringstream is(line);
    is >> m_per_task_thread_num;
    ifs.close();

}

// read file list from global config path
void TaskManager::ReadFileList(){
    // scan dir find config files and insert into m_live_file_list
    // read from m_default_download_path

}

std::vector<std::string> ReadConfigFileIntoLines(const std::string& filename){
    std::vector<std::string> lines;
    std::ifstream ifs(filename.c_str());
    if(!ifs.good()){
        std::cout << "can not open file " << filename << std::endl;
    }
    std::string line;
    while(std::getline(ifs,line)){
        lines.push_back(line); 
    }
    ifs.close();
    return lines;
}

Segments ReadSegmentsFromConfigFile(const std::string& filename){
    Segments segments;
    vector<string> lines = ReadConfigFileIntoLines(filename);
    for(int i = 6; i<lines.size();++i){
        istringstream iss(lines[i]); 
        BegPos beg;
        EndPos end;
        iss >> beg >> end;
        segments.push_back(make_pair(beg,end));
    }
    return segments;
}

/**
 * @brief read config file and add segments into m_file_segment_list
 *
 * @param FileId index in m_live_file_list
 */
void TaskManager::AddSegmentList(FileId id){
    const File& file = mp_cache->GetFile(id);
    string conf_path = file.m_config_file_path;
    mp_cache->AddFileSegments(id,ReadSegmentsFromConfigFile(conf_path));
}

/**
 * @brief write new segments into file's config file
 *
 * @param FileId
 */
void TaskManager::UpdateConfigFile(FileId id){
    lock.lock();
    File file = mp_cache->GetFile(id);
    Segments ranges = mp_cache->GetSegments(id);
    ofstream oss(file.m_config_file_path.c_str(),ios_base::trunc);
    // 0. is finish
    oss << file.CheckFinish() << std::endl;
    // 1. url
    oss << file.m_url << std::endl; 
    // 2. file size 
    oss << file.m_total_size << std::endl;
    // 3. file path
    //  save absolute path (global config download path)
    oss << m_default_download_path << "/" << file.m_config_file_path <<std::endl;
    // 4. segement num
    oss << ranges.size() << endl;
    // every segement start end
    for(int i = 0; i<ranges.size();++i){
        oss << ranges[i].first << "\t" << ranges[i].second << std::endl; 
    }
    oss.close();
    lock.unlock();
}

// --------------operation for thread -----------


