#include <iostream>
#include <math.h>
#include <omp.h>
#include <ctime>
#include "mMath.h"
#include "Env.h"
#include <fstream>
#include <string>
#include <ios>



namespace py = pybind11;
using namespace std;

//namespace p = boost::python;
//namespace np = boost::python::numpy;

//namespace p = pybind11::literals;
//namespace np = pybind11::numpy;

#define PI 3.141592

#define w_target 4
#define w_col -4.0
#define w_smooth_v -0.01
#define w_smooth_w -0.01
#define w_smooth_acceleration -0.006
#define w_pref_v 0.0
#define w_pref_w -0.0
#define w_pref_dir 0.02
//#define w_comfort_dist 0.006
#define w_comfort_dist 0.01

int countAgent = 0;
int i;
float collision_dist_multiplier_agent = 1.0;
double collision_dist_multiplier_obstacle = 1.0;

#define _EPSILON 0.00001
#define tao0 3.0
#define LONG_RANGE_STIFFNESS 0.05
//#define MAX_ACCEL 0.25f   // #define MAX_ACCEL 0.09f*0.7f 
#define MAX_ACCEL 0.09*0.35f 
#define C_LONG_RANGE_CONSTRAINT  0.5 * C_MAX_ACCELERATION
#define C_TAU_MAX 20

Env::Env()
{
//    py::initialize();
//	 np::initialize();
//	Py_Initialize();

	srand((unsigned int)time(0));
	cout << "Env()" << endl;
}

Env::~Env()
{
	cout << "~Env()" << endl;
}

const vector<Agent*> & Env::Observe()
{	
	#pragma omp parallel for
	for(int i=0; i<agent_num; i++)
	{
		Agent* cur_agent;
		cur_agent = _agents.at(i);
		#pragma omp parallel for
		for(int j=0; j<agent_num; j++)
		{
			if(i==j)
				continue;

			Agent* other_agent;
			other_agent = _agents.at(j);

			double other_data[7];
			other_agent->getData(other_data);
			cur_agent->setVision(other_data, false);
		}

		#pragma omp parallel for
		for(int j=0; j<obstacle_num; j++)
		{
			Obstacle* obs;
			obs = _obstacles.at(j);

			double obs_data[7];
			obs->getData(obs_data);
			cur_agent->setVision(obs_data, false);
		}

		#pragma omp parallel for
		for(int j=0; j<wall_num; j++)
		{
			Wall* wall;
			wall = _walls.at(j);

			vector<Edge*> edges = wall->getEdges();
			for(int k=0; k<edges.size(); k++){
				double edge_data[4];
				edge_data[0] = edges.at(k)->getSt()[0];
				edge_data[1] = edges.at(k)->getSt()[1];
				edge_data[2] = edges.at(k)->getEd()[0];
				edge_data[3] = edges.at(k)->getEd()[1];

				cur_agent->setVision(edge_data, true);
			}
		}
	}

	return _agents;
}


/*
void RevertAgent(Agent* cur, Agent* other)
{
		double correction_x1_agent1 = 0.0;
		double correction_y1_agent1 = 0.0;
		double correction_x2_agent2 = 0.0;
		double correction_y2_agent2 = 0.0;

		double inv_mass_particle1 = 1;
		double inv_mass_particle2 = 1;

		//double cur_agent_mass = cur->getAgentMass();    //TO ADD MASS IN AGENTS' STATE SPACE
		//double other_agent_mass = other->getAgentMass();
		//cout<<"cur_agent_mass: " <<cur_agent_mass<< ", other_agent_mass: "<< other_agent_mass <<"\n\n";
		//double inv_mass_particle1 = 1/cur_agent_mass;   //TO ADD MASS IN AGENTS' STATE SPACE
		//double inv_mass_particle2 = 1/other_agent_mass;  //TO ADD MASS IN AGENTS' STATE SPACE

		double *cur_agent_pos = cur->getP();
		double *other_agent_pos = other->getP();
		double *cur_agent_r = cur->getR();
		double *other_agent_r = other->getR();

		double dis1 = Dist(cur_agent_pos, other_agent_pos);

        double coef1_Agent1 = inv_mass_particle1 / (inv_mass_particle1 + inv_mass_particle2);
		double coef1_Agent2 = inv_mass_particle2 / (inv_mass_particle1 + inv_mass_particle2);

		//cout<<"\ncoef1_Agent1: " << coef1_Agent1<< ", coef1_Agent2: " <<coef1_Agent2<< "\n";
		//cout<<"Agent1 ID: " << cur->getId() << ", " << cur->getAgentMass() << ", Agent2 ID: " << other->getId() << ", " << other->getAgentMass()<< "\n\n";

		//double coef2 = (dis1 - cur_agent_r[0] - other_agent_r[0])/dis1;
		double coef2 = (dis1 - collision_dist_multiplier_agent * (cur_agent_r[0] + other_agent_r[0]))/dis1;

		//if((dis1 <= collision_dist_multiplier_agent * (cur_agent_r[0] + other_agent_r[0])) && (dis1 != 0)){
		correction_x1_agent1 = -coef1_Agent1 * coef2 * (cur_agent_pos[0] - other_agent_pos[0]);
		correction_y1_agent1 = -coef1_Agent1 * coef2 * (cur_agent_pos[1] - other_agent_pos[1]);
		correction_x2_agent2 =  coef1_Agent2 * coef2 * (cur_agent_pos[0] - other_agent_pos[0]);
		correction_y2_agent2 =  coef1_Agent2 * coef2 * (cur_agent_pos[1] - other_agent_pos[1]);
		//}

		cout<<"cur_agent_pos[0]: " << cur_agent_pos[0] <<", cur_agent_pos[1]: "<< cur_agent_pos[1] <<"\n";
		cur->setP(cur_agent_pos[0] + correction_x1_agent1, cur_agent_pos[1] + correction_y1_agent1);
		cout<<"After, cur_agent_pos[0]: " << cur_agent_pos[0] <<", cur_agent_pos[1]: "<< cur_agent_pos[1] <<"\n\n";
		other->setP(other_agent_pos[0] + correction_x2_agent2, other_agent_pos[1] + correction_y2_agent2);
	//}
}
*/


void clamp(double* v, double maxValue) {
  float lengthV = sqrtf(v[0] * v[0] + v[1] * v[1]);
  if (lengthV > maxValue) {                  // if (lengthV > C_LONG_RANGE_CONSTRAINT) { 
    float mult = (maxValue / lengthV);     // mult = (C_LONG_RANGE_CONSTRAINT / lengthV)
    v[0] *= mult;
    v[1] *= mult;
//cout<<"From clamp(), v[0]: " << v[0] <<", v[1]: "<< v[1] <<"\n";
  }
}


void RevertAgent(Agent* cur, Agent* other)
{
		double* correction_agent1;
		double* correction_agent2;

		double inv_mass_particle1 = 1;
		double inv_mass_particle2 = 1;

		double *cur_agent_pos = cur->getP();
		double *other_agent_pos = other->getP();
		double *cur_agent_r = cur->getR();
		double *other_agent_r = other->getR();

		double *cur_agent_oldPos = cur->getPprev();
		double *other_agent_oldPos = other->getPprev();

		double dis1 = Dist(cur_agent_pos, other_agent_pos);

        double coef1_Agent1 = inv_mass_particle1 / (inv_mass_particle1 + inv_mass_particle2);
		double coef1_Agent2 = inv_mass_particle2 / (inv_mass_particle1 + inv_mass_particle2);

		double coef2 = (dis1 - collision_dist_multiplier_agent * (cur_agent_r[0] + other_agent_r[0]))/dis1;

		double radius_init = cur_agent_r[0] + other_agent_r[0];
    		double radius_sq_init = radius_init * radius_init;
		double radius_sq = radius_sq_init;
	    	if (dis1 < radius_init) {
	    	radius_sq = (radius_init - dis1) * (radius_init - dis1);
	    	}

		double v_x = (cur_agent_pos[0] - cur_agent_oldPos[0]) / cur->getTimeStep() - (other_agent_pos[0] - other_agent_oldPos[0])/ cur->getTimeStep();
		double v_y = (cur_agent_pos[1] - cur_agent_oldPos[1]) / cur->getTimeStep() - (other_agent_pos[1] - other_agent_oldPos[1])/ cur->getTimeStep();
		
		//double x0 = cur_agent_pos[0] - other_agent_pos[0];   // x0 = cur_agent_oldPos[0] - other_agent_oldPos[0]
		//double y0 = cur_agent_pos[1] - other_agent_pos[1];   // x0 = cur_agent_oldPos[1] - other_agent_oldPos[1]
		double x0 = cur_agent_oldPos[0] - other_agent_oldPos[0];   // changed
		double y0 = cur_agent_oldPos[1] - other_agent_oldPos[1];   // changed

		double v_sq = v_x * v_x + v_y * v_y;
		double x0_sq = x0 * x0;
		double y0_sq = y0 * y0;
		double x_sq = x0_sq + y0_sq;
		double a = v_sq;
		double b = -v_x * x0 - v_y * y0;   // b = -1 * v_.dot(x0_).  Have to check this. 
		double b_sq = b * b;
		double c = x_sq - radius_sq;
		double d_sq = b_sq - a * c;
		double d = sqrtf(d_sq);
		double tao = (b - d) / a;
		//cout<<"tao: " << tao <<"\n";

		double dv_i = 1.0;    //changed
    		double dv_j = -1.0;   //changed

		//double max_acceleration = cur->getTimeStep() * MAX_ACCEL;
		double max_acceleration = MAX_ACCEL;

		if (d_sq > 0.0 and abs(a) > 0.0001 and tao > 0 and tao < C_TAU_MAX){
			float clamp_tao = exp(-tao * tao / tao0);
			float c_tao = abs(tao - tao0);
			float tao_sq = c_tao * c_tao;
			float grad_x_i = 2 * c_tao * ((dv_i / a) * ((-2. * v_x * tao) - (x0 + (v_y * x0 * y0 + v_x * (radius_sq - y0_sq)) / d)));
			float grad_y_i = 2 * c_tao * ((dv_i / a) * ((-2. * v_y * tao) - (y0 + (v_x * x0 * y0 + v_y * (radius_sq - x0_sq)) / d)));
			float grad_x_j = -grad_x_i;
			float grad_y_j = -grad_y_i;
			float stiff = exp(-tao * tao / 25);    //changed
			float s = stiff * tao_sq / (inv_mass_particle1 * (grad_y_i * grad_y_i + grad_x_i * grad_x_i) + inv_mass_particle2 * (grad_y_j * grad_y_j + grad_x_j * grad_x_j));     //changed
			correction_agent1[0] = s * coef1_Agent1 * grad_x_i;
			correction_agent1[1] =  s * coef1_Agent1 * grad_y_i;
			clamp(correction_agent1, max_acceleration);
			correction_agent2[0] =  s * coef1_Agent2 * grad_x_j;;
			correction_agent2[1] =  s * coef1_Agent2 * grad_y_j;
			clamp(correction_agent2, max_acceleration);			
	  }
	  //}
		//cout<<"cur_agent_pos[0]: " << cur_agent_pos[0] <<", cur_agent_pos[1]: "<< cur_agent_pos[1] <<"\n\n";
		cur->setP(cur_agent_pos[0] + correction_agent1[0], cur_agent_pos[1] + correction_agent1[1]);
		//cout<<"After, cur_agent_pos[0]: " << cur_agent_pos[0] <<", cur_agent_pos[1]: "<< cur_agent_pos[1] <<"\n";
		other->setP(other_agent_pos[0] + correction_agent2[0], other_agent_pos[1] + correction_agent2[1]);
	//}
}

//USING THIS TO RESOLVE AGENT-OBSTACLE COLLISION
void RevertObstacle(Agent* cur, Obstacle* obsta)
{
		double correction_x1_obstacle = 0.0;
		double correction_y1_obstacle = 0.0;
		double correction_x2_agent = 0.0;
		double correction_y2_agent = 0.0;

		double *agent_pos = cur->getP();
		double *obstacle_pos = obsta->getP();
		double *agent_r = cur->getR();
		double *obstacle_r = obsta->getR();

		double dis1 = Dist(agent_pos, obstacle_pos);

		double inv_mass_obstacle = 0;
		double inv_mass_agent = 1;

		double coef1_obstacle = inv_mass_obstacle / (inv_mass_obstacle + inv_mass_agent);
		double coef1_agent = inv_mass_agent / (inv_mass_obstacle + inv_mass_agent);

		//double coef2 = (dis1 - r1[0] - r2[0])/dis1;
		double coef2 = (dis1 - collision_dist_multiplier_obstacle * (agent_r[0] + obstacle_r[0]))/dis1;
		
		//if((dis1 <= collision_dist_multiplier_obstacle * (r1[0] + r2[0])) && (dis1 != 0)){
		correction_x1_obstacle = -coef1_obstacle * coef2 * (obstacle_pos[0] - agent_pos[0]);
		correction_y1_obstacle = -coef1_obstacle * coef2 * (obstacle_pos[1] - agent_pos[1]);
		correction_x2_agent =  coef1_agent * coef2 * (obstacle_pos[0] - agent_pos[0]);
		correction_y2_agent =  coef1_agent * coef2 * (obstacle_pos[1] - agent_pos[1]);
		//}

		cur->setP(agent_pos[0] + correction_x2_agent, agent_pos[1] + correction_y2_agent);
}

void Env::Update()
{
	srand((unsigned int)time(0));

	double score = 0.0;
	double prev_dist = getTargetDist();

	Agent* agent_;
	for(int i=0; i<agent_num; i++){
		agent_ = _agents.at(i);
		agent_->Action();
	}

	double cur_dist = getTargetDist();
	double diff_dist = prev_dist - cur_dist;

	double target_score = getTargetScore(diff_dist);
	//double target_score = getTargetScore(prev_dist, cur_dist, diff_dist);
	
	double prefV_score = getPrefScore(_agents.at(0));
	double dir_score = getDirScore(_agents.at(0));
	double smooth_score = getSmoothScore(_agents.at(0));
	double comfort_distance_score = getComfortDistanceScore(_agents.at(0));

	for(int i=0; i<agent_num; i++){
		agent_ = _agents.at(i);
		agent_->setVisionVel();
		agent_->visionReset();
		// if(Dist(agent_->getP(), agent_->getD()) < 4.0)
		// 	agent_->setD( -24 + rand()%48, -16 + rand()%32);
	}

	#pragma omp parallel for
	for(int i=0; i<agent_num; i++)
	{
		Agent* cur_agent;
		cur_agent = _agents.at(i);
		cur_agent->setCol(false);

		bool isCol = false;
		#pragma omp parallel for
		for(int j=i+1; j<agent_num; j++)
		{
			Agent* other_agent;
			other_agent = _agents.at(j);
			double other_data[7];
			other_agent->getData(other_data);

			double *curP1 = cur_agent->getP();
			double *otherP1 = other_agent->getP();
			double *curR = cur_agent->getR();
			double *otherR = other_agent->getR();
		
			double distance_btwn_agents = Dist(curP1, otherP1);
				
			//if(cur_agent->colCheck(other_data)){    // BASELINE'S agent-agent COLLISION CHECK CONDITION
			if((distance_btwn_agents <= collision_dist_multiplier_agent * (curR[0] + otherR[0])) && (distance_btwn_agents != 0)){
				isCol = true;

				//cur_agent->Revert(other_agent->getP(), true);   // TO CALL BASELINES'S Revert() 
				//other_agent->Revert(cur_agent->getP(), true);
				
				double *curP = cur_agent->getP();
				double *curR = cur_agent->getR();
				double *otherP = other_agent->getP();
				double *otherR = other_agent->getR();

				//cur_agent->RevertAgent(curP, otherP, curR,  otherR, true, true);     
				//other_agent->RevertAgent(curP, otherP, curR,  otherR, true, false); 

				RevertAgent(cur_agent, other_agent);    
			}
		}

		#pragma omp parallel for
		for(int j=0; j<obstacle_num; j++)
		{
			Obstacle* cur_obs;
			cur_obs = _obstacles.at(j);
			double obs_data[7];
			cur_obs->getData(obs_data);

			double *agentP1 = cur_agent->getP();
			double *obstacleP1 = cur_obs->getP();
			double *agentR = cur_agent->getR();
			double *obstacleR = cur_obs->getR();

			double distance_btwn_agentAndObstacle = Dist(agentP1, obstacleP1);

			//if(cur_agent->colCheck(obs_data)){   // was BASELINE'S agent-obstacle COLLISION CHECK CONDITION
			if((distance_btwn_agentAndObstacle <= collision_dist_multiplier_obstacle * (agentR[0] + obstacleR[0])) && (distance_btwn_agentAndObstacle != 0)){
				isCol = true;

				//cur_agent->Revert(cur_obs->getP(), true);  //TO CALL BASELINES'S Revert() 
				//cur_agent->RevertObstacle(cur_obs->getP(),cur_agent->getP(), cur_obs->getR(), cur_agent->getR());    // TO CALL Agent.cpp file's RevertObstacle()

				RevertObstacle(cur_agent, cur_obs);   //TO CALL Envt.cpp file's RevertObstacle() 
			}
		}

		if(isCol){
			cur_agent->setCol(true);
		}
	}
/*
	for(int i=0; i<agent_num; i++)
	{
		Agent* cur_agent = _agents.at(i);
		double *curP1 = cur_agent->getP();
		double *oldP1 = cur_agent->getPprev();
		double cur_v_x = (curP1[0] - oldP1[0]) / cur_agent->getTimeStep();
		double cur_v_y = (curP1[1] - oldP1[1]) / cur_agent->getTimeStep();
		//cur_agent->setV(cur_v_x,cur_v_y);	
		cur_agent->setVsim(cur_v_x,cur_v_y);	
 	}
*/
	double col_score = 0.0;
	if(_agents.at(0)->getCol())
		col_score = w_col;

	score += target_score;
	score += prefV_score;
	score += smooth_score;
	score += col_score;
	score += dir_score;
	score += comfort_distance_score;

	_reward = score;
	
	_reward_sep[0] = target_score;
	_reward_sep[1] = prefV_score;
	_reward_sep[2] = smooth_score;
	_reward_sep[3] = col_score;
	_reward_sep[4] = dir_score;
	_reward_sep[5] = comfort_distance_score;
}

void Env::setAction(int i, double w, double a_x, double a_y)
{	
	_agents.at(i)->setAction(w, a_x, a_y);
	if(i==0)
		_cur_step += 1;
}

bool Env::isTerm(bool isTest)
{
	for(i=0; i<agent_num; i++)
	{
	 if(!isTest && _agents.at(i)->getCol())
		return true;
	}
	countAgent =0;
	
	for(i=0; i<agent_num; i++)
	{
		if(Dist(_agents.at(i)->getD(), _agents.at(i)->getP()) < 0.4)
		{
			_agents.at(i)->setAnimationStatus(1);
			countAgent++;
		}
	
	}
	if(countAgent == agent_num)
	{
		return true;
	}
	if(_cur_step > _max_step)
		return true;

	return false;
}

bool Env::isCol()
{
	if(_agents.at(0)->getCol())
		return true;
}

double Env::getTargetDist()
{
	Agent* agent_ = _agents.at(0);
	double dist = Dist(agent_->getP(), agent_->getD());
}

double Env::getTargetScore(double dist)
{
	double score = 0.0;
	if(dist > 0.0){
		score = w_target * dist;
	}
	else{
		score = w_target * dist;
		//score = (-2.0) * dist;
	}

	return score;
}

/*
double Env::getTargetScore(double PrevDist, double curDist, double dist)
{
	double score = 0.0;
	if((dist > 0.0) && (curDist<PrevDist)){
		score = w_target * dist;
		//cout<<"PrevDist: " << PrevDist<<", curDist: " << curDist << "\n";
		//cout<<"good\n\n\n";
	}
	else if((dist > 0.0) && (curDist>PrevDist)){
		score = (-0.25) * (dist);
		//score = (-2.0) * dist;
		//cout<<"PrevDist: " << PrevDist<<", curDist: " << curDist << "\n";
		//cout<<"current distance bigger than previous!!\n\n\n";
	}

	return score;
}
*/

double Env::getSmoothScore(Agent* agent_)
{
	double* cur_v = agent_->getV();
	double* prev_v = agent_->getVprev();
	double cur_w = agent_->getW();
	double prev_w = agent_->getWprev();
	double* _Current_agent_accelaration = agent_->getAccelaration();
	double* _prev_agent_accelaration = agent_->getPrevAccelaration();

	double cur_v_ = sqrt(cur_v[0]*cur_v[0] + cur_v[1]*cur_v[1]);

	double smooth_v = 0.0;
	double smooth_w = 0.0;
	double smooth_acceleration = 0.0;

	double v_dist = Dist(cur_v, prev_v);
	smooth_v = w_smooth_v*v_dist;

	//double w_dist = fabs(cur_w - prev_w);
	//smooth_w = w_smooth_w*w_dist;

	double w_dist = (1+(fabs(cur_w - prev_w)/(2*PI))) * (1+(fabs(cur_w - prev_w)/(2*PI))) * 10;
	smooth_w = w_smooth_w*w_dist;

	//return smooth_v + smooth_w;
	
	double w_acc_dist = 0.0;
	w_acc_dist = Dist(_Current_agent_accelaration, _prev_agent_accelaration);
	//w_acc_dist = fabs(_Current_agent_accelaration - _prev_agent_accelaration);
	smooth_acceleration = w_smooth_acceleration * w_acc_dist;  
		
	return smooth_v + smooth_w + smooth_acceleration;
}

double Env::getPrefScore(Agent* agent_)
{
	double* cur_v = agent_->getV();
	double cur_w = agent_->getW();

	double pref_v = 0.0;
	double pref_w = 0.0;
	double origin[2] = {0.0, 0.0};
	double tmp_v_dist = Dist(origin, cur_v);
	if(tmp_v_dist > 1.0){
		pref_v = w_pref_v*(tmp_v_dist - 1.0);
	}

	double origin_ = 0.0;
	double tmp_w_dist = fabs(origin_ - cur_w);
	if(tmp_w_dist > 1.5){
		pref_w = w_pref_w*(tmp_w_dist - 1.5);
	}
	return pref_v + pref_w;
}

double Env::getDirScore(Agent* agent_)
{
	double* cur_v = agent_->getV();

	double cur_v_len = sqrt(cur_v[0]*cur_v[0]+cur_v[1]*cur_v[1]);
	double cur_v_norm[2] = {cur_v[0]/cur_v_len, cur_v[1]/cur_v_len};
	double dir[2] = {0.0, 1.0};
	// pref_dir = w_pref_dir*(-1.0 * tanh(10.0*acos(Dot(cur_v_norm, dir))));

	// return pref_dir;
	double pref_dir = 0.0;

	double* cur_d = agent_->getD();
	double* cur_p = agent_->getP();
	double vec[2];

	vec_sub_vec(cur_d, cur_p, vec);
	double vec_len = sqrt(vec[0]*vec[0]+vec[1]*vec[1]);
	double vec_norm[2] = {vec[0]/vec_len, vec[1]/vec_len};

	double* cur_q = agent_->getQy();

	pref_dir = w_pref_dir*(-1.0 * acos(Dot(cur_q, vec_norm)));
	pref_dir += w_pref_dir*(-1.0 * acos(Dot(cur_v_norm, dir))); 
	return pref_dir;
}


double Env::getComfortDistanceScore(Agent* agent_)
{
	double comfort_distance_score1 = 0.0, comfort_distance_score2 = 0.0;
	for(int i=0; i<agent_num; i++)
	{
		Agent* cur_agent;
		cur_agent = _agents.at(i);
		double *curR = cur_agent->getR();

		for(int j=i+1; j<agent_num; j++)
		{
			Agent* other_agent;
			other_agent = _agents.at(j);

			double *otherR = other_agent->getR();				
			double dist = Dist(cur_agent->getP(), other_agent->getP());
			
			// if(dist <= 2*(curR[0] + otherR[0]))
			if(dist <= 5*(curR[0] + otherR[0]))
			{
				//comfort_distance_score1 = -w_comfort_dist;
				comfort_distance_score1 = comfort_distance_score1 - w_comfort_dist;
			}
			/*
			commenting on 05-04-2022
			else
			{
				//comfort_distance_score1 = w_comfort_dist;
				comfort_distance_score1 = comfort_distance_score1 + w_comfort_dist;
			}	
			*/
		}
	}
	return comfort_distance_score1;
}


double Env::getReward()
{
	return _reward;
}

double* Env::getRewardSep()
{
	return _reward_sep;
}

void Env::addAgent(Agent* agent)
{
	_agents.push_back(agent);
}

void Env::addObstacle(Obstacle* obstacle)
{
	_obstacles.push_back(obstacle);
}

void Env::addWall(Wall* wall)
{
	_walls.push_back(wall);
}

const vector<Agent*>& Env::getAgents()
{
	return _agents;
}

const vector<Obstacle*> & Env::getObstacles()
{
	return _obstacles;
}

const vector<Wall*> & Env::getWalls()
{
	return _walls;
}

Agent* Env::getAgent(int id)
{
	return _agents[id];
}

Obstacle* Env::getObstacle(int id)
{
	return _obstacles[id];
}

Wall* Env::getWall(int id)
{
	return _walls[id];
}

int Env::getNumAgents() const
{
	return _agents.size();
}

int Env::getNumObstacles() const
{
	return _obstacles.size();
}

int Env::getNumWalls() const
{
	return _walls.size();
}
