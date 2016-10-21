#include "../src/util.hpp"
#include <cassert>
#include <iostream>

using namespace std;

void test_get_range_headers_str(std::pair<int,int> range){
    cout << Util::GenRangeHeaderStr(range) << endl;
}

void test_split(){
    vector<pair<long long,long long> > offsets = Util::SplitFile(109,10);
    assert(offsets.size()>=10);
    for(int i = 0; i<offsets.size();++i){
        cout << offsets[i].first << " - " << offsets[i].second << endl;
        test_get_range_headers_str(offsets[i]);
    }
}

void test_init_file(){
    Util::InitFileDownload("http://img5.duitang.com/uploads/item/201411/14/20141114140306_BrNX2.thumb.700_0.jpeg",10);
}

void testReadLines(){
    vector<string> lines = Util::ReadConfigFileIntoLines("./20141114140306_BrNX2.thumb.700_0.jpeg.config");
    for(int i = 0; i< lines.size();++i){
        cout << lines[i] << endl; 
    }
}

void test_get_config_range(){
    pair<long long,long long> range = Util::GetRangeByRowNo("./20141114140306_BrNX2.thumb.700_0.jpeg.config",4+11);
    cout << range.first << " " << range.second << endl;
}

int main(){
    //test_split();
    // test_init_file();
    // testReadLines();
    test_get_config_range();
}
