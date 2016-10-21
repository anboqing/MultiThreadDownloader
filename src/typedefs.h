#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <map>
#include <vector>

using namespace std;

typedef unsigned int FileId,Size,SegmentId;
typedef long long BegPos,EndPos;
typedef pair<BegPos,EndPos> SegmentRange;
typedef vector<SegmentRange > Segments;

/* Deprecated
struct Task{
    std::string m_url; 
    std::pair<long long,long long> m_range;
    std::string m_data_file_path;
    std::string m_config_file_path;
    
};

struct ThreadData{
    Task* mp_task;
    unsigned m_range_id;
};
*/

#endif
