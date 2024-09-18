#include <iostream>
#include <ctime>
#include <omp.h>
#include "../mMath.h"
#include "Hallway.h"

using namespace std;

double intial_y_val_l_left = -14;
double intial_y_val_h_left = 14;
double intial_y_val_l_right = -14;
double intial_y_val_h_right = 14;
double temp_intial_y_val_l_right=0, temp_intial_y_val_l_left=0, temp_intial_y_val_h_left=0, temp_intial_y_val_h_right=0;

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
			
			obs_pos[0] = 4.0;
			obs_pos[1] = 1.0;
			
			//obs_pos[1] = 24;
			//cout << "print 1 is working"<<"\n";

			double obs_r[2];
			//obs_r[0] = (10 + rand()%10)/20.0;
			//obs_r[1] = (10 + rand()%10)/20.0;
			
			obs_r[0] = 2.0 + (rand()%3)*0.1;
			obs_r[1] = 2.0 + (rand()%3)*0.1;
			//cout << "Hello 1,"<< "		"<<"obs_r[0]= " << obs_r[0]<<",	"<<"obs_r[1]= " << obs_r[1]<<"\n";

			Obstacle* obs = new Obstacle(obs_r, obs_pos); // p q d
			addObstacle(obs);
		}
	}
	else{
		double obs_h = -3.0 + rand()%6;
		for(int i=0; i<obstacle_num; i++)
		{
			double obs_pos[2];
			double obs_r[2];

			if(i==0){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = 3.6 + obs_h;
				
				obs_pos[0] = 4.0;
				obs_pos[1] = 1.0;
				
				//cout << "print 2 is working"<<"\n";

				//obs_r[0] = 2.8 + (rand()%3)*0.1;
				//obs_r[0] = 2.8 + (rand()%3)*0.1;
				
				obs_r[0] = 2.0 + (rand()%3)*0.1;
				obs_r[1] = 2.0 + (rand()%3)*0.1;
				//cout << "Hello 2,"<< "		"<<"obs_r[0]= " << obs_r[0]<<",	"<<"obs_r[1]= " << obs_r[1]<<"\n";
			}
			else if(i==1){
				//obs_pos[0] = obs_x;
				//obs_pos[1] = -3.6 + obs_h;
				
				obs_pos[0] = 5.0;
				obs_pos[1] = 5.0;
				//cout << "print 3 is working"<<"\n";

				//obs_r[0] = 2.0 + (rand()%3)*0.1;
				//obs_r[1] = 2.0 + (rand()%3)*0.1;
				
				obs_r[0] = 2.8 + (rand()%3)*0.1;
				obs_r[1] = 2.8 + (rand()%3)*0.1;
				//cout << "Hello 3,"<< "		"<<"obs_r[0]= " << obs_r[0]<<",	"<<"obs_r[1]= " << obs_r[1]<<"\n";
			}
			else{
				while(true){
					obs_pos[0] = -10.0 + rand()%18;
					//cout << "print 4 is working"<<"\n";
					if(obs_pos[0] < obs_x - 5.0 || obs_pos[0] > obs_x + 5.0)
						break;
				}

				obs_pos[1] = -4.0 + rand()%8;
				//cout << "print 5 is working"<<"\n";

				//obs_r[0] = (10 + rand()%10)/20.0;
				//obs_r[1] = (10 + rand()%10)/20.0;
				
				obs_r[0] = 2.0 + (rand()%3)*0.1;
				obs_r[1] = 2.0 + (rand()%3)*0.1;
				//cout << "Hello 4,"<< "		"<<"obs_r[0]= " << obs_r[0]<<",	"<<"obs_r[1]= " << obs_r[1]<<"\n";
			}

			double tmp;
			if(obs_r[0] < obs_r[1]){
				tmp = obs_r[0];
				obs_r[0] = obs_r[1];
				obs_r[1] = tmp;
				//cout << "Hello 5,"<< "		"<<"obs_r[0]= " << obs_r[0]<<",	"<<"obs_r[1]= " << obs_r[1]<<"\n";
				
			}
			//cout << "from Hallway, obs_pos[0]= "<< "  " << obs_pos[0]<<"\n";

			Obstacle* obs = new Obstacle(obs_r, obs_pos); // p q d
			addObstacle(obs);
		}
	}

	for(int i=0; i<agent_num; i++)
	{
		double agent_r[2];
		//agent_r[0] = (3 + rand()%5)/10.0;
		//agent_r[1] = (3 + rand()%2)/10.0;
		
		agent_r[0] = 0.4;
		agent_r[1] = 0.4;
		
		//cout << "agent_r[0] = " << agent_r[0]<< "  " << "agent_r[1] = " << agent_r[1]<<"\n";
		
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
				
			if(i==0){
				agent_pos[0] = 10.18;
				agent_pos[1] = 5;
				//std::cout << "Hello hello 1" <<"\n";
			}
			else if(i==1){
				agent_pos[0] = 5;
				agent_pos[1] = 10.18;
				//std::cout << "Hello hello 2" <<"\n";
			}
			else if(i==2){
				agent_pos[0] = -2.0;
				agent_pos[1] = 12.0;
				//std::cout << "Hello hello 3" <<"\n";
			}
			else if(i==3){

				agent_pos[0] = -9;
				agent_pos[1] = 10.18;
				//std::cout << "Hello hello 4" <<"\n";
			}
			else if(i==4){
				agent_pos[0] = -14.18;
				agent_pos[1] = 5.0;
				//std::cout << "Hello hello 5" <<"\n";
			}
			else if(i==5){
				agent_pos[0] = -16.0;
				agent_pos[1] = -2.0;
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==6){
				agent_pos[0] = -14.18;
				agent_pos[1] = -9.0;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==7){
				agent_pos[0] = -9.0;
				agent_pos[1] = -14.18;
				//std::cout << "Hello hello 8" <<"\n";
			}
			else if(i==8){
				agent_pos[0] = -2;
				agent_pos[1] = -16.0;
				//std::cout << "Hello hello 9" <<"\n";
			}
			else if(i==9){
				agent_pos[0] = 5;
				agent_pos[1] = -14.18;
				//std::cout << "Hello hello 10" <<"\n";
			}
			else if(i==10){
				agent_pos[0] = 10.18;
				agent_pos[1] = -9;
				//std::cout << "Hello hello 11" <<"\n";
			}
			else if(i==11){
				agent_pos[0] = 12;
				agent_pos[1] = -2.0;
				//std::cout << "Hello hello 12" <<"\n";
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
			/*
			if(i==0){
				d_pos[0] = 8.875;
				d_pos[1] = 4.25;
			
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==1){
				d_pos[0] = 4.25;
				d_pos[1] = 8.875;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==2){
				d_pos[0] = -2;
				d_pos[1] = 10.5;
				//std::cout << "Hello hello 8" <<"\n";
			}
			else if(i==3){
				d_pos[0] = -8.25;
				d_pos[1] = 8.875;
				//std::cout << "Hello hello 9" <<"\n";
			}
			else if(i==4){
				d_pos[0] = -12.875;
				d_pos[1] = 4.25;
				//std::cout << "Hello hello 10" <<"\n";
			}
			else if(i==5){
				d_pos[0] = -14.5;
				d_pos[1] = -2.0;
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==6){
				d_pos[0] = -12.875;
				d_pos[1] = -8.25;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==7){
				d_pos[0] = -8.25;
				d_pos[1] = -12.875;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==8){
				d_pos[0] = -2;
				d_pos[1] = -14.5;
				//std::cout << "Hello hello 10" <<"\n";
			}
			else if(i==9){
				d_pos[0] = 4.25;
				d_pos[1] = -12.875;
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==10){
				d_pos[0] = 8.875;
				d_pos[1] = -8.25;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==11){
				d_pos[0] = 10.5;
				d_pos[1] = -2.0;
				//std::cout << "Hello hello 7" <<"\n";
			}

			//std::cout << "value of i: = " << i << " \n ";
		*/




	/*
			if(i==0){
				d_pos[0] = 10.0;
				d_pos[1] = 0.0;
			
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==1){
				d_pos[0] = -9.2;
				d_pos[1] = 0.0;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==2){
				d_pos[0] = 10;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 8" <<"\n";
			}
			else if(i==3){
				d_pos[0] = -9.2;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 9" <<"\n";
			}
			else if(i==4){
				d_pos[0] = 7.6;
				d_pos[1] = 0.0;
				//std::cout << "Hello hello 10" <<"\n";
			}
			else if(i==5){
				d_pos[0] = -11.6;
				d_pos[1] = 0.0;
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==6){
				d_pos[0] = 7.6;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==7){
				d_pos[0] = -11.6;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==8){
				d_pos[0] = 5.2;
				d_pos[1] = 0.0;
				//std::cout << "Hello hello 10" <<"\n";
			}
			else if(i==9){
				d_pos[0] = -14;
				d_pos[1] = 0.0;
				//std::cout << "Hello hello 6" <<"\n";
			}
			else if(i==10){
				d_pos[0] = 5.2;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 7" <<"\n";
			}
			else if(i==11){
				d_pos[0] = -14;
				d_pos[1] = -4.0;
				//std::cout << "Hello hello 7" <<"\n";
			}

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
			double cur_front = 3.14;
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
			double cur_front = (rand()%314)/100.- 3.14*0.5;
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




