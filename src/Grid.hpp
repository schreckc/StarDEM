#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include <list>
#include "Eigen/Core"
#include "Smarticle.hpp"
#include "Obstacle.hpp"

using namespace Eigen;

class Grid {

private :
  uint nb_lines;

  FLOAT spacing;
  
  FLOAT x_max, y_max;
  FLOAT x_min, y_min;

  uint nb_nodes;
  uint nb_cells;

  std::vector<std::list<Smarticle*> > cells;
  std::list<Smarticle*> empty_list;
  
  inline uint index(uint i, uint j) const;
  VEC2I nodeFromIndex(int ind) const;
public :
  uint i_max, j_max;
  Grid();
  Grid(FLOAT width, FLOAT height, FLOAT space_step);

  const std::list<Smarticle*> & getSmarticles(int i, int j) const;

  VEC2 getMin() const;
  VEC2 getMax() const;
  
  void nextStep(std::vector<Smarticle*> & smarticles);
  void animate();

#ifndef NO_GRAPHICS_ 
  void draw();
#endif

  friend class Simulation;
};

#endif
