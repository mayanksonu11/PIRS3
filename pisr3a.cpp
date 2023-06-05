#include<bits/stdc++.h>
#include<cmath>
#include "header.hpp"
#include "common.hpp"
using namespace std;

// double POWER = 100; //dbM
// double RANGE = 100; // (range) in meters
// int MAX_NUM_RBS = 275;

int main()
{

    srand(1);

    env.length=1000; 
    env.breadth=1000; 
    env.num_of_rrh=1+10;
    env.num_of_users=1500;
    env.num_of_slice=1;

    
   
   /****************************************************************************************
    *    Above mentioned parameters are System Level Parameters and it shouldn't be changed*
    ****************************************************************************************/    
    user u1;
    for(int i=0;i<env.num_of_users;i++){
        u1.user_id = i;
        env.user_list.push_back(u1);
    }

    // vector<rrh> rrhs(env.num_of_rrh);
    rrh r1;
    for(int i=0;i<env.num_of_rrh;i++){
        r1.rrh_id = i;
        r1.rrh_type= ((r1.rrh_id == 0) ? 0 : 1) ;
        if(r1.rrh_type==MBS){
            r1.num_rbs = 100;
            r1.max_power = dbm_to_watt(46); //in watt
            r1.range = 700;
        }
        else{
            r1.num_rbs = 50;
            r1.max_power = dbm_to_watt(26); //in dbm
            r1.range = 150;
        }
        env.rrh_list.push_back(r1);
    }

    ifstream file("bs.txt");
    if (file.is_open()) {
        std::string line,posX="",posY="";
        int ind=0;
        while (getline(file, line)){
            int pos = line.find("\t");
            env.rrh_list[ind].loc_x = stod(line.substr(0,8));
            env.rrh_list[ind].loc_y = stod(line.substr(pos+1,8));
            cout <<  env.rrh_list[ind].loc_x  << " " << env.rrh_list[ind].loc_y << endl;
            ind++;
        }
    }
    file.close();
    
    ifstream file1("ue.txt");
    if (file1.is_open()) {
        std::string line,posX="",posY="";
        int ind=0;
        while (getline(file1, line)){
            int pos1 = findNthOccur(line,'\t',1);
            int pos2 = findNthOccur(line,'\t',2);
            env.user_list[ind].loc_x = stod(line.substr(0,8));
            env.user_list[ind].loc_y = stod(line.substr(pos1+1,8));
            int choice = stoi(line.substr(pos2+1,1));
            env.user_list[ind].min_rate_req = BL_assign(choice);
            env.user_list[ind].max_rate_req = EL_assign(choice);
            cout <<  env.user_list[ind].loc_x  << " " << env.user_list[ind].loc_y << " " << choice << endl;
            ind++;
        }
    }
    file1.close();
    
    
    /*
        Upto here I believe system level description of whole system has been done in OOPS
    */


    // Implementation of Preeti's code to find out SINR and SNR for SBS and MBS respectively

    //Let's create an adjacency matrix which shall represent the users in vicinity of the different RRHs    

    vector<vector<int>> adjacency(env.num_of_rrh,vector<int>(env.num_of_users,0));
    vector<vector<double>> UE2BS_dist(env.num_of_rrh,vector<double>(env.num_of_users,0));

    for(int i=0;i<env.num_of_rrh;i++){
        for(int j=0;j<env.num_of_users;j++){
            UE2BS_dist[i][j]= distance1(env.rrh_list[i],env.user_list[j]);
            if(UE2BS_dist[i][j]<=env.rrh_list[i].range){
                adjacency[i][j]=1;
            }
        }
    }

    // vector<double> SINR_MBS = find_SNR_MBS(UE2BS_dist);
    // cout << "SNR_dB" << endl;
    // display(SNR_dB);
    // cin >> wait_flag;

    // cout << "Distance Matrix" << endl;
    // display2d(UE2BS_dist);
    // cin >> wait_flag;
    // cout << "Adjacency Matrix" << endl;
    // display2d(adjacency);
    // cin >> wait_flag;
    // **************We shall now find out SINR_DB for each user from each RRH
    // vector<vector<double>> SINR_BS = find_SINR_sbs(UE2BS_dist); // This gives SINR of all the small base station
    //***************assigning MBS SINR in the SINR
    // for(int i=0;i<env.num_of_users;i++){
    //     SINR_BS[0][i] = SINR_MBS[i];
    // }
    vector<vector<double>> SINR_BS (env.num_of_rrh,vector<double>(env.num_of_users,0));
    find_mse_sinr(SINR_BS);
    cout << "SINR_BS" << endl;
    display2d(SINR_BS);
    // cin >> wait_flag;

    vector<pair<int,double>> best_bs;
    
    for(int i=0;i<env.num_of_users;i++){
        double max_sinr = -1e7;
        int bs_ind = -1;
        for(int j=0;j<env.num_of_rrh;j++){
            if(max_sinr < SINR_BS[j][i]){
                max_sinr = SINR_BS[j][i];
                bs_ind = j;
            }
        }
        best_bs.push_back({bs_ind,max_sinr});
    }
    // for(auto v:best_bs){
    //     cout << "(" << v.first << "," << v.second << "),";
    // }
    // cin >> wait_flag;
    //Evaluation of rate achieved on the different base stations by different users
    vector<vector<double>> rate(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<bool>> Q(env.num_of_rrh,vector<bool>(env.num_of_users,false));
    vector<vector<int>> num_of_RB(env.num_of_rrh,vector<int>(env.num_of_users,0));
    vector<vector<double>> achvble_thput(env.num_of_rrh,vector<double>(env.num_of_users,0));

    ofstream f1;
    f1.open("variable.py");
    vector<user> eligible_users;
    f1 << "def init(num_of_ue,num_of_bs):\n";
    f1 << "\tweight_cost = [[0]*num_of_ue]*num_of_bs\n";
    for(int i=0;i<env.num_of_rrh;i++){
        double BW = 0;
        if(env.rrh_list[i].rrh_type==MBS){
            BW= 0.18;
        }else if(env.rrh_list[i].rrh_type==SBS){
            BW = 0.72;
        }
        f1 << "\tweight_cost[" << i << "] = [";
        for(auto ue:env.user_list ){
            rate[i][ue.user_id] = BW*log2(1+SINR_BS[i][ue.user_id]);
            // cout << rate[i][ue.user_id] << " " << ue.min_rate_req << " " <<  ceil(ue.min_rate_req/rate[i][ue.user_id]) <<endl;
            // cin >> wait_flag; 
            // rij is number of resource block required by UE j from base station i : min_rate_req/rate_per_RB
            num_of_RB[i][ue.user_id] =  ceil(ue.min_rate_req/rate[i][ue.user_id]);
            achvble_thput[i][ue.user_id] = num_of_RB[i][ue.user_id]*rate[i][ue.user_id];
            if(achvble_thput[i][ue.user_id] > ue.max_rate_req && Q[i][ue.user_id]==false){
                Q[i][ue.user_id] = true;
                f1 << "(" << num_of_RB[i][ue.user_id] << "," << achvble_thput[i][ue.user_id] << "," << ue.max_rate_req << "," << ue.min_rate_req << ")";
                eligible_users.push_back(ue);
            }
            else if(achvble_thput[i][ue.user_id] > ue.min_rate_req && Q[i][ue.user_id]==false){
                Q[i][ue.user_id] = true;
                f1 << "(" << num_of_RB[i][ue.user_id] << "," << achvble_thput[i][ue.user_id] << "," << ue.max_rate_req << "," << ue.min_rate_req << ")";
                eligible_users.push_back(ue);
            }
            if(ue.user_id!=env.num_of_users-1){
                f1 << ",";
            }
        }
        f1 << "]" <<endl;
        // cout << eligible_users.size() << endl;
        // cin >> wait_flag;
        // eligible_users.clear();

    }
    f1 << "\treturn weight_cost\n";
    f1.close();

    cout << "List of eligible users:" << eligible_users.size() << endl;
    
    // Pj is data rate per resource block 


    // User association to the Highest SINR base station... 
    //Choice-1: connect to the base station with highest SINR
    // for(int j=0;j<env.num_of_users;j++){
    //     int key=0; double max_sinr_db=0;
    //     for(int i=0;i<env.num_of_rrh;i++){
    //         if(SINR_dB[i][j]>max_sinr_db and adjacency[i][j]==1){
    //             max_sinr_db = SINR_dB[i][j];
    //             key = i;
    //         }
    //     }
    //     env.user_list[j].act_bs_id = key;
    // }

    //Choice-2: Connect to the base station as per PIRS3 strategy


    // for(int j=0;j<env.num_of_users;j++){
    //     cout << env.user_list[j].user_id << " " << env.user_list[j].act_bs_id << "|";
    // }


    //********************************************
    // Highly use case dependent --- Dangerous to use it anywhere else
    //Resource block assignment /
    /*
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
    */
    // vector<pair<double,int>> temp_SINR;
    // for(int i=0;i<env.)
    // sort(temp.begin(),temp.end(),)
    ///*******************************************
    // display2d(SIG_dB_sbs);
    // display2d(SIG_SBS);

    cout << "\nHello World!" << endl;
}