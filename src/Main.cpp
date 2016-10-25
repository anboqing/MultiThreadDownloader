#include "ThreadPool.h"
#include "TaskManager.h"
#include <glog/logging.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>


ThreadPool pool(20);

int main(int argc,char** argv){

    if(argc < 2){
        cout << "Usage : ./main <DownloadUrl> " << endl; 
        return 1;
    }

    FLAGS_log_dir = "/home/anboqing/Code/MultiThreadDownloader/src";
    google::InitGoogleLogging("main.log");

    pool.start();

    TaskManager* task_manager = TaskManager::GetInstance("./global_config.conf",&pool);

    task_manager->DownloadUrl(argv[1]);

    pause();
}
