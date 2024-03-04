#include "Object.hpp"

Object::Object() {
  G = VEC2(0, 0);
  vel = VEC2(0, 0);
  avel = 0;

  force = VEC2(0, 0);
  torque = 0;
  
  nb_neigh = 0;
  nb_contacts = 0;

  smarticle = false;
}

