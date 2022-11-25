#include <vector>
class user{
    public:
        int user_id;
        int loc_x,loc_y;
        int slice_id;
        int act_bs_id;
        // std::vector<rb> rb_ids; 
        int rb_id;
        int num_of_rb; // Highly one case dependent
        double req_rate_min;
        double req_rate_max;
        double latency;
    
    // public:
    //     user(int user_id, int loc_x, int loc_y, int slice_id, int act_bs_id, int rb_id,double req_rate, double latency){
    //         this->user_id = user_id;
    //         this->loc_x = loc_x;
    //         this->loc_y  = loc_y;
    //         this->slice_id = slice_id;
    //         this->act_bs_id = act_bs_id;
    //         this->rb_id = rb_id;
    //         this->req_rate = req_rate;
    //         this->latency = latency;
    //     }

    //     int get_user_id(){
    //         return user_id;
    //     }
    //     int get_loc_x(){
    //         return loc_x;
    //     }
    //     int get_loc_y(){
    //         return loc_y;
    //     }
    //     int get_slice_id(){
    //         return slice_id;
    //     }
    //     int get_act_bs_id(){
    //         return act_bs_id;
    //     }
    //     int get_rb_id(){
    //         return rb_id;
    //     }
    //     double get_req_rate(){
    //         return req_rate;
    //     }
    //     double get_latency(){
    //         return latency;
    //     }
};