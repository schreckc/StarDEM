This is the code corresponding to the article "StarDEM: efficient discrete element method for star-shaped particles".

https://github.com/schreckc

*************************

Dependency:
Eigen3
qt5

**************

Compilation:
cd QGLViewer
qmake
make
sudo make install
cd ..
qmake dem2d.pro
make

Change the number of threads used by OpenMP:
export OMP_NUM_THREADS=1

**************

Script to reproduce the examples of the article:

Packing:
  * random shapes
    ./packing.sh
  * spheres
    ./packing.sh 1
  * crosses
    ./packing.sh 2
  * butterflies
    ./packing.sh 3
  * bats
    ./packing.sh 4
Bridge:
  * random shapes
    ./bridge.sh    
  * flowers
    ./bridge.sh 1
  * blobs
    ./bridge.sh 2
  * blobs + visualization of the force network
    ./bridge.sh 3
Drum:
   * random shapes
    ./drum.sh
   * flowers
    ./drum.sh 1
   * peanuts
    ./drum.sh 2
   * rocks
    ./drum.sh 3
Column collapsing
   * random shapes
    ./rest_angle.sh
   * pears
    ./rest_angle.sh 1
   * dragonflies
    ./rest_angle.sh 2
   * rocks
    ./rest_angle.sh 3


********************************
    
Usage:
./dem2d -c <config file> -s <scene file>

To get all the option
./dem -h 

Keyboard shortcut: see help window (Keyboard tab).

************

Work in progress:
  - import/export to replay a simulation
  - dynamic obstacle
  - svg export


**************

toogle light/dark mode with the _WHITE macro (or just uncomment the line in dem_conf_hpp)

***********

Examples of configuration files and scene files are in examples_article


