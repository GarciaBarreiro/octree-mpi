
[![pipeline status Master](https://gitlab.citius.usc.es/lidar/rule-based-classifier-cpp/badges/master/pipeline.svg)](https://gitlab.citius.usc.es/lidar/rule-based-classifier-cpp/commits/master)
[![pipeline status Dev](https://gitlab.citius.usc.es/lidar/rule-based-classifier-cpp/badges/dev/pipeline.svg)](https://gitlab.citius.usc.es/lidar/rule-based-classifier-cpp/commits/dev)

# rule-based-classifier: C++ version.

## Background

LiDAR (Light and Ranging Detection) technology has now become the quintessential technique for collecting geospatial 
data from the earth's surface. This code implements a method for point cloud partitioning based on distributed memory
and MPI (Message Passing Interface) technology.

Original project: https://gitlab.citius.usc.es/lidar/rule-based-classifier.
		
## Install

#### Cloning the project
```bash
git clone https://github.com/GarciaBarreiro/octree-mpi.git
cd octree-mpi
```

#### Dependencies
- Eigen, Armadillo and OpenMPI
  - Ubuntu
      ```bash
      sudo apt install libeigen3-dev libarmadillo-dev openmpi-bin openmpi-common openssh-client openssh-server libopenmpi1.3 libopenmpi-dbg libopenmpi-dev
      ```
  - ArchLinux
      ```bash
      sudo pacman -S eigen openmpi
      git clone https://aur.archlinux.org/armadillo.git armadillo
      (cd armadillo && makepkg -si --noconfirm)
      ```
 
The following commands must be executed in the root folder of the project.

- LASTools:
    ```bash
    wget https://lastools.github.io/download/LAStools_221128.zip && unzip LAStools_221128.zip -d ./lib && rm LAStools_221128.zip
    ```
- LASlib:
    ```bash
    (cd lib/LAStools && cmake . && make)
    ```

## Usage

The recommended way to compile the project is through CMake. In the project directory, just execute
  ```bash
  (cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cd build && make)
  ```

This creates the executable build/rule-based-classifier-cpp.

/!\ WARNING: If cmake is executed in the project directory, the already existing Makefile will be overwritten.


#### Execution
    ${path_to_binary_executable} -i data/ptR_18C.las -r search_radius [-o output_dir]

## Authorship
Grupo de Arquitectura de Computadores (GAC)  
Centro Singular de Investigación en Tecnologías Inteligentes (CiTIUS)  
Universidad de Santiago de Compostela (USC)  

Maintainers: 
- Miguel Yermo García ([miguel.yermo@usc.es](mailto:miguel.yermo@usc.es))
- Silvia Rodríguez Alcaraz ([silvia.alcaraz@usc.es](mailto:silvia.alcaraz@usc.es))
