#include "ThreadPool.h"
#include "TaskManager.h"
#include <glog/logging.h>
#include <unistd.h>

ThreadPool pool(20);

int main(int argc,char** argv){

    //google::InitGoogleLogging("./log");
    //FLAGS_log_dir = "./log";

    pool.start();

    TaskManager* task_manager = TaskManager::GetInstance("./global_config.conf",&pool);

    task_manager->DownloadUrl(argv[1]);

    pause();
}
