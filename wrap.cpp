#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "funcs.hpp"

namespace py = pybind11;

using namespace pybind11::literals;
//using namespace pybind11::numpy;


PYBIND11_MODULE(wrap, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

     m.def("add", &add, "A function which adds two numbers",
          "i"_a=1, "j"_a=2);
}

//PYBIND11_PLUGIN(wrap) {
//    py::module m("wrap", "pybind11 example plugin");
//    m.def("add", &add, "A function which adds two numbers",
//          "i"_a=1, "j"_a=2);
//
//    return m.ptr();
//}
