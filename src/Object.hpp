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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "QGLViewer/qglviewer.h"
#include "definitions.hpp"


class Object {
  
protected:

  VEC2 force;
  FLOAT torque; 
  VEC2 G; //gravity center
  VEC2 vel; //velocity
  FLOAT avel; // angular velocity
  
  uint nb_neigh;
  uint nb_contacts;

  bool smarticle;
  
public:
Object();

  virtual void addForce(VEC2 f) = 0;

  virtual void animate() = 0;
#ifndef NO_GRAPHICS_ 
  virtual void draw() = 0;
#endif

  friend class Contact;

};

#endif
