#include <iostream>
#include <vector>
#include <ctime>
#include "parser.h"
#include "scenarios/Basic.h"
#include "scenarios/Passing.h"
#include "scenarios/Dot.h"
#include "scenarios/Hallway.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


using namespace std;
//namespace p = pybind11::literals;
//namespace np = pybind11::numpy;

//namespace p = boost::python;
//namespace np = boost::python::numpy;

namespace py = pybind11;


Parser::Parser(string Scenario, int a, int o)
{
//	np::initialize();
//	Py_Initialize();

	srand((unsigned int)time(0));
	agent_num = a;
	obstacle_num = o;

	if(Scenario.compare("Basic") == 0)
		_env = new Basic(agent_num, obstacle_num);
	else if(Scenario.compare("Passing") == 0)
		_env = new Passing(agent_num, obstacle_num);
	else if(Scenario.compare("Dot") == 0)
		_env = new Dot(agent_num, obstacle_num);
	else if(Scenario.compare("Hallway") == 0)
		_env = new Hallway(agent_num, obstacle_num);
}

Parser::~Parser()
{
	cout << "~Parser" << endl;
	delete _env;
}

void Parser::Reset(int idx,  int a, int o)
{
	_env -> Reset(idx);
}

pybind11::dict Parser::Step(py::array_t<double> action_, bool isTest)
{
//	float* a = reinterpret_cast<float*>(action_.get_data());
	auto action = action_.unchecked<2>();

	double w, v_x, v_y;
	for(int i=0; i<action.shape(0); i++)
	{
//		int idx_offset = action.shape(1) * i;

        w = action(i, 0);
		v_x = action(i, 1);
		v_y = action(i, 2);

//		w = a[idx_offset];
//		v_x = a[idx_offset+1];
//		v_y = a[idx_offset+2];
		//cout<< "from Step() of Parser.cpp, v_x: " << v_x << ", v_y: " << v_y << "\n";
		_env->setAction(i, w, v_x, v_y);
	}
	_env->Update();

	pybind11::dict memory;
	memory["obs"] = Observe();
	memory["isCol"] = _env->isCol();
	memory["isTerm"] = _env->isTerm(isTest);
	memory["reward"] = _env->getReward();
	double* reward_sep = _env->getRewardSep();
	pybind11::list reward_sep_list;
	for(int i=0; i<6; i++)
		reward_sep_list.append(reward_sep[i]);
	memory["reward_sep"] = reward_sep_list;

	return memory;
}

pybind11::dict Parser::Observe()
{	
	pybind11::dict obs;
	pybind11::list agent_state;
	pybind11::list obstacle_state;
	pybind11::list wall_state;

	vector<Agent*> agents = _env -> Observe();
	vector<Obstacle*> obstacles = _env-> getObstacles();
	vector<Wall*> walls = _env-> getWalls();

	int ray_num = agents.at(0)->getVisionRayNum();

	Agent* cur_agent;
	for(int i=0; i<agent_num; i++)
	{
		cur_agent = agents.at(i);
		pybind11::dict cur_agent_data;

		double r_data[10]; // r, p, d, front, color
		cur_agent->getRenderData(r_data);
		pybind11::list render_list;
		for(int j=0; j<10; j++)
			render_list.append(r_data[j]);

		double body_data[18]; // body
		cur_agent->getBodyState(body_data);
		pybind11::list body_list;
		for(int j=0; j<18; j++)
			body_list.append(body_data[j]);
			//cout<< "body_state from Observe() of Parser.cpp, v_sim[0]: " << body_data[3] << ",  v_sim[1]: " << body_data[4] << "\n";
			
		double* sensor_data; // sensor
		sensor_data = cur_agent->getVision();
		pybind11::list sensor_list;
		double vision_depth = cur_agent->getVisionDepth();
		for(int j=0; j<ray_num; j++)
			sensor_list.append(3.0*sensor_data[j]/vision_depth);

		double* velocity_data; // sensor
		velocity_data = cur_agent->getVisionVel();
		pybind11::list velocity_list;
		for(int j=0; j<ray_num; j++)
			velocity_list.append(velocity_data[j]);

		double* sensor_offset_data;
		sensor_offset_data = cur_agent->getVisionOffset();
		pybind11::list offset_list;
		for(int j=0; j<ray_num; j++)
			offset_list.append(sensor_offset_data[j]);

		cur_agent_data["render_data"] = render_list;
		cur_agent_data["body_state"] =  body_list;
		cur_agent_data["sensor_state"] =  sensor_list;
		cur_agent_data["velocity_state"] =  velocity_list;
		cur_agent_data["offset_data"] = offset_list;
		
		pybind11::list last50PosXlist;
		double* xList = cur_agent->getLast50PosX();

		for(int j=0;j<20;j++)	//for trajectory with last 50 positions
			last50PosXlist.append(xList[j]);
			
		cur_agent_data["last_50_posX"] = last50PosXlist;
		
		pybind11::list last50PosYlist;
		double* yList = cur_agent->getLast50PosY();
		for(int j=0;j<20;j++)   //for trajectory with last 50 positions
			last50PosYlist.append(yList[j]);
		
		cur_agent_data["last_50_posY"] = last50PosYlist;
		
		int animStatus = cur_agent->getAnimationStatus();
		cur_agent_data["anim_status"] = animStatus;

		agent_state.append(cur_agent_data);
	}

	Obstacle* cur_obstacle;
	for(int i=0; i<obstacle_num; i++)
	{
		cur_obstacle = obstacles.at(i);

		pybind11::dict cur_obstacle_state;

		double* p = cur_obstacle->getP();
		pybind11::list p_list;
		p_list.append(p[0]);
		p_list.append(p[1]);
		cur_obstacle_state["p"] = p_list;

		double* r = cur_obstacle->getR();
		pybind11::list r_list;
		r_list.append(r[0]);
		r_list.append(r[1]);
		cur_obstacle_state["r"] = r_list;

		double f = cur_obstacle->getFront();
		pybind11::list f_list;
		f_list.append(f);
		cur_obstacle_state["front"] = f_list;

		obstacle_state.append(cur_obstacle_state);
	}

	Wall* cur_wall;
	for(int i=0; i<walls.size(); i++)
	{
		cur_wall = walls.at(i);

		pybind11::dict cur_wall_state;

		double* p = cur_wall->getP();
		pybind11::list p_list;
		p_list.append(p[0]);
		p_list.append(p[1]);
		cur_wall_state["p"] = p_list;

		double w = cur_wall->getW();
		pybind11::list w_list;
		w_list.append(w);
		cur_wall_state["w"] = w_list;

		double h = cur_wall->getH();
		pybind11::list h_list;
		h_list.append(h);
		cur_wall_state["h"] = h_list;

		wall_state.append(cur_wall_state);
	}

	obs["agent"] = agent_state;
	obs["obstacle"] = obstacle_state;
	obs["wall"] = wall_state;
		
	return obs;
}


//PYBIND11_MODULE(csim, m) {
//    m.doc() = "pybind11 example plugin"; // optional module docstring
//    py::class_<Parser>(m, "Parser").def("Reset", &Parser::Reset).def("Step", &Parser::Step).def("Observe", &Parser::Observe);
//}


PYBIND11_PLUGIN(csim)
{

//	p::class_<Parser>("Parser", p::init<std::string, int, int>())


    py::module m("csim", "pybind11 plugin");
    py::class_<Parser>(m, "Parser").def(py::init<std::string, int, int>()).def("Reset", &Parser::Reset).def("Step", &Parser::Step).def("Observe", &Parser::Observe);



    return m.ptr();

}


