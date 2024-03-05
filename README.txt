This is the code corresponding to the article "StarDEM: efficient discrete element method for star-shaped particles".

https://github.com/schreckc

*************************

Dependency:
Eigen3
qt5

**************

Compilation:
$ cd QGLViewer
$ qmake
$ make
$ sudo make install
$ cd ..
$ qmake dem2d.pro
$ make

Change the number of threads used by OpenMP:
$ export OMP_NUM_THREADS=1

**************

Script to reproduce the examples of the article: 
(to display the timings add the option -verbose)

Packing:
  * random shapes
    $ ./packing.sh
  * spheres
    $ ./packing.sh 1
  * crosses
    $ ./packing.sh 2
  * butterflies
    $ ./packing.sh 3
  * bats
    $ ./packing.sh 4
Bridge:
  * random shapes
    $ ./bridge.sh    
  * flowers
    $ ./bridge.sh 1
  * blobs
    $ ./bridge.sh 2
  * blobs + visualization of the force network
    $ ./bridge.sh 3
Drum:
   * random shapes
    $ ./drum.sh
   * flowers
    $ ./drum.sh 1
   * peanuts
    $ ./drum.sh 2
   * rocks
    $ ./drum.sh 3
Column collapsing
   * random shapes
    $ ./rest_angle.sh
   * pears
    $ ./rest_angle.sh 1
   * dragonflies
    $ ./rest_angle.sh 2
   * rocks
    $ ./rest_angle.sh 3


********************************
    
Usage:
$ ./dem2d -c <config file> -s <scene file>

To get all the option
$ ./dem -h 

Keyboard shortcut: see help window (Keyboard tab).

************

Work in progress:
  - import/export to replay a simulation
  - dynamic obstacle
  - svg export

********************

No-graphics mode:
The simulation runs without displaying the results on the viewer. The results have to be recorded using the -export option.
The results can then be replayed using -import in the graphics mode. Each frame is recorded in a custom format based on the obj format:

# comment
ns <number of smarticles> <radius>
v x0 y0 # coordinates of each smarticles
v x1 y1
... 
v xn yn
r a0 # rotation of each smarticles
r a1
...
r an

To use the no-graphics mode:
$ qmake dem2d_nographics.pro
$ make
$ ./dem_ng <options>


**************

Toogle light/dark mode with the _WHITE macro (or just uncomment the line in dem_conf_hpp)

***********

Examples of configuration files and scene files are in examples_article


