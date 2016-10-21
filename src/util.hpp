#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include <map>
#include <curl/curl.h>
#include <sstream>
#include "typedefs.h"
#include "Task.h"
#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <fstream>

extern int errno;

class Util{

    public:

        static std::string GenRangeHeaderStr(std::pair<long long,long long> range){
            std::ostringstream oss;
            oss << "Range:bytes=";
            oss << range.first;
            oss << "-";
            oss << range.second;
            return oss.str();
        }

        static struct curl_slist* SetRange(CURL* curl,struct curl_slist* list,std::pair<int,int> range){
                if(curl) {
                    list = curl_slist_append(list, GenRangeHeaderStr(range).c_str());
                    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                }
                return list;
            }

        static void DisplayProgress(int progress,double speed,std::string unit,char* timeFormat,curl_off_t dlnow,curl_off_t dltotal)     
            {

                fflush(stdout);
                printf("\r");
                printf(" speed:%.2f%s/s ", speed, unit.c_str());
                printf("[%d%%] remainingTime: %s", progress,timeFormat);  
                printf(" downloaded: %ld%s",dlnow,unit.c_str());
                printf(" total: %ld%s",dltotal,unit.c_str());

                /*此处不能少，需要刷新输出缓冲区才能显示，
                  这是系统的输出策略*/
                fflush(stdout);
            }


        static
            int ProgressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
            {
                CURL *easy_handle = static_cast<CURL *>(clientp);
                char timeFormat[9] = "Unknow";

                // Defaults to bytes/second
                double speed;
                std::string unit = "B";

                curl_easy_getinfo(easy_handle, CURLINFO_SPEED_DOWNLOAD, &speed); // curl_get_info必须在curl_easy_perform之后调用

                if (speed != 0)
                {
                    // Time remaining
                    double leftTime = (dltotal - dlnow) / speed;
                    int hours = leftTime / 3600;
                    int minutes = (leftTime - hours * 3600) / 60;
                    int seconds = leftTime - hours * 3600 - minutes * 60;

#ifdef _WIN32
                    sprintf_s(timeFormat, 9, "%02d:%02d:%02d", hours, minutes, seconds);
#else
                    sprintf(timeFormat, "%02d:%02d:%02d", hours, minutes, seconds);
#endif

                    if (speed > 1024 * 1024 * 1024)
                    {
                        unit = "G";
                        speed /= 1024 * 1024 * 1024;
                    }
                    else if (speed > 1024 * 1024)
                    {
                        unit = "M";
                        speed /= 1024 * 1024;
                    }
                    else if (speed > 1024)
                    {
                        unit = "kB";
                        speed /= 1024;
                    }

                    //printf("speed:%.2f%s/s", speed, unit.c_str());

                    if (dltotal != 0)
                    {
                        double progress = ((double)dlnow / (double)dltotal) * 100;
                        DisplayProgress(progress,speed,unit,timeFormat,dlnow,dltotal); 
                        //printf("\t%.2f%%\tRemaing time:%s\n\r", progress, timeFormat);
                    }
                }

                return 0;
            }

        static size_t NoUseCodeblock(char *buffer, size_t x, size_t y, void *userdata)
            {
                (void)buffer;
                (void)userdata;
                return x * y;
            }

            // by bytes
        static double GetDownloadFileLength(std::string url)
            {
                CURL *easy_handle = NULL;
                int ret = CURLE_OK;
                double size = -1;

                do
                {
                    easy_handle = curl_easy_init();
                    if (!easy_handle)
                    {
                        break;
                    }

                    // Only get the header data
                    ret = curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
                    ret |= curl_easy_setopt(easy_handle, CURLOPT_HEADER, 1L);
                    ret |= curl_easy_setopt(easy_handle, CURLOPT_NOBODY, 1L);
                    ret |= curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, NoUseCodeblock);	// libcurl_a.lib will return error code 23 without this sentence on windows

                    if (ret != CURLE_OK)
                    {
                        break;
                    }

                    ret = curl_easy_perform(easy_handle);
                    if (ret != CURLE_OK)
                    {
                        char s[100] = {0};
#ifdef _WIN32
                        sprintf_s(s, sizeof(s), "error:%d:%s", ret, curl_easy_strerror(static_cast<CURLcode>(ret)));
#else
                        sprintf(s, "error:%d:%s", ret, curl_easy_strerror(static_cast<CURLcode>(ret)));
#endif
                        break;
                    }

                    // size = -1 if no Content-Length return or Content-Length=0
                    ret = curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
                    if (ret != CURLE_OK)
                    {
                        break;
                    }

                } while (0);

                curl_easy_cleanup(easy_handle);
                return size;
            }

        static std::vector<std::pair<long long,long long> >  SplitFile(const long int total_len,const long int thread_num){
                std::vector<std::pair<long long,long long> > offsets;
                std::size_t segements = thread_num;
                std::size_t seg_size = total_len/segements;
                long long start_pos,end_pos;
                for(int i = 0;i<segements-1;++i){
                    start_pos = i*seg_size; 
                    end_pos = (i+1)*seg_size - 1;
                    offsets.push_back(std::make_pair(start_pos,end_pos));
                }
                offsets.push_back(std::make_pair(end_pos+1,total_len-1));
                return offsets;
        }

};

#endif
