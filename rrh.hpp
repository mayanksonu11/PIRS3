#include <vector>
#include "rb.hpp"
#include <string>
using namespace std;
class rrh{
    public:
        int rrh_id;
        int loc_x;
        int loc_y;
        double range;
        int num_rbs;
        double power;
        int rrh_type; // 0 for MBS and 1 for SBS
        vector<rb> rbs;
    
};