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

**************

Script:
  * random shapes
    ./bridge.sh    
  * flowers
    ./bridge.sh 1
  * blobs
    ./bridge.sh 2
  * blobs + visualization of the force network
    ./bridge.sh 3
    
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


