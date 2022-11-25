#include<vector>
#include "rrh.hpp"
#include "user.hpp"
#include "slice1.hpp"
using namespace std;

class environment{
    public:
        int length,breadth;
        int num_of_rrh;
        int num_of_sbs;
        int num_of_users;
        int num_of_slice;
        vector<rrh> rrh_list;
        vector<user> user_list;
        vector<slice1> slice_list;
};