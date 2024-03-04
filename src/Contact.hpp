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
