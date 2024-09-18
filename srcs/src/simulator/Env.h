#ifndef ENV_H
#define ENV_H

#include <vector>
#include "Agent.h"
#include "Obstacle.h"
#include "Wall.h"

//#include <boost/python.hpp>
//#include <boost/python/dict.hpp>
//#include <boost/python/numpy.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


#include <string>

namespace py = pybind11;
using namespace std;

class Env{
	protected:
		vector<Agent* >  _agents;
		vector<Obstacle* >  _obstacles;
		vector<Wall* >  _walls;

		int agent_num;
		int obstacle_num;
		int wall_num;

		int _cur_step = 0;
		//int _max_step = 400;
		int _max_step = 1000;		//it was intitialized as 400 originally
		double _reward;
		double _reward_sep[6];
		//double _reward_sep[7];

		int eval_set_num = 6;
		vector<vector<double>> eval_agent_p;
		vector<vector<double>> eval_agent_r;
		vector<vector<double>> eval_agent_d;
		vector<vector<double>> eval_obs_p;
		vector<vector<double>> eval_obs_r;

	public:
		Env();
		~Env();

		pybind11::dict EnvTwo(double w_target, double w_col, double w_smooth_v, double w_smooth_w, double w_smooth_acceleration, double w_pref_v, double w_pref_w, double w_pref_dir, double w_bubble);
		Env(double w_target, double w_col, double w_smooth_v, double w_smooth_w, double w_smooth_acceleration, double w_pref_v, double w_pref_w, double w_pref_dir, double w_bubble);
		
		virtual void Reset(int idx) = 0;
		virtual void initWalls() = 0;
		virtual void initEvaluation() = 0;
		virtual void ResetEval(int idx) = 0;
		virtual void ResetEnv() = 0;

		const vector<Agent*> & Observe();
		void setAction(int i, double w, double a_x, double a_y);
		void Update();
		double getReward();
		double* getRewardSep();
		double getTargetDist();
		double getTargetScore(double dist);
		//double getTargetScore(double PrevDist, double curDist, double dist);
		double getSmoothScore(Agent* agent_);
		double getPrefScore(Agent* agent_);
		double getDirScore(Agent* agent_);
		double getBubbleScore(Agent* agent_);
		double getComfortDistanceScore(Agent* agent_);
		bool isTerm(bool isTest);
		bool isCol();

		// Object
		void addAgent(Agent* agent);
		void addObstacle(Obstacle* obstacle);
		void addWall(Wall* wall);

		const vector<Agent*> & getAgents();
		const vector<Obstacle*> & getObstacles();
		const vector<Wall*> & getWalls();

		Agent* getAgent(int id);
		Obstacle* getObstacle(int id);
		Wall* getWall(int id);

		int getNumAgents() const;
		int getNumObstacles() const;
		int getNumWalls() const;
		
		void getParameters();
};

#endif
