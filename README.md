# Crowd Simulation via Pybind11

This project aims to replace the Python.Boost with the Pybind11 and currently only tested with Anaconda. It has only been tested on the Linux version.


## Prerequisites

### 1. Anaconda related
Firstly, create a conda environment with python version 3.8. We use pythonc 3.8 for installing the Imitation package.
```bash
conda create -n whatevername python=3.8
```
Second, install the g++ compiler and its required component.
 and install openGL headers into current conda environment

#### Linux
```bash
conda install -c conda-forge gxx_linux-64
```
```bash
conda install -c conda-forge freeglut mesalib glew glfw 
conda install -c conda-forge openmpi
```

#### MacOS
```bash
conda install -c conda-forge clangxx_osx-64
```
```bash
conda install -c conda-forge freeglut mesalib glew glfw 
conda install -c conda-forge openmpi
```
#### Windows with GPU support [DO NOT MODIFY]
```bash
conda create -n whatevername python=3.8

conda activate whatevername

conda install -c conda-forge cudatoolkit=11.2 cudnn=8.1.0

conda install -c conda-forge pybind11

pip install pyopengl

pip install imitation

pip install stable-baselines3

pip install mpi4py
```

install pyopengl from https://www.lfd.uci.edu/~gohlke/pythonlibs/#pyopengl

pip install pyopengl does not work on windows.

### 2. With GPU support
In our setting, we use cudatoolkit 11.3.1
```bash
conda install -c anaconda cudatoolkit
```

[//]: # (#### Windows 10)

[//]: # (Make sure you install the Microsoft Visual Studio 2019. The integrated MSVC will be used if you are in Windows platform)

[//]: # (```bash)

[//]: # (conda install -c conda-forge freeglut glew glfw )

[//]: # (conda install -c conda-forge openmpi)

[//]: # (```)

[//]: # ()
[//]: # (### 2. Additional Headers)

[//]: # (Make sure copy the files in folder "additional" to the "GL" folder located in created conda environment )

[//]: # (```bash)

[//]: # (addtional/freeglut_std.h -> /xxxxx/anaconda3/envs/whatevername/include/GL)

[//]: # (addtional/glut.h -> /xxxxx/anaconda3/envs/whatevername/include/GL)

[//]: # (```)

[//]: # (If the platform is Windows, the 'GL' path probably locates at the different path:)

[//]: # (```bash)

[//]: # (addtional/gl.h -> /xxxxx/anaconda3/envs/whatevername/Library/include/GL)

[//]: # (addtional/glu.h -> /xxxxx/anaconda3/envs/whatevername/Library/include/GL)

[//]: # (```)

### 3. Pybind11
Finally, install the pybind11
```bash
conda install -c conda-forge pybind11
```

### 4. Other Miscellaneous
#### Linux or MacOS
```bash
conda install pip mpi4py openmpi-mpicc
```

[//]: # (#### Windows 10)

[//]: # (```bash)

[//]: # (conda install pip)

[//]: # (```)

[//]: # (You also need to install both MPI and MPI SDK from)

[//]: # (https://www.microsoft.com/en-us/download/details.aspx?id=100593)

[//]: # (and added the installation path to system environment:)

[//]: # (```bash)

[//]: # (xxxxxxx/Microsoft SDKs/MPI)

[//]: # (xxxxxxx/MPI/Bin)

[//]: # (```)



### 5. Pip related
Run the command to install library from requirement.txt
#### Linux or MacOS
```bash
pip install stable-baselines3[extra]

pip install imitation

pip install tensorflow==2.11.0

pip install pyopengl

pip install numpy matplotlib ipython
```

[//]: # (#### Windows 10)




[//]: # ()
[//]: # (Without:)

[//]: # (```bash)

[//]: # (conda activate whatevername)

[//]: # (pip install tensorflow==1.15.5)

[//]: # (pip install nvidia-pyindex)

[//]: # (pip install pyopengl)

[//]: # (pip install gym==0.15.7)

[//]: # (pip install numpy matplotlib ipython)

[//]: # (pip install mpi4py)

[//]: # (```)

[//]: # (pip install -r requirements.txt)
## Build and Run
Navigate to the root directory of this project and run following command to wrap c++ code with pybind11
```bash
python setup.py build_ext -i
```
There should be a file called "csim.xxxxxxx.so" or "csim.xxxxxx.pyd" generated and then copy genrated .so file to 
```bash
srcs/src
```
Then, navigate "srcs/src" and find the file named "run.py" to run it
```bash
python run.py
```

## Clean and Rebuild
Sometimes, you may need to rebuild pybind11 wrapper when c++ code is changed. In this case, run
```bash
python setup.py clean
python setup.py build_ext -i
```

[//]: # (## Potential Problems)

[//]: # ()
[//]: # (### Compiler Version on Linux)

[//]: # ()
[//]: # (Sometimes, there can have an error to run the code 'run.py' even if you build successfully. If it is the Linux platform, it might be caused by the wrong g++. To exam the g++ version, you can navigate to the path you install the environment:)

[//]: # ()
[//]: # (```bash)

[//]: # (/xxxxx/anaconda3/envs/whatevername/x86_64-conda_cos6-linux-gnu)

[//]: # (```)

[//]: # (The above version is preferred in Linux platform and should be automatically installed whe you executed the line )

[//]: # (```bash)

[//]: # (conda install -c conda-forge gxx_linux-64)

[//]: # (```)

[//]: # (The best way to fix the problem is to remove current created conda environment and re-do the all steps again.)

[//]: # ()
[//]: # (### fatal error C1083: Cannot open include file: 'pybind11/pybind11.h': No such file or directory)

[//]: # (In Windows, the compiler is used from the system scope with MSVC. It may cause the compiler cannot find the pybind headers in the Anaconda environment. )

[//]: # (To solve it, you need to find the line in setup.py:)

[//]: # (```python)

[//]: # (include_dirs=[)

[//]: # (    'pybind11/include',)

[//]: # (],)

[//]: # (```)

[//]: # (And add the pybind11 path in your Anaconda environment:)

[//]: # (```python)

[//]: # (include_dirs=[)

[//]: # (    'pybind11/include',)

[//]: # (    "xxxxxxxx/anaconda3/envs/whatevername/Library/include")

[//]: # (],)

[//]: # (```)

[//]: # ()
[//]: # (### Encounter syntax errors related to "GL/gl.h")

[//]: # (Change )

[//]: # (```cpp)

[//]: # (#include <GL/gl.h>)

[//]: # (#include <GL/glu.h>)

[//]: # (#include <GL/glut.h>)

[//]: # (```)

[//]: # (to)

[//]: # (```cpp)

[//]: # (#include <GL/glut.h>)

[//]: # (#include <GL/gl.h>)

[//]: # (#include <GL/glu.h>)

[//]: # (```)

[//]: # (Refer to https://social.msdn.microsoft.com/Forums/vstudio/en-US/c80ec47e-559c-4ee5-8393-3922be6c203a/visual-studio-2008-visual-c-problem-when-using-glh-gluh-gluth-headers-excessive?forum=vcgeneral)

[//]: # ()
[//]: # (### Encounter NaN when training starts)

[//]: # (Re-run the run.py)
