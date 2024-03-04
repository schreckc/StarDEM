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

#include "Grid.hpp"
#include "error.hpp"
#include "dem_conf.hpp"

inline uint Grid::index(uint i, uint j) const {
  return i*(j_max) + j;
}

VEC2I Grid::nodeFromIndex(int ind) const {
  int i, j;
  i = (int) ind / (j_max+1);
  j = ind - i*(j_max+1);
  return VEC2I(i, j);
}

Grid::Grid() : x_max(0),  y_max(0), x_min(0),  y_min(0), i_max(0),  j_max(0) {
  empty_list =  std::list<Smarticle*>();
}

Grid::Grid(FLOAT width, FLOAT height, FLOAT space_step) {
  empty_list =  std::list<Smarticle*>();
  x_max = width;
  y_max = height;

  x_min = 0;
  y_min = 0;
  
  spacing = space_step;
  
  i_max = x_max/spacing + 1;
  j_max = y_max/spacing + 1;

  nb_lines = (i_max+1)*(j_max+1);
  nb_nodes = (i_max+1)*(j_max+1);
  nb_cells = (i_max)*(j_max);
  
  cells = std::vector<std::list<Smarticle*> >(nb_cells);

  for (uint i = 0; i < i_max; ++i) {
    for (uint j = 0; j < j_max; ++j) {
      cells[index(i, j)] = std::list<Smarticle*>();
    }
  }
}


void Grid::animate() {
  
}

#ifndef NO_GRAPHICS_ 
void Grid::draw() {

  if (dem_conf::display_grid) {
    glColor3f(0, 0, 1);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex3f(x_min, y_min, 0);
    glVertex3f(x_max, y_min, 0);
    glVertex3f(x_min, y_min, 0);
    glVertex3f(x_min, y_max, 0);
    glVertex3f(x_max, y_max, 0);
    glVertex3f(x_max, y_min, 0);
    glVertex3f(x_max, y_max, 0);
    glVertex3f(x_min, y_max, 0);
  
    glEnd();

    
    FLOAT h = spacing;
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (uint i = 0; i <= i_max; ++i) {
      glVertex3f(i*h, y_min, 0);
      glVertex3f(i*h, y_max, 0);
    }
    for (uint j = 0; j <= j_max; ++j) {
      glVertex3f(x_min, j*h, 0);
      glVertex3f(x_max, j*h, 0);
    }
    glEnd();
  }
}
#endif


const std::list<Smarticle*> & Grid::getSmarticles(int i, int j) const {
  if (i >= 0 && i < i_max && j >= 0 && j < j_max) {
    return cells[index(i, j)];
  } else {
    return empty_list;
  }
}

VEC2 Grid::getMin() const {
  return VEC2(x_min, y_min);
}
VEC2 Grid::getMax() const {
  return VEC2(x_max, y_max);
}

void Grid::nextStep(std::vector<Smarticle*> & smarticles) {
  for (uint i = 0; i < nb_cells; ++i) {
    cells[i].clear();
  }
  for (auto &s : smarticles) {
    VEC2I c = s->getCell();
    uint ind = c(0)*j_max + c(1);
    if (ind < nb_cells) {
      cells[ind].push_back(s);
    }
  }
}

