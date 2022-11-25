#include<vector>
#include "sub_slice.hpp"
using namespace std;

class slice1{
    public:
        int slice_id;
        int num_of_sub_slice;
        vector<sub_slice> sub_slice_list; 
    // public:
    //     slice1(int num_of_sub_slice){
    //         this->num_of_sub_slice = num_of_sub_slice;
    //     }

    //     int get_num_of_sub_slice(){
    //         return num_of_sub_slice;
    //     }
    //     vector<sub_slice> get_sub_slice_list(){
    //         return sub_slice_list;
    //     }

    //     void add_sub_slice(sub_slice s1){
    //         this->sub_slice_list.push_back(s1);
    //     }
};
