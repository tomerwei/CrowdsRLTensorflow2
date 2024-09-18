#ifndef PARSER_H
#define PARSER_H

//#include <boost/python.hpp>
//#include <boost/python/dict.hpp>
//#include <boost/python/numpy.hpp>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

#include "Env.h"

class Parser{
	protected:
		Env* _env;

		int agent_num;
		int obstacle_num;
		int wall_num;

	public:
		Parser(std::string scenario, int a, int o);
		~Parser();

		void Reset(int idx, int a, int o);
		pybind11::dict Step(py::array_t<double> action_, bool isTest);
		pybind11::dict Observe();
};


#endif
