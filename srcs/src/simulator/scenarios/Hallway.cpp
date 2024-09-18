#include <iostream>
#include <ctime>
#include <omp.h>
#include "../mMath.h"
#include "Hallway.h"

using namespace std;

int count1=0, count2=0, count3=0, count4=0, c1=0, c2=0; 

Hallway::Hallway(int agent_n, int obs_n)
{
	//cout << "Hello from HAllway Constructor \n";
	agent_num = agent_n;
	obstacle_num = obs_n;

	initEvaluation();
	initWalls();

	Reset(-1);
}

void Hallway::initWalls()
{
	wall_num = 2;

	double p1[2] = {0.0, 11.0};
	double w1 = 48.0;
	double h1 = 10.0;
	addWall(new Wall(p1, w1, h1));

	double p2[2] = {0.0, -11.0};
	double w2 = 48.0;
	double h2= 10.0;
	addWall(new Wall(p2, w2, h2));

	wall_num = _walls.size();
}

Hallway::~Hallway()
{
	for(vector< Agent* >::iterator it = _agents.begin() ; it != _agents.end(); it++)
		delete (*it);
	_agents.clear();

	for(vector< Obstacle* >::iterator it = _obstacles.begin() ; it != _obstacles.end(); it++)
		delete (*it);
	_obstacles.clear();
}

void Hallway::initEvaluation()
{
	srand((unsigned int)time(0));
}

void Hallway::Reset(int idx)
{
	if(idx == -1)
		ResetEnv();
	else
		ResetEval(idx);
}

void Hallway::ResetEval(int idx)
{
	for(vector< Agent* >::iterator it = _agents.begin() ; it != _agents.end(); it++)
		delete (*it);
	_agents.clear();

	for(vector< Obstacle* >::iterator it = _obstacles.begin() ; it != _obstacles.end(); it++)
		delete (*it);
	_obstacles.clear();
}

void Hallway::ResetEnv()
{
	for(vector< Agent* >::iterator it = _agents.begin() ; it != _agents.end(); it++)
		delete (*it);
	_agents.clear();

	for(vector< Obstacle* >::iterator it = _obstacles.begin() ; it != _obstacles.end(); it++)
		delete (*it);
	_obstacles.clear();

	srand((unsigned int)time(0));
/*
	bool isRand = false;
	if(rand()%2 == 0){
		isRand = true;
	}

	double obs_x = -4 + rand()%16;
	if(isRand){

		for(int i=0; i<obstacle_num; i++)
		{
			double obs_pos[2];
			//obs_pos[0] = -12.0 + rand()%20;
			//obs_pos[1] = -4.0 + rand()%8;
			
			obs_pos[0] = -5.0;
			obs_pos[1] = -8.0;
			
			//obs_pos[1] = 24;
			//cout << "print 1 is working"<<"\n";

			double obs_r[2];
			//obs_r[0] = (10 + rand()%10)/20.0;
			//obs_r[1] = (10 + rand()%10)/20.0;
			
			obs_r[0] = 2.0;
			obs_r[1] = 2.0;

			Obstacle* obs = new Obstacle(obs_r, obs_pos); // p q d
			addObstacle(obs);
		}
	//}
	else{
*/
		double obs_h = -3.0 + rand()%6;
		for(int i=0; i<obstacle_num; i++)
		{
			double obs_pos[2];
			double obs_r[2];

			if(i==0){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = 3.6 + obs_h;
				
				obs_pos[0] = 6.0;
				obs_pos[1] = -6.0;
				
				//cout << "print 2 is working"<<"\n";

				//obs_r[0] = 2.8 + (rand()%3)*0.1;
				//obs_r[0] = 2.8 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0;
				obs_r[1] = 2.0;
			}
			else if(i==1){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = -7.0;
				obs_pos[1] = -10.0;
				//cout << "print 3 is working"<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0;
				obs_r[1] = 2.0;
			}
			else if(i==2){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = -6.0;
				obs_pos[1] = 2.0;
				//cout << "print 3 is working "<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 1.0;
				obs_r[1] = 1.0;
			}
			else if(i==3){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = -9.0;
				obs_pos[1] = -6.0;
				//cout << "print 3 is working"<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0;
				obs_r[1] = 2.0;
			}
			else if(i==4){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = -9.0;
				obs_pos[1] = -8.0;
				//cout << "print 3 is working"<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0;
				obs_r[1] = 2.0;
			}
			else if(i==5){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = -11.0;
				obs_pos[1] = -9.0;
				//cout << "print 3 is working"<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0;
				obs_r[1] = 2.0;
			}


			double tmp;
			if(obs_r[0] < obs_r[1]){
				tmp = obs_r[0];
				obs_r[0] = obs_r[1];
				obs_r[1] = tmp;
			}

			Obstacle* obs = new Obstacle(obs_r, obs_pos); // p q d
			addObstacle(obs);
		}
	//}

	for(int i=0; i<agent_num; i++)
	{
		double agent_r[2];
		//agent_r[0] = (3 + rand()%5)/10.0;
		//agent_r[1] = (3 + rand()%2)/10.0;
		
		//agent_r[0] = 0.25;
		//agent_r[1] = 0.25;

		agent_r[0] = 0.40;
		agent_r[1] = 0.40;
		
		//cout << "agent_r[0] = " << agent_r[0]<<"  " << "agent_r[1] = " << agent_r[1]<<"\n";
		
		// agent_r[0] = (3 + rand()%2)/10.0;
		// agent_r[1] = agent_r[0];
		// agent_r[0] = 1.0;
		// agent_r[1] = 0.3;

		double tmp;
		if(agent_r[0] < agent_r[1]){
			tmp = agent_r[0];
			agent_r[0] = agent_r[1];
			agent_r[1] = tmp;
		}
		Agent* agent = new Agent(agent_r);
		agent->setId(i);

		bool col = false;
		double agent_pos[2];
		while(true)
		{		

			double newDegree = (i * (360.0f/agent_num));
			//std::cout << "newDegree: " << newDegree << "\n\n" << std::endl;
            		double newAngle = (newDegree / 180.0f * 3.14f);
			//std::cout << "newAngle: " << newAngle << "\n\n" << std::endl;

		    	agent_pos[0] = cos(newAngle) * 14;
		    	agent_pos[1] = sin(newAngle) * 14;

            		//std::cout << "agent_pos[0]: " << agent_pos[0] << ",  " << "agent_pos[1]: " << agent_pos[1] << std::endl;
				
			c1++;
			if(c1%48==0)
			{
				break;
			}


			

			col = false;
			int start_idx = 0;
			for(int j=start_idx; j<i; j++)
			{
				double boundary = (getAgent(j)->getR())[0] + agent_r[0] + 0.5;
				if(Dist(agent_pos, getAgent(j)->getP()) < boundary)
				{
					col = true;
					break;
				}
			}

			if(col == false)
			{
				for(int j=0; j<obstacle_num; j++)
				{
					double boundary = getObstacle(j)->getR()[0] + agent_r[0] + 1.0;
					if(Dist(agent_pos, getObstacle(j)->getP()) < boundary)
					{
						col = true;
						break;
					}
				}
			}

			if(col == false)
				break;
		}

		agent->setP(agent_pos[0], agent_pos[1]);
		agent->setPprev(agent_pos[0], agent_pos[1]);

		bool d_col = false;
		double d_pos[2];
		while(true){
	
		float newDegree = float(i * (360.0f/agent_num));
		//std::cout << "newDegree: " << newDegree << "\n\n" << std::endl;
            	float newAngle = (newDegree / 180.0f * 3.14f);
		//std::cout << "newAngle: " << newAngle << "\n\n" << std::endl;
			
	    	d_pos[0] = (-1) * cos(newAngle) * 15;
	    	d_pos[1] = (-1) * sin(newAngle) * 15;
            

           	//std::cout << "d_pos[0]: " << d_pos[0] << ",  " << "d_pos[1]: " << d_pos[1] << std::endl;
	
			c2++;
			if(c2%48==0)
			{
				break;
			}




				
			/*
			std::cout << "value of i from destination creation: = " << i << " \n ";
			
			*/
			

			d_col = false;
			for(int j=0; j<obstacle_num; j++)
			{
				double boundary = getObstacle(j)->getR()[0] + 0.5;
				if(Dist(d_pos, getObstacle(j)->getP()) < boundary)
				{
					d_col = true;
					break;
				}
			}

			if(Dist(d_pos, agent->getP()) < 3.0)
				d_col = true;

			if(!d_col)
				break;

			//std::cout << "goal " << i << " : " << agent_pos[0] << "," << agent_pos[1] << std::endl;

		}

		// double cur_front = (rand()%314)/100.+3.14*0.5;
		// double y_coord[2];
		// double x_coord[2];
		// RadianToCoor(cur_front, y_coord);
		// RadianToCoor(cur_front-0.5*3.141592, x_coord);

		// agent->setFront(cur_front);
		// agent->setQy(y_coord[0], y_coord[1]);
		// agent->setQx(x_coord[0], x_coord[1]);
		// agent->setD( d_pos[0], d_pos[1]);
		// agent->setColor(0.9, 0.5, 0.1);

		if(i%2==0){
			// double cur_front = ((rand()%628)/100.0)-3.14;
			// double cur_front = 0.5*3.14 + ((rand()%314)/100.0);
			//double cur_front = 3.14;

			double cur_front = 3.14 + atan2(agent_pos[1] - d_pos[1], agent_pos[0] -d_pos[0]);
			//cout<<"cur_front1: " << cur_front << ", ang1: " << cur_front << "\n\n";

			double y_coord[2];
			double x_coord[2];
			RadianToCoor(cur_front, y_coord);
			RadianToCoor(cur_front-0.5*3.141592, x_coord);

			agent->setFront(cur_front);
			agent->setQy(y_coord[0], y_coord[1]);
			agent->setQx(x_coord[0], x_coord[1]);
			agent->setD( d_pos[0], d_pos[1]);
			agent->setColor(0.1, 0.9, 0.1);
		}
		else{
			//double cur_front = (rand()%314)/100.- 3.14*0.5;
			double cur_front = 3.14 + atan2(agent_pos[1] - d_pos[1], agent_pos[0] -d_pos[0]);
			//cout<<"cur_front2: " << cur_front << ", ang1: " << cur_front << "\n\n";
			double y_coord[2];
			double x_coord[2];
			RadianToCoor(cur_front, y_coord);
			RadianToCoor(cur_front-0.5*3.141592, x_coord);

			agent->setFront(cur_front);
			agent->setQy(y_coord[0], y_coord[1]);
			agent->setQx(x_coord[0], x_coord[1]);
			agent->setD( d_pos[0], d_pos[1]);
			agent->setColor(0.9, 0.1, 0.1);
		}
		if(i==0)
			agent->setColor(0.9, 0.5, 0.1);

		addAgent(agent);
	}
	_cur_step = 0;
}
