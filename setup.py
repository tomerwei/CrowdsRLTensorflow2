import os, sys
from glob import glob
from distutils.core import setup, Extension
from distutils import sysconfig
import platform


# this code block is aim to remove the parameter that shows the warning message if compile with the gcc (rather than
# g++). If compile with g++, this code block is not necessary.
#######################################################################################
import distutils.sysconfig
cfg_vars = distutils.sysconfig.get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "")
########################################################################################
        

# These two lines force the python setuptools to use the specific compilers.
# "CC" is for gcc
# "CXX" is for g++
# In here, I just force setuptools only compile the code with g++

# os.environ["CC"] = "gcc"
# os.environ["CXX"] = "C:\\Users\\Apple\\anaconda3\\envs\\RL_Pybind11\\Library\\mingw-w64\\bin\\g++.exe"

# parameters for g++ or gcc. These params should make the equivalent behavior for gcc and g++ when compiles with
# c++ code. If running on Windows, the compiler is used from MSVC (cl.exe) and you will not need the 'cpp_args'
cpp_args = ['-std=c++11', '-xc++', '-lstdc++', '-shared-libgcc']

if platform.system() == "Windows":
    cpp_args = []

# x is a python list that contains the all c++ code that required to be compiled with pybind11
x = glob("srcs/src/simulator/*.cpp")
x.extend([
    "srcs/src/simulator/scenarios/Basic.cpp",
    "srcs/src/simulator/scenarios/Passing.cpp",
    "srcs/src/simulator/scenarios/Dot.cpp",
    "srcs/src/simulator/scenarios/Hallway.cpp",
])

# print(sorted(x))

# name 'csim' is defined in original 
ext_modules = [
    Extension(
        'csim',
        # ['funcs.cpp', 'wrap.cpp'],
        sorted(x),
        include_dirs=[
            'pybind11/include',
            # "xxxxxxx/anaconda3/envs/secondtest/Library/include"
        ],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='csim',
    version='0.0.1',
    author='random author',
    author_email='xxxx@njit.edu',
    description='Example',
    ext_modules=ext_modules,
)
