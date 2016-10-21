#include <stdio.h>
#include <string>
#include <curl/curl.h>
#include "../src/util.hpp"

using namespace std;

int GetUrl(const string& filename,const string& url)
{
    CURL *curl;
    CURLcode res;
    FILE *fp;
    if ((fp = fopen(filename.c_str(), "w")) == NULL)  // 返回结果用文件存储
        return -1;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: image/webp,*/*;q=0.8");
    // add Range:bytes=0-100
    // headers = curl_slist_append(headers, "Range:bytes=0-100");
    curl = curl_easy_init();    // 初始化
    if (curl)
    {
        //curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// 代理
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
        curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //将返回的http头输出到fp指向的文件
        //curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp); //将返回的html主体数据输出到fp指向的文件
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //  write data part of http response to file  
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);  
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, Util::ProgressCallback);  
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, curl); 
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);  

        res = curl_easy_perform(curl);   // 执行
        if (res != 0) {

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        fclose(fp);
        return 0;
    }
}

// split func




// test multi thread and break-resume


int main(int argc,char** argv){
    GetUrl(argv[2],argv[1]);
    double len = Util::GetDownloadFileLength(argv[1]);
    printf("\n file lenth : %.2f \n",len);
    return 0;
}
