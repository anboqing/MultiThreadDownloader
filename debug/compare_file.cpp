#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include <assert.h>

using namespace std;

int main(int argc,char** argv){

    FILE* fp_ori,*fp_my;
    fp_ori = fopen(argv[1],"rb");
    fp_my = fopen(argv[2],"rb");

    fseek(fp_ori,0L,SEEK_SET);
    fseek(fp_my,0L,SEEK_SET);

    long ori_beg = ftell(fp_ori);
    long my_beg = ftell(fp_my);

    fseek(fp_ori,0L,SEEK_END);
    fseek(fp_my,0L,SEEK_END);


    long ori_end = ftell(fp_ori);
    long my_end = ftell(fp_my);

    long len_ori = ori_end - ori_beg;
    long len_my = my_end - my_beg;

    rewind(fp_ori);
    rewind(fp_my);

    cout << "origin file size: "<<  len_ori << "  | my file size : " << len_my << endl;

    assert(len_ori==len_my);

    char* buf_ori=(char*)malloc(len_ori);
    char* buf_my = (char*)malloc(len_my);

    fread(buf_ori,8,len_ori%8,fp_ori);
    fread(buf_my,8,len_my%8,fp_my);

    long i = 0;
    for(i = 0; i<len_ori;++i){
        if(buf_ori[i]!=buf_my[i]){
            break;
        }
    }
    if(i!=len_ori)
        cout << "different at : "<< i << endl;
    else
        cout << "same" << endl;

    fclose(fp_ori);
    fclose(fp_my);

    free(buf_ori);
    free(buf_my);

}
