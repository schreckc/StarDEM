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

