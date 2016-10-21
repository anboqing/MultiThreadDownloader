#ifndef __FILE_H__
#define __FILE_H__

#include <string>
#include "Task.h"
#include <vector>

class File{
    private:
        typedef unsigned int SIZE;
    public:
        std::string m_url;
        std::string m_config_file_path;
        std::string m_data_file_path;

        std::vector<Task> m_tasks;

        double m_progress;
        double m_avg_speed;

        SIZE m_total_size;
        SIZE m_left_size;

        bool m_b_is_finish;

        bool CheckFinish(){
            bool res = true;
            for(int i = 0; i<m_tasks.size();++i){
                res = res && m_tasks[i].CheckFinish(); 
            } 
            m_b_is_finish = res;
            return res;
        }
};

#endif
