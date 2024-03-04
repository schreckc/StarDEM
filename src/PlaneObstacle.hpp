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

#ifndef PLANE_OBSTACLE_HPP
#define PLANE_OBSTACLE_HPP

#include "Obstacle.hpp"

class PlaneObstacle : public Obstacle {
private :
  VEC2 pos;
  VEC2 dir;
  VEC2 normal;
  FLOAT length;
  
public :
  PlaneObstacle(VEC2 p = VEC2(0, 0), VEC2 d = VEC3(0, 1), FLOAT l = 0);
  ~PlaneObstacle();

  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw();
#endif
  
  VEC2 getPosition() const;
  VEC2 getNormal(VEC2 pos) const;
  VEC2 getNormal() const;
  
  FLOAT distance(VEC2 v) const;
  void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &n) const;

  void rotate(FLOAT angle);
  void exportSVG(std::ofstream & file, int id) const;
};

#endif
 
