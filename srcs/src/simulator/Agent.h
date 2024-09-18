#ifndef AGENT_H
#define AGENT_H

#include "Wall.h"
#include <iostream>
//#include "Obstacle.h"
#include <queue>
//#include <boost/circular_buffer.hpp>
//using namespace std;

//#include "Env.h"

class Agent{
	protected:

		int _id;
		double _r[2];
		//double _shape[12];
		double _shape[20];
		double _p[2];
		double _p_prev[2];
		double _d[2];
		double _q_x[2];
		double _q_y[2];
		double _front = 0.0;
		double _color[3];

		double _vision[36];
		double _vision_prev[36];
		double _vision_vel[36];
		double _vision_offset[36];
		double _vision_depth = 7.0;
		double _vision_interval = 10.0;
		double _vision_range = 360.0;
		int _vision_ray_num = 36; // _vision_range / _vision_interval + 1

		double _v[2];
		double _v_prev[2];
		double _v_sim[2];

		double v_x_old = 0.0;
	    double v_y_old = 0.0;

		double _v_sim_old[2];

		double _v_by_calculation[2];
		double _v_prev_by_calculation[2];
		double _v_prev_diff[2];
		double _acc_by_calculation[2];

		double _w = 0.0;
		double _w_prev = 0.0;
		double _w_sim = 0.0;
		double _w_sim_old = 0.0;

		//double _time_step = 0.1;
		double _time_step = 0.09;

		bool _col = false;
		bool _stop = false;
		
		double _p_prev_third[2];
		double _agent_accelaration[2];
		double _agent_accelaration2[2];
		double _prev_agent_accelaration[2];

		double _acc_prev_third[2];

		double _v_old[2];
		
		////for trajectory with last 50 positions
//		boost::circular_buffer<double> lastFiftyPosXv{400};
//		boost::circular_buffer<double> lastFiftyPosYv{400};

		std::deque<double> lastFiftyPosXv{20};
		std::deque<double> lastFiftyPosYv{20};
		
		double _last50PosX[20];
		double _last50PosY[20];

	public:
		Agent();
		Agent(double* r);
		Agent(double* r, double* p);
		~Agent();
		void Print();
		void setAction(double w, double v_x, double v_y);
		void Action();

		void Revert(double* p, bool col);    //commented this line and added the next line to add pbd 
		void Revert(double* p1, double* p2, double* r1, double* r2, bool col);
		void RevertObstacle(double* p1, double* p2, double* r1, double* r2);

		void RevertWall(double* p, bool col);

		void RevertAgent(double* p1, double* p2, double* r1, double* r2, bool col, bool isCurrentAgent);

		void setVisionWall(Wall* w);
		// bool isVisibleWall(Wall* w);

		bool isVisible(double* d);
		bool isCollidable(double* d);
		bool colCheck(double* _data);
		//void colCheck(double* p1, double* p2);
		bool colCheckWall(Wall* w);

		void visionInit();
		void visionReset();
		void setVision(double* d, bool isWall);
		void setVisionVel();
		void setVisionOffset();
		void setShape();
		void setVisionDepth(double _vd) {_vision_depth = _vd;};
		void setVisionRayNum(int _r) {_vision_ray_num = _r;};

		double* getVision();
		double* getVisionOffset();
		double getVisionDepth() {return _vision_depth;};
		int getVisionRayNum() {return _vision_ray_num;};
		double* getVisionVel() {return _vision_vel;};

		void getData(double* _data);
		void getRenderData(double* r_data);
		void getBodyState(double* b_data);
		void getSensorState(double* s_data);

		void setP(double x, double y) {_p[0] = x; _p[1] = y;};
		void setPprev(double x, double y) {_p_prev[0] = x; _p_prev[1] = y;};
		void setQx(double x, double y) {_q_x[0] = x; _q_x[1] = y;};
		void setQy(double x, double y) {_q_y[0] = x; _q_y[1] = y;};
		void setD(double x, double y) {_d[0] = x; _d[1] = y;};
		void setR(double _rx, double _ry) {_r[0] = _rx; _r[1] = _ry;};
		void setV(double v_x, double v_y) {_v[0] = v_x; _v[1] = v_y;};
		void setVprev(double v_x, double v_y) {_v_prev[0] = v_x; _v_prev[1] = v_y;};
		void setVsim(double v_x, double v_y) {_v_sim[0] = v_x; _v_sim[1] = v_y;};
		void setW(double w) {_w = w;};
		void setWprev(double w){_w_prev = w;};
		void setWsim(double w) {_w_sim = w;};
		void setFront(double f) {_front = f;};
		void setColor(double r, double g, double b) {_color[0] = r; _color[1] = g; _color[2] = b;};
		void setCol(bool c) {_col = c;};
		void setId (int id) {_id = id;}
		void setTimeStep(double t) {_time_step=t;};

		double* getP() {return _p;};
		double* getPprev() {return _p_prev;};
		double* getQx() {return _q_x;};
		double* getQy() {return _q_y;};
		double* getD() {return _d;};
		double* getR() {return _r;};
		double* getV() {return _v;};
		double* getVprev() {return _v_prev;};
		double* getVsim() {return _v_sim;};
		double getW() {return _w;};
		double getWprev() {return _w_prev;};
		double getWsim() {return _w_sim;};
		double getFront() {return _front;};
		double* getColor() {return _color;}
		bool getCol() {return _col;};
		int getId () const {return _id;};
		double getTimeStep() {return _time_step;};
		
		int _animStatus = 0; //0 means animating
		void setAnimationStatus (int animStatus) {_animStatus = animStatus;}
		int getAnimationStatus () {return _animStatus;};
		
		int _animStatus2 = 0;
		void setAnimationStatus2 (int animStatus2) {_animStatus2 = animStatus2;}
		int getAnimationStatus2 () {return _animStatus2;};
		
		void setAccelaration(double x, double y) {_agent_accelaration[0] = x; _agent_accelaration[1] = y;};
		void setPrevAccelaration(double x, double y) {_prev_agent_accelaration[0] = x; _prev_agent_accelaration[1] = y;};
		
		double* getAccelaration() {return _agent_accelaration;};
		double* getPrevAccelaration() {return _prev_agent_accelaration;};
		
		double* getLast50PosX() {return _last50PosX;};
		double* getLast50PosY() {return _last50PosY;};
};


#endif
