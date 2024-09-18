#include <omp.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "Agent.h"
#include "mMath.h"
#include <queue>
//#include <boost/circular_buffer.hpp>
#include <fstream>


#define PI 3.14159265

int loop_counter = 0;
double alpha = 0.95;
double alpha2 = 0.2;

using namespace std;

Agent::Agent()
{
	_r[0] = 0.4;
	_r[1] = 0.4;

	_v[0] = 0.0;
	_v[1] = 0.0;

	v_x_old = 0.0;
	v_y_old = 0.0;

	_v_sim_old[0] = 0;
	_v_sim_old[1] = 0;

	_v_sim[0] = 0.0;
	_v_sim[1] = 0.0;

	_w_sim_old = 0.0;
	
	_agent_accelaration[0] = 0.0;
	_agent_accelaration[1] = 0.0;
	
	_prev_agent_accelaration[0] = 0.0;
	_prev_agent_accelaration[1] = 0.0;

	_p_prev[0] = 0;
	_p_prev[1] = 0;

	_p[0] = 0.0;
	_p[1] = 0.0;

	_v_old[0] = 0.0; 
	_v_old[1] = 0.0;
	
	setShape();
	visionInit();
	setVisionOffset();
	
}

Agent::Agent(double* r)
{
	_r[0] = r[0];
	_r[1] = r[1];

	_v[0] = 0.0;
	_v[1] = 0.0;

	_v_old[0] = 0.0; 
	_v_old[1] = 0.0;

	_v_sim[0] = 0.0;
	_v_sim[1] = 0.0;

	v_x_old = 0.0;
	v_y_old = 0.0;

	_v_sim_old[0] = 0.0;
	_v_sim_old[1] = 0.0;

	_w_sim_old = 0.0;

	_agent_accelaration[0] = 0.0;
	_agent_accelaration[1] = 0.0;
	
	_prev_agent_accelaration[0] = 0.0;
	_prev_agent_accelaration[1] = 0.0;

	_p_prev[0] = 0;    // ADDED BY ME - START
	_p_prev[1] = 0;

	_p[0] = 0.0;
	_p[1] = 0.0;

	setShape();
	visionInit();
	setVisionOffset();
	
	for(int i=0; i<20; i++)   //for trajectory with last 50 positions
	{
		lastFiftyPosXv.push_back(-1000);
	 	lastFiftyPosYv.push_back(-1000);
	 
		_last50PosX[i] = -1000;
		_last50PosY[i] = -1000;
	}
}

Agent::Agent(double* r, double* p)
{
	_r[0] = r[0];
	_r[1] = r[1];

	_p[0] = p[0];
	_p[1] = p[1];

	_v[0] = 0.0;
	_v[1] = 0.0;

	_v_old[0] = 0.0; 
	_v_old[1] = 0.0;

	_v_sim[0] = 0.0;
	_v_sim[1] = 0.0;

	v_x_old = 0.0;
	v_y_old = 0.0;

	_v_sim_old[0] = 0.0;
	_v_sim_old[1] = 0.0;

	_w_sim_old = 0.0;

	_agent_accelaration[0] = 0.0;
	_agent_accelaration[1] = 0.0;
	
	_prev_agent_accelaration[0] = 0.0;
	_prev_agent_accelaration[1] = 0.0;

	_p_prev[0] = 0;     	// ADDED BY ME - START
	_p_prev[1] = 0;

	setShape();
	visionInit();
	setVisionOffset();
}

Agent::~Agent()
{
}

void Agent::getData(double* _data)
{
	_data[0] = _p[0];
	_data[1] = _p[1];
	_data[2] = _r[0];
	_data[3] = _r[1];
	_data[4] = _front;
	_data[5] = _v_sim[0];
	_data[6] = _v_sim[1];

	return;
}

void Agent::getRenderData(double* r_data)
{
	// r, p, d, front, color
	r_data[0] = _r[0];
	r_data[1] = _r[1];
	r_data[2] = _p[0];
	r_data[3] = _p[1];
	r_data[4] = _d[0];
	r_data[5] = _d[1];
	r_data[6] = _front;
	r_data[7] = _color[0];
	r_data[8] = _color[1];
	r_data[9] = _color[2];
}

void Agent::getBodyState(double* b_data)
{
	double width = 48.0;
	double height = 32.0;
	double len = sqrt(width*width + height*height)*1.0;

	double scale_p[2];
	scale_p[0] = _p[0] / len;
	scale_p[1] = _p[1] / len;

	double scale_d[2];
	scale_d[0] = _d[0] / len;
	scale_d[1] = _d[1] / len;

	double dist = Dist(scale_p, scale_d);

	double pd[2];
	pd[0] = (scale_d[0] - scale_p[0]) / dist;
	pd[1] = (scale_d[1] - scale_p[1]) / dist;
	double Inner = InnerProduct2d(_q_y, pd);
	double Cross = CrossProduct2d(_q_y, pd);

	//goal
	b_data[0] = Inner;
	b_data[1] = Cross;
	b_data[2] = dist;
	//currunt move
	b_data[3] = _v_sim[0];
	b_data[4] = _v_sim[1];
	b_data[5] = _w_sim;
	//shape
	b_data[6]   = 2.0*(_shape[0] - 0.3)/0.7; //scale to 0~2
	b_data[7]   = 2.0*(_shape[1] - 0.3)/0.7; //scale to 0~2
	b_data[8]   = 2.0*(_shape[2] - 0.3)/0.7; //scale to 0~2
	b_data[9]   = 2.0*(_shape[3] - 0.3)/0.7; //scale to 0~2
	b_data[10] = 2.0*(_shape[4] - 0.3)/0.7; //scale to 0~2
	b_data[11] = 2.0*(_shape[5] - 0.3)/0.7; //scale to 0~2
	b_data[12] = 2.0*(_shape[6] - 0.3)/0.7; //scale to 0~2
	b_data[13] = 2.0*(_shape[7] - 0.3)/0.7; //scale to 0~2
	b_data[14] = 2.0*(_shape[8] - 0.3)/0.7; //scale to 0~2
	b_data[15] = 2.0*(_shape[9] - 0.3)/0.7; //scale to 0~2
	b_data[16] = 2.0*(_shape[10] - 0.3)/0.7; //scale to 0~2
	b_data[17] = 2.0*(_shape[11] - 0.3)/0.7; //scale to 0~2
}

void Agent::visionInit()
{
	#pragma omp parallel for
	for(int i=0; i<_vision_ray_num; i++){
		_vision[i] = _vision_depth;
		_vision_prev[i] = _vision_depth;
		_vision_vel[i] = 0.0;
	}
}

void Agent::visionReset()
{
	for(int i=0; i<_vision_ray_num; i++){
		_vision_prev[i] = _vision[i];
		_vision[i] = _vision_depth;
	}
}

void Agent::setVisionOffset()
{
	double vision_st_ang;
	vision_st_ang = 90.0 - _vision_range/2.0;
	

	#pragma omp parallel for
	for(int i=0; i<_vision_ray_num; i++){
		double cur_ang = vision_st_ang + (i+0.5)*_vision_interval;
		double cur_rad = AngleToRadian(cur_ang);
		double cur_offset = 1.0 / sqrt( cos(cur_rad)*cos(cur_rad) / (_r[0]*_r[0])  + sin(cur_rad)*sin(cur_rad) / (_r[1]*_r[1]));
		_vision_offset[i] = cur_offset;
	}
}

void Agent::setShape()
{
	#pragma omp parallel for
	for(int i=0; i<12; i++){
		double cur_ang = 30.0*i;
		double cur_rad = AngleToRadian(cur_ang);
		double cur_offset = 1.0 / sqrt( cos(cur_rad)*cos(cur_rad) / (_r[0]*_r[0])  + sin(cur_rad)*sin(cur_rad) / (_r[1]*_r[1]));
		_shape[i] = cur_offset;
	}
}

double* Agent::getVisionOffset()
{
	return _vision_offset;
}

bool Agent::isVisible(double* d)
{
	double other_p[2];
	other_p[0] = d[0];
	other_p[1] = d[1];

	double dist = Dist(_p, other_p);
	double r_max = _r[0] > _r[1] ? _r[0] : _r[1];
	double other_r_max = d[2] > d[3] ? d[2] : d[3];

	if(dist <= _vision_depth + r_max + other_r_max)
		return true;

	return false;
}

void Agent::setVision(double* _data, bool isWall)
{
	if(!isWall){
		if(!isVisible(_data))
			return;
	}

	double st_pts[2] = {_p[0], _p[1]};

	#pragma omp parallel for
	for(int i=0; i<_vision_ray_num; i++){
		double cur_ang = 90.0 - _vision_range/2.0 + (i+0.5)*_vision_interval;
		double cur_rad = AngleToRadian(cur_ang)+(_front - PI/2.0);
		double angle_coord[2];
		RadianToCoor(cur_rad, angle_coord);

		double cur_rad_local = AngleToRadian(cur_ang);
		double angle_coord_local[2];
		RadianToCoor(cur_rad_local, angle_coord_local);

		double ed_pts[2];
		ed_pts[0] = st_pts[0] + angle_coord[0]*(_vision_depth+_vision_offset[i]);
		ed_pts[1] = st_pts[1] + angle_coord[1]*(_vision_depth+_vision_offset[i]);

		double new_depth = _vision_depth;
		if(isWall){
			double edge_st[2] = {_data[0], _data[1]};
			double edge_ed[2] = {_data[2], _data[3]};

			double result[3];
			LineIntersection(st_pts, ed_pts, edge_st, edge_ed, result);
			if(result[2] == 1){
				double pts[2] = {result[0], result[1]};
				double pts_len = Dist(st_pts, pts);
				new_depth = pts_len;
				if(new_depth < _vision[i]){
					_vision[i] = new_depth;
					if(_vision[i] < _vision_offset[i])
						_vision[i] = _vision_offset[i];
				}
			}
		}
		else{
			if(_data[2] == _data[3]){ // circle
				double other_p[2];
				other_p[0] = _data[0];
				other_p[1] = _data[1];
				double ray = RayToSphereDistance(st_pts, other_p, angle_coord, _data[2]);
				if(ray >= 0.0 && ray <= _vision_depth){
					new_depth = ray;
					if(new_depth < _vision[i]){
						_vision[i] = new_depth;
						if(_vision[i] < _vision_offset[i])
							_vision[i] = _vision_offset[i];
					}
				}
			}
			else{ // ellipse
				double cur_pts[2];
				double cur_len = _vision_depth + _vision_offset[i];
				if(LineEllipseIntersection(st_pts, ed_pts, _data, cur_pts)){
					new_depth = Dist(st_pts, cur_pts);
					if(new_depth < _vision[i]){
						_vision[i] = new_depth;
						if(_vision[i] < _vision_offset[i])
							_vision[i] = _vision_offset[i];
					}
				}
			}
		}
	}
}

void Agent::setVisionVel(){
	#pragma omp parallel for
	for(int i=0; i<_vision_ray_num; i++){
		_vision_vel[i] = _vision[i] - _vision_prev[i];
		if(_vision_vel[i] > 2.0 || _vision_vel[i] < -2.0){
			if(_vision[i] == _vision_depth || _vision_prev[i] == _vision_depth){
				_vision_vel[i] = 0.0;
			}
		}
	}
}

bool Agent::isCollidable(double* d)
{
	double other_p[2];
	other_p[0] = d[0];
	other_p[1] = d[1];

	double dist = Dist(_p, other_p);
	double r_max = _r[0] > _r[1] ? _r[0] : _r[1];
	double other_r_max = d[2] > d[3] ? d[2] : d[3];

	if(dist <= r_max + other_r_max + 0.1)
		return true;
	return false;
}


bool Agent::colCheck(double* _data)
{
	if(!isCollidable(_data))
		return false;

	double e1[5];
	e1[0] = _p[0];
	e1[1] = _p[1];
	e1[2] = _r[0];
	e1[3] = _r[1];
	e1[4] = _front-0.5*PI;

	double e2[5];
	e2[0] = _data[0];
	e2[1] = _data[1];
	e2[2] = _data[2];
	e2[3] = _data[3];
	e2[4] = _data[4]-0.5*PI;

	if(colEllipsetoEllipse(e1, e2))
		return true;
	else
		return false;
}

double* Agent::getVision()
{
	return _vision;
}

void Agent::setAction(double w, double v_x, double v_y)
{
	//cout<< "v_x: " << v_x << ", v_y: " << v_y << "\n";
	//_v_prev[0] = _v[0];
	//_v_prev[1] = _v[1];

/*
	if(loop_counter == 0)
	{
		v_x = v_x;
		v_y = v_y;
	}
	else
	{
		v_x = alpha * v_x_old + (1 - alpha) * v_x;
		v_y = alpha * v_y_old + (1 - alpha) * v_y;

		v_x_old = v_x;
		v_y_old = v_y;
	}
*/
	_v[0] = 2.0*v_x;
	_v[1] = 2.0*v_y;

	/*
	if(loop_counter == 0)
	{
		_v[0] = _v[0];
		_v[1] = _v[1];
	}
	else
	{
		_v[0] = alpha * _v_old[0] + (1 - alpha) * _v[0];
		_v[1] = alpha * _v_old[1] + (1 - alpha) * _v[1];

		_v_old[0] = _v[0];
		_v_old[1] = _v[1];
	}
	*/



	_v_sim[0] = mClip(-1.3, 1.3, _v[0]);    
	_v_sim[1] = mClip(-1.3, 1.3, _v[1]);

	//cout<< "Before alpha, _v_sim[0]: " << _v_sim[0] << ", _v_sim[1]: " << _v_sim[1] << "\n";
		
	if(loop_counter == 0)
	{
		_v_sim[0] = _v_sim[0];
		_v_sim[1] = _v_sim[1];
		//cout<< "First Iteration, _v_sim[0]: " << _v_sim[0]<< ",  _v_sim[1]: " << _v_sim[1] << "\n\n\n\n";
	}
	else
	{
		//cout<< "loop: _v_sim_old[0]: " << _v_sim_old[0]<< ",  _v_sim_old[1]: " << _v_sim_old[1] << "\n\n";
		_v_sim[0] = alpha * _v_sim_old[0] + (1 - alpha) * _v_sim[0];
		_v_sim[1] = alpha * _v_sim_old[1] + (1 - alpha) * _v_sim[1];
		//cout<< "after, _v_sim[0]: " << _v_sim[0]<< ",  _v_sim[1]: " << _v_sim[1] << "\n";
		
		//if((_v_sim_old[0]<0 && _v_sim[0]>0) || (_v_sim_old[0]>0 && _v_sim[0]<0))
		//	_v_sim[0] = _v_sim[0] * (-1);

		//if((_v_sim_old[1]<0 && _v_sim[1]>0) || (_v_sim_old[1]>0 && _v_sim[1]<0))
		//	_v_sim[1] = _v_sim[1] * (-1);
		

		//_v_sim_old[0] = _v_sim[0];
		//_v_sim_old[1] = _v_sim[1];
			//	cout<< "_v_sim_old[0]: " << _v_sim_old[0]<< ",  _v_sim_old[1]: " << _v_sim_old[1] << "\n\n";
	}
	_v_sim_old[0] = _v_sim[0];
	_v_sim_old[1] = _v_sim[1];
	//cout<< "_v_sim_old[0]: " << _v_sim_old[0]<< ",  _v_sim_old[1]: " << _v_sim_old[1] << "\n\n";


	_w = 4.0*w;
	_w_sim = mClip(-4.0, 4.0, _w);


	/*
	if(loop_counter == 0)
	{
		_w_sim = _w_sim;
	}
	else
	{
		_w_sim = alpha2 * _w_sim_old + (1 - alpha2) * _w_sim;

		_w_sim_old = _w_sim;
	}
	*/

	loop_counter++;
	//cout<< "from setAction() of Agent.cpp, _v_sim[0]: " << _v_sim[0] << ", _v_sim[1]: " << _v_sim[1] << "\n";
}

void Agent::Action()
{
	double dist_to_goal = Dist(_p, getD());
	double* gr = getR();

	if(_stop)
		return;

	_front += _w_sim * _time_step;
	//cout<< "from Action() of Agent.cpp, _front " << _front << "\n";

	if(_front > PI)
		_front -= PI*2;
	if(_front < -PI)
		_front += PI*2;


	RadianToCoor(_front-0.5*PI, _q_x);
	RadianToCoor(_front, _q_y);
	//_front = 0.0;

	//cout<< "from Action() of Agent.cpp, _q_x[0]: " << _q_x[0] << ",  _q_x[1]: " << _q_x[1] << "\n";
	//cout<< "from Action() of Agent.cpp, _q_y[0]: " << _q_y[0] << ",  _q_y[1]: " << _q_y[1] << "\n\n";

	_p_prev_third[0] = _p_prev[0];
	_p_prev_third[1] = _p_prev[1];
	
	_p_prev[0] = _p[0];
	_p_prev[1] = _p[1];

	_prev_agent_accelaration[0] = _agent_accelaration[0];
	_prev_agent_accelaration[1] = _agent_accelaration[1];

	_acc_prev_third[0] = _prev_agent_accelaration[0];
	_acc_prev_third[1] = _prev_agent_accelaration[1];

	_prev_agent_accelaration[0] = _agent_accelaration[0];
	_prev_agent_accelaration[1] = _agent_accelaration[1];

	if(dist_to_goal > gr[0])
	{
		_p[0] += _v_sim[0] * _q_x[0] * _time_step;
		_p[1] += _v_sim[0] * _q_x[1] * _time_step;

		_p[0] += _v_sim[1] * _q_y[0] * _time_step;
		_p[1] += _v_sim[1] * _q_y[1] * _time_step;
	}
	_agent_accelaration[0] = (_p_prev_third[0] - (2 * _p_prev[0]) + _p[0]) / (_time_step * _time_step); //added on 14th feb, 2022
	_agent_accelaration[1] = (_p_prev_third[1] - (2 * _p_prev[1]) + _p[1]) / (_time_step * _time_step); //added on 14th feb, 2022

	 lastFiftyPosXv.pop_front();
	 lastFiftyPosXv.push_back(_p[0]);
	 lastFiftyPosYv.pop_front();
	 lastFiftyPosYv.push_back(_p[1]);
	 
	for(int i=0; i<20; i++) //for trajectory with last 50 positions
	{
		_last50PosX[i] = lastFiftyPosXv[i];
		_last50PosY[i] = lastFiftyPosYv[i];
	}

	loop_counter++;

	_v[0] = _v_sim[0];
	_v[1] = _v_sim[1];

	_w = _w_sim;
	//cout<< "from Action() of Agent.cpp, _v[0]: " << _v[0] << ", _v[1]: " << _v[1] << "\n";


	double* gd = getD();

	if(dist_to_goal <= gr[0])
	{
		_v_sim[0] = 0.0;
		_v_sim[1] = 0.0;

		_p[0] = gd[0] - .25*gr[0];
		_p[1] = gd[1]- .25*gr[0];
	}


}
