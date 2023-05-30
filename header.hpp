#include<bits/stdc++.h>
#include <fstream>
#include <cmath>
#include "environment.hpp"
using namespace std;

int wait_flag;
environment env;
default_random_engine generator;

double distance1(rrh J, user U){
    return sqrt(pow((J.loc_x-U.loc_x),2)+pow((J.loc_y-U.loc_y),2));
}

int findNthOccur(string str, char ch, int N)
{
    int occur = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ch) {
            occur += 1;
        }
        if (occur == N)
            return i;
    }
    return -1;
}

template <typename t>
void display(vector<t> arr){
    for(int i=0;i<arr.size();i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}

template <typename t>
t sum_array(vector<t> arr){
    double sum = 0;
    for(int i=0;i<arr.size();i++){
        sum += arr[i];
    }
    return sum;
}

template <typename t>
void display2d(vector<vector<t>> arr){
	for(int i=0;i<arr.size();i++){
		for(int j=0;j<arr[i].size();j++)
			cout <<arr[i][j] << " ";
		cout << endl;
	}
}

vector<vector<int>> clusterification(environment env, vector<user> users, vector<rrh> rrhs, int num_of_rbs)
{
    vector<vector<int>> clusters(num_of_rbs);
    for(int i=0;i<env.num_of_users;i++){
        clusters[users[i].rb_id].push_back(users[i].user_id);
    }
    return clusters;
}

double dbm_to_watt(double val){
    double res =0;
    res = val - 30;
    res = pow(10,res/10);
    return res;
}

double BL_assign(int choice){
    if(choice==0)
        return 4;
    else if(choice==1)
        return 2;
    else
        return 1;
}

double EL_assign(int choice){
    if(choice==0)
        return 12;
    else if(choice==1)
        return 10;
    else
        return 9;
}

void random_factor_mbs(int lambda_user, vector<double> &X_LOS_list,vector<double> &X_NLOS_list,vector<double> &z_list){
    int m = 1;
    double v_los = pow(10,0.4);
    double v_nlos = pow(10,0.6);
    double mu_los = log(pow(m,2)/sqrt(v_los+pow(m,2))); 
    double sigma_los = sqrt(log(v_los/pow(m,2)+1)); 
    double mu_nlos = log(pow(m,2)/sqrt(v_nlos+pow(m,2)));
    double sigma_nlos = sqrt(log(v_nlos/pow(m,2)+1));

    lognormal_distribution<double> distribution_logn_los(mu_los,sigma_los);
    lognormal_distribution<double> distribution_logn_nlos(mu_nlos,sigma_nlos);
    exponential_distribution<double> distribution_exp(1);
    for(int i=0;i<lambda_user;i++){
        X_LOS_list[i] = distribution_logn_los(generator);
        X_NLOS_list[i] = distribution_logn_nlos(generator);
        z_list[i] = distribution_exp(generator);
    }
}

void random_factor_sbs(int lambda_user, int num_sbs, vector<vector<double>> &x_list_sbs,vector<vector<double>> &z_list_sbs,vector<vector<double>> &R_list){
    double var_dB=5.8;
    double v=pow(10,(var_dB/10));
    double m = 1; // mean
    //v = 2; // variance
    double mu = log(pow(m,2)/sqrt(v+pow(m,2)));
    double sigma = sqrt(log(v/pow(m,2)+1));
    lognormal_distribution<double> distribution_logn(mu,sigma);
    exponential_distribution<double> distribution_exp(1);

    for (int s=1;s<num_sbs;s++){
        for(int j=0;j<lambda_user;j++){   
            
            x_list_sbs[s][j] = distribution_logn(generator);
            z_list_sbs[s][j] = distribution_exp(generator); 
            v=5;
            gamma_distribution<double> distribution_gamma(v,1/v);
            R_list[s][j]= distribution_gamma(generator);
        }
    }
            
}

vector<double> find_SNR_MBS(vector<vector<double>> UE2BS_dist){
    vector<double> x_LOS_list(env.num_of_users);
    vector<double> x_NLOS_list(env.num_of_users);
    vector<double> z_list(env.num_of_users);
    vector<double> PL_LOS(env.num_of_users);
    vector<double> Tot_gain_LOS(env.num_of_users);
    vector<double> SINRdB_LOS(env.num_of_users);
    vector<double> SNR_LOS_mbs_move(env.num_of_users);
    vector<double> SNRdB_LOS_mbs(env.num_of_users);
    
    random_factor_mbs(env.num_of_users, x_LOS_list, x_NLOS_list, z_list);

    // cout << "x_LOS_list" << endl;
    // display(x_LOS_list);
    // cout << "x_NLOS_list" << endl;
    // display(x_NLOS_list);
    // cout << "z_list" << endl;
    // display(z_list);
    // cin >> wait_flag;
    for(auto ue:env.user_list){
        double MS_dist = UE2BS_dist[0][ue.user_id];
        double h_BS=25; // height of BS
        double h_UT=2.5; // height of user
        double fc=2.8;// Centre frequency is expressed in GHz 
        double c=3e8; 
        double h_E=1; // height of common ground
        double d_BP=4*(h_BS-h_E)*(h_UT-h_E)*(fc/c)*pow(10,9); // effective distance friss free space eq
        double K=pow(10,(-2)); // some constant 
        double NRB_tot=273;
        double G_T=10; // transmitter gain
        double G_R=10; // antenna gain
        double x_LOS = x_LOS_list[ue.user_id]; // used for channel modelling
        double x_NLOS = x_NLOS_list[ue.user_id];
        double z = z_list[ue.user_id];
        double G_LOS=10*log10(K*x_LOS*z);
        double G_NLOS=10*log10(K*x_NLOS*z);
        double d_3D=sqrt(pow((MS_dist*1000),2)+pow((h_BS-h_UT),2));
        double PL_NLOS1=13.54+39.08*log10(d_3D)+20*log(fc)-0.6*(h_UT-1.5);
        // cout << d_3D << " " << fc << " " << d_BP << " " << h_BS << " " << h_UT << endl;
        if((MS_dist*1000)<=d_BP){
            PL_LOS[ue.user_id]=28+22*log10(d_3D)+20*log10(fc);
            Tot_gain_LOS[ue.user_id]=G_LOS-PL_LOS[ue.user_id];
        }
        else{
            PL_LOS[ue.user_id]=28.0+40*log10(d_3D)+20*log10(fc)-9*log10(pow(d_BP,2)+pow(h_BS-h_UT,2));
            Tot_gain_LOS[ue.user_id]=G_LOS-PL_LOS[ue.user_id];
        }

        PL_NLOS1 =max(PL_LOS[ue.user_id],PL_NLOS1);

        Tot_gain_LOS[ue.user_id] = G_LOS-PL_LOS[ue.user_id];
        double Tot_gain_NLOS1 = G_NLOS-PL_NLOS1;

        if (MS_dist <0.6){
            //Calculating power per RB: Total Tx power of BS=46dBm and initially power is equally divided among the total RBs noise power per RB bandwidth=-148.365 dB
            SNR_LOS_mbs_move[ue.user_id]=((pow(10,(16/10.0))/NRB_tot)*pow(10,(Tot_gain_NLOS1/10))*G_T*G_R)/(pow(10,-(14.84)));//Considering only the noise PSD -174dBm/Hz, noise power per RB=-148.365dB
            SNRdB_LOS_mbs[ue.user_id]=10*log10(SNR_LOS_mbs_move[ue.user_id]);
        }
        else{
            //Calculating power per RB: Total Tx power of BS=46dBm and initially power is equally divided among the total RBs noise power per RB bandwidth=-148.365 dB
            SNR_LOS_mbs_move[ue.user_id] =((pow(10,(16/10.0))/NRB_tot)*pow(10,(Tot_gain_NLOS1/10))*G_T*G_R)/(pow(10,-(14.84)));      //Considering only the noise PSD -174dBm/Hz, noise power per RB=-148.365dB
            SNRdB_LOS_mbs[ue.user_id] =10*log10(SNR_LOS_mbs_move[ue.user_id]);//converting to dB
        }
    }
    // cout << "SNR_LOS_mbs_move" << endl;
    // display(SNR_LOS_mbs_move);
    // cin >> wait_flag;
    return SNR_LOS_mbs_move;
}

vector<vector<double>> find_SINR_sbs(vector<vector<double>> UE2BS_dist){
    vector<vector<double>> x_list_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> z_list_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> R_list(env.num_of_rrh, vector<double>(env.num_of_users,0));
    random_factor_sbs(env.num_of_users,env.num_of_rrh,x_list_sbs,z_list_sbs,R_list);
    vector<vector<double>> SNR_LOS_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> SNRdB_LOS_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    
    for(int i=1;i<env.num_of_rrh;i++){
        int NRB_sbs = env.rrh_list[i].num_rbs;
        for(int j=0;j<env.num_of_users;j++){
            double distance_SBS = UE2BS_dist[i][j];
            double p_t_RB=1/(double)NRB_sbs;// dont know 
            // double p_t_RB=1;    
            double p_t_dB=10*log10(p_t_RB);
            double alpha=61.4; // some constant
            double beta=2;
            double var_dB=5.8;// not used
            double x = x_list_sbs[i][j];// parameter used in eq
            double z = z_list_sbs[i][j];
            double z_dB=10*log10(z);
            double x_dB=10*log10(x);
            double PL = alpha+beta*10*log10(distance_SBS*1000)+x_dB;// path loss
            double f=28e9;// millimeter wave frequency 
            double lamda=(3e8/f);// wavelength
            double L_t=0.009; 
            double L_r=0.009;
            double G_t=20*log10(M_PI*L_t/lamda);
            double G_r=20*log10(M_PI*L_r/lamda);

            //%% hi^2 generation 

            // v=5;
            // R = gamrnd(v,(1/v));
            double R  = R_list[i][j];
            double var_gamma= 10*log10(abs(R));

           //noise_dB = -204+10*log10(60*(10^3)*12)+10;

            double noise_dB = -204+10*log10(200*pow(10,6))+10;//standard vaue of noise
           //SNRdB_LOS_sbs(j)=+p_t_dB+G_t+G_r-PL-noise_dB;
            SNRdB_LOS_sbs[i][j]=var_gamma+p_t_dB+G_t+G_r-PL-noise_dB;// signal to noise ratio line of sight of small base station
            SNR_LOS_sbs[i][j]= pow(10,(SNRdB_LOS_sbs[i][j]/10)); // changing unit
        }
    }
    vector<vector<double>> interference(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> signal(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> SINR_LOS_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    vector<vector<double>> SINRdB_LOS_sbs(env.num_of_rrh, vector<double>(env.num_of_users,0));
    for (int s=1; s < env.num_of_rrh ; s++){
        vector<double> temp(env.num_of_rrh,0);
        for(int i=0; i < env.num_of_users; i++){
            double noise_dB = -204+10*log10(200*pow(10,6))+10; 
            double noise = pow(10,(noise_dB/10));
            for(int k=1;k<env.num_of_rrh;k++){
                temp[k] = SNR_LOS_sbs[k][i]*noise;
            }
            temp[s] = 0;
            interference[s][i] =  sum_array(temp); // interference using snr los of sbs
            signal[s][i] = SNR_LOS_sbs[s][i]*noise; // signal 
            SINR_LOS_sbs[s][i] = signal[s][i]/(interference[s][i]+noise);// SINR calculation
            SINRdB_LOS_sbs[s][i] = 10*log10(SINR_LOS_sbs[s][i]);// changing unit
        }
    }
    return SINR_LOS_sbs;
}

#if 0
vector<vector<double>> find_SINR(environment env, vector<user> users, vector<rrh> rrhs, vector<vector<double>> UE2BS_dist){
    vector<vector<double>> SINR(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SINR_db(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SIG_db_BS(env.num_of_rrh,vector<double>(env.num_of_users,0));
    vector<vector<double>> SIG_BS(env.num_of_rrh,vector<double>(env.num_of_users,0));
    // default_random_engine generator2;
    for(int i=0;i<env.num_of_rrh;i++){
        for(int j=0;j<env.num_of_users;j++){
            double dist = UE2BS_dist[i][j];
            double p_t_RB = rrhs[i].max_power/(double)rrhs[i].num_rbs;
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
            SIG_db_BS[i][j] = var_gamma + p_t_dB + G_t + G_r - PL;
            SIG_BS[i][j] = pow(10,(SIG_db_BS[i][j] / 10));
        }
    }
    cout<< "Channel gain:" << endl;
    display2d(SIG_BS);
    cin >> wait_flag;
    double noise = pow(10,-204.0/10);
    for(int j=0;j<env.num_of_users;j++){
        for(int i=0;i<env.num_of_rrh;i++){
            double I = 0; 
            for(int k=0;k<env.num_of_rrh;k++){
                I += SIG_BS[k][j]; 
            }
            SINR[i][j] = SIG_BS[i][j]/(I - SIG_BS[i][j] + noise); 
            SINR_db[i][j] = log10(SINR[i][j]); 
        }
    }
    cout <<"SINR below" << endl;
    display2d(SINR);
    return SINR_db;
}
#endif

void find_mse_sinr(vector<vector<double>> &SINR_BS){
    ifstream file1("SINR_base.txt");
    if (file1.is_open()) {
        std::string line;
        int ind=0;
        while (getline(file1, line)){
            int pos1 = -1;
            for(int i=0;i<env.num_of_users;i++){
                double temp_sinr = stod(line.substr(pos1+1,12));
                SINR_BS[ind][i] = temp_sinr;
                pos1 = findNthOccur(line,'\t',i+1);
            }
            ind++;
        }
    }
    file1.close();
}