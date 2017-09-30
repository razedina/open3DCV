#ifndef dmatch_h_
#define dmatch_h_

#include "keypoint/keypoint.h"

namespace open3DCV
{
    class DMatch
    {
    public:
        DMatch () {};
        DMatch (const int r_ikey1, const int r_ikey2, const float dist) :
            ind_key_(r_ikey1, r_ikey2), dist_(dist) {};
        DMatch (const int r_ikey1, const int r_ikey2, const Vec2f r_pt1, const Vec2f r_pt2, const float dist) :
            ind_key_(r_ikey1, r_ikey2), point_(r_pt1, r_pt2), dist_(dist) {};
        DMatch (const DMatch& match) :
            ind_key_(match.ind_key_), point_(match.point_), dist_(match.dist_) {};
        
        void update_match_pt(const std::vector<Keypoint>& key1, const std::vector<Keypoint>& key2);
        
        std::pair<int, int> ind_key_;
        std::pair<Vec2f, Vec2f> point_;
        float dist_;
    };
    
    inline void DMatch::update_match_pt(const std::vector<Keypoint>& key1, const std::vector<Keypoint>& key2)
    {
        point_.first = key1[ind_key_.first].coords();
        point_.second = key2[ind_key_.second].coords();
    }

} // namespace open3DCV

#endif
