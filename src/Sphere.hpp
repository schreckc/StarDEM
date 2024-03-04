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

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <QGLViewer/qglviewer.h>
#include <list>

class Sphere {
private :
  float m_size;
  float cr, cg, cb;

  const static int parallels_count = 8;
  const static int meridians_count = 10;
  const static int size_array = 18*meridians_count + 18*meridians_count* (parallels_count-1);

  static GLfloat* vertices;
  static GLfloat* normals;
  
public :
  
  Sphere(float s = 1);
  ~Sphere();

  void setColor(float r, float g, float b);
  void setSize(float s);
  
  void draw();

  static void create_array();
  static void create_vertex(float theta, float phi, int &index);

};
  

#endif
