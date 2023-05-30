#include <vector>
class user{
    public:
        int user_id;
        double loc_x,loc_y;
        int slice_id;
        int act_bs_id;
        // std::vector<rb> rb_ids; 
        int rb_id;
        int num_of_rb; // Highly one case dependent
        double min_sinr;
        double min_rate_req;
        double max_rate_req;
        double latency;
    
};