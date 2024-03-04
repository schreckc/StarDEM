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
