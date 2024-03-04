/* 
 * Copyright (C) 2024  Camille Schreck
 *
 * This file is part of the StarDEM library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
