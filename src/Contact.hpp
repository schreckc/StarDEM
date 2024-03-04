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

#ifndef CONTACT_HPP
#define CONTACT_HPP

#include "Object.hpp"

class Smarticle;

class Contact {

public :
  Object *p1;
  Object *p2;
  VEC2 pt;
  VEC2 normal;
  FLOAT dist;
  VEC2 I1, I2;
  VEC2 force;
  FLOAT torque1, torque2;
  VEC2 du;
  
  VEC3 col;

  FLOAT a1, a2;
  
  Contact();
  Contact(VEC2 v, VEC2 n, FLOAT d);

  void setColor(VEC3 c);
  void setColor(FLOAT r, FLOAT g, FLOAT b);
  
  void merge(Contact *c);

  void findP2Influence();
  
  VEC2 computeForce();
  void addForce();
  
  void draw();
};


#endif
