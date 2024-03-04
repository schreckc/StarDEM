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

#include "Sphere.hpp"

#include <iostream>
#include <cassert>



GLfloat* Sphere::vertices = new GLfloat[Sphere::size_array];
GLfloat* Sphere::normals = new GLfloat[Sphere::size_array];

Sphere::Sphere(float s) {
    m_size = s;
    setColor(1, 1, 1);
  }

 Sphere::~Sphere() {
  }

void Sphere::setColor(float r, float g, float b) {
   cr = r;
   cg = g;
   cb = b;
}

void Sphere::setSize(float s) {
   m_size = s;
}

void Sphere::draw() {
  glPushMatrix();
  glScalef(m_size, m_size, m_size);
 
  glBegin(GL_TRIANGLE_STRIP);
  glColor3f(cr, cg, cb);
  for (int i = 0; i < size_array; i+=3) {
    glNormal3f(normals[i], normals[i+1], normals[i+2]);
    glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
  }
  glEnd();
  glPopMatrix();
}




void Sphere::create_array() {
    GLfloat phi_prec = 0;
    GLfloat phi_step = M_PI / (parallels_count + 1);
    GLfloat theta_prec = 0;
    GLfloat theta_step = 2*M_PI / (meridians_count);

    GLfloat phi = phi_step;
    GLfloat theta = 0;
    int index = 0;

    for (int i = 0; i < meridians_count; ++i) {
      theta_prec = theta;
      theta += theta_step;
      create_vertex(theta_prec, phi, index);
      create_vertex(theta, phi, index);
      create_vertex(theta, phi_prec, index);
     }

    for (int j = 0; j < parallels_count-1; ++j) {
      phi_prec = phi;
      phi += phi_step;
      theta = 0;
      for (int i = 0; i < meridians_count; ++i) {
	theta_prec = theta;
	theta += theta_step;
	create_vertex(theta, phi_prec, index);
	create_vertex(theta_prec, phi_prec, index);
	create_vertex(theta, phi, index);
	create_vertex(theta_prec, phi, index);
	create_vertex(theta, phi, index);
	create_vertex(theta_prec, phi_prec, index);
      }
    }
    phi_prec = phi;
    phi = M_PI;
    theta = 0;
    for (uint i = 0; i < meridians_count; ++i) {
      theta_prec = theta;
      theta += theta_step;
      create_vertex(theta, phi_prec, index);
      create_vertex(theta_prec, phi_prec, index);
      create_vertex(theta, phi, index);
    }
  }

void Sphere::create_vertex(float theta, float phi, int &index) {
  vertices[index] = cos(theta)*sin(phi);
  vertices[index+1] = sin(theta)*sin(phi);
  vertices[index+2] = cos(phi);
  normals[index] = cos(theta)*sin(phi);
  normals[index+1] = sin(theta)*sin(phi);
  normals[index+2] = cos(phi);
  index += 3;
}

