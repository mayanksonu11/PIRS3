#include<bits/stdc++.h>
#include "environment.hpp"
using namespace std;

double POWER = 100; //dbM
double RANGE = 200; // (range) in meters
int MAX_NUM_RBS = 275;


double distance1(rrh J, user U){
    return sqrt(pow((J.loc_x-U.loc_x),2)+pow((J.loc_y-U.loc_y),2));
}

template <typename t>
void display2d(vector<vector<t>> arr){
	for(int i=0;i<arr.size();i++){
		for(int j=0;j<arr[i].size();j++)
			cout <<arr[i][j] << " ";
		cout << endl;
	}
}

vector<vector<int>> clusterification(environment env, vector<user> users, vector<rrh> rrhs)
{
    vector<vector<int>> clusters(MAX_NUM_RBS);
    for(int i=0;i<env.num_of_users;i++){
        clusters[users[i].rb_id].push_back(users[i].user_id);
    }
    return clusters;
}

vector<vector<double>> find_SINR(environment env, vector<user> users, vector<rrh> rrhs, vector<vector<double>> UE2SBS_dist){
    vector<vector<double>> SINR(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SINR_db(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SIG_dB_sbs(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SIG_SBS(env.num_of_rrh,vector<double>(env.num_of_users,0));
    default_random_engine generator;
    // default_random_engine generator2;
    for(int i=0;i<env.num_of_rrh;i++){
        for(int j=0;j<env.num_of_users;j++){
            double dist = UE2SBS_dist[i][j];
            double p_t_RB = 1/(double)rrhs[i].num_rbs;
            double p_t_dB = 10*log10(p_t_RB);
            double alpha = 61.4;
            double beta = 2;
            double var_db = 5.8;
            double v = pow(10,(var_db/10)); // in watt
            double m =1;
            double mu = log10(pow(m,2)/sqrt(v+pow(m,2)));
            double sigma = sqrt(log(v/pow(m,2)+1));
            lognormal_distribution<double> distribution_logn(mu,sigma);
            double x = distribution_logn(generator);
            double x_db = 10*log10(x);
            double PL = alpha + beta*10*log10(dist) + x_db;
            // cout << alpha << " " << beta << " "<< dist << " " << x_db << " " << PL << endl;
            double f= 28e9;
            double lambda = (3e8/f);
            double L_t = 0.009; 
            double L_r = 0.009;
            double G_t = 20*log10(M_PI*L_t/lambda);
            double G_r = 20*log10(M_PI*L_r/lambda);
            gamma_distribution<double> distribution_gamma(v,1/v);
            double R = distribution_gamma(generator);
            double var_gamma = 10*log10(abs(R));
            // double noise_db = -204;
            // cout << var_gamma << " " << p_t_dB << " " << G_t << " " << G_r << " " << PL << endl;
            SIG_dB_sbs[i][j] = var_gamma + p_t_dB + G_t + G_r - PL;
            SIG_SBS[i][j] = pow(10,(SIG_dB_sbs[i][j] / 10));
        }
    }
    cout<< "Channel gain:" << endl;
    display2d(SIG_SBS);
    double noise = pow(10,-204.0/10);
    for(int j=0;j<env.num_of_users;j++){
        for(int i=0;i<env.num_of_rrh;i++){
            double I = 0; 
            for(int k=0;k<env.num_of_rrh;k++){
                I += SIG_SBS[k][j]; 
            }
            SINR[i][j] = SIG_SBS[i][j]/(I - SIG_SBS[i][j] + noise); 
            SINR_db[i][j] = log10(SINR[i][j]); 
        }
    }
    cout <<"SINR in DB below" << endl;
    display2d(SINR_db);
    return SINR_db;
}
