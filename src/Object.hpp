#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <QGLViewer/qglviewer.h>
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
