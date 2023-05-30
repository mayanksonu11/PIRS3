#include<bits/stdc++.h>
#include<cmath>
#include "header.hpp"
using namespace std;

// double POWER = 100; //dbM
// double RANGE = 100; // (range) in meters
// int MAX_NUM_RBS = 275;

int main()
{
    environment env;
    env.length=100; 
    env.breadth=100; 
    env.num_of_rrh=1;
    env.num_of_users=100;
    env.num_of_slice=1;

    
   
    /*
        Above mentioned parameters are System Level Parameters and it shouldn't be changed 
    */    
    srand(1);
    // vector<user> users(env.num_of_users);
    user u1;
    for(int i=0;i<env.num_of_users;i++){
        u1.user_id = i;
        u1.loc_x = rand()%env.length;
        u1.loc_y = rand()%env.breadth;
        // u1.slice_id = rand()%env.num_of_slice;
        // u1.rbs = 0; //rand()%275;
        u1.min_rate_req = (rand()%100)/10.0;
        // u1.req_rate_max = u1.req_rate_min + rand()%10;
        u1.latency = rand()%10;
        env.user_list.push_back(u1);

    }

    // vector<rrh> rrhs(env.num_of_rrh);
    rrh r1;
    for(int i=0;i<env.num_of_rrh;i++){
        r1.rrh_id = i;
        // r1.loc_x = env.user_list[i].loc_x + rand() %(int)RANGE;//rand()%env.length;
        // r1.loc_y = env.user_list[i].loc_y + rand() %(int)RANGE;//rand()%env.breadth;
        r1.loc_x = env.length/2;
        r1.loc_y = env.breadth/2; // Take care of this :: potential error
        r1.range = RANGE;
        r1.num_rbs = 275;
        // r1.rrh_type="MBS";
        MAX_NUM_RBS = max(r1.num_rbs,MAX_NUM_RBS);
        for(int j=0;j<r1.num_rbs;j++){
            rb temp_rb;
            temp_rb.power = POWER/r1.num_rbs;
            r1.rbs.push_back(temp_rb); 
            // few parameters undefined
        }
        env.rrh_list.push_back(r1);
    }
    
    slice1 s1;
    for(int i=0;i<env.num_of_slice;i++){
        s1.slice_id= i;
        s1.num_of_sub_slice = 3;
        sub_slice sub1;
        sub1.min_rate = 0;
        sub1.max_rate= 3;
        s1.sub_slice_list.push_back(sub1);
        sub1.min_rate = 3;
        sub1.max_rate= 6;
        s1.sub_slice_list.push_back(sub1);
        sub1.min_rate = 6;
        sub1.max_rate= 10;
        s1.sub_slice_list.push_back(sub1);
    }
    /*
        Upto here I believe system level description of whole system has been done in OOPS
    */

    //Let's create an adjacency matrix which shall represent the users in vicinity of the different RRHs    

    vector<vector<int>> adjacency(env.num_of_rrh,vector<int>(env.num_of_users,0));
    vector<vector<double>> UE2SBS_dist(env.num_of_rrh,vector<double>(env.num_of_users,0));

    for(int i=0;i<env.num_of_rrh;i++){
        for(int j=0;j<env.num_of_users;j++){
            UE2SBS_dist[i][j]= distance1(env.rrh_list[i],env.user_list[j]);
            // cout << rrhs[i].loc_x << " " << users[j].loc_x << " " << rrhs[i].loc_y << " " << users[j].loc_y << endl;
            // cout << UE2SBS_dist[i][j] << endl;
            if(UE2SBS_dist[i][j]<=env.rrh_list[i].range){
                adjacency[i][j]=1;
            }
        }
    }
    // display2d(adjacency);
    // display_cluster(clusterification(env,users,rrhs));
    // We shall now find out SINR_DB for each user from each RRH
    vector<vector<double>> SINR_dB = find_SINR(env,env.user_list,env.rrh_list,UE2SBS_dist);
    // cout << "SINR_DB" << endl;
    // display2d(SINR_dB);
    for(int j=0;j<env.num_of_users;j++){
        int key=0; double max_sinr_db=0;
        for(int i=0;i<env.num_of_rrh;i++){
            if(SINR_dB[i][j]>max_sinr_db and adjacency[i][j]==1){
                max_sinr_db = SINR_dB[i][j];
                key = i;
            }
        }
        env.user_list[j].act_bs_id = key;
        // cout << users[j].user_id << " " <<10*log10(max_sinr) << endl;
    }

    for(int j=0;j<env.num_of_users;j++){
        cout << env.user_list[j].user_id << " " << env.user_list[j].act_bs_id << "|";
    }
    //********************************************
    // Highly use case dependent --- Dangerous to use it anywhere else
    //Resource block assignment /
    double mu_e=0.511;
    double r_min=10;
    int available_rb = env.rrh_list[0].num_rbs;
    for(int i=0;i<env.num_of_users;i++){
        int bs_id = env.user_list[i].act_bs_id;
        int u_id = env.user_list[i].user_id;
        int num_of_rb = ceil(r_min/(mu_e*SINR_dB[bs_id][u_id]));
        available_rb -= num_of_rb;
        if(available_rb<0)
            break;
        env.user_list[i].num_of_rb = num_of_rb;
    }
    cout << endl;
    for(int i=0;i<env.num_of_users;i++){
        cout << env.user_list[i].num_of_rb << " ";
    }
    cout << "\nAvailable RB=" << available_rb << endl;
    while(available_rb>0){
        for(int i=0;i<env.num_of_users;i++){
            env.user_list[i].num_of_rb++;
            available_rb--;
            if(available_rb==0){
                break;
            }
        }
    }
    cout << "\nAvailable RB=" << available_rb << endl;
    for(int i=0;i<env.num_of_users;i++){
        cout << env.user_list[i].num_of_rb << " ";
    }
    // vector<pair<double,int>> temp_SINR;
    // for(int i=0;i<env.)
    // sort(temp.begin(),temp.end(),)
    ///*******************************************
    // display2d(SIG_dB_sbs);
    // display2d(SIG_SBS);

    cout << "\nHello World!" << endl;
}