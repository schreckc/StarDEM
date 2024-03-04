#include "Obstacle.hpp"
#include "dem_conf.hpp"
#include "error.hpp"

Obstacle::Obstacle() : Object() {
  friction = dem_conf::friction_coef_;
  rotation = 0;
  force = VEC2(0, 0);
  dynamic = false;
  mass = 0;
  I = 0;
#ifdef _WHITE
  color = VEC3(0, 0, 0);
# else
  color = VEC3(1, 1, 1);
#endif
}

void Obstacle::animate() {
  force = VEC2(0, 0);
  for (auto& m : motions) {
    apply(m);
  }
}

void Obstacle::animate(FLOAT t) {
  if (dynamic) {
    torque -= 4000*avel;
    if (mass != 0) {
      vel += force/mass*dem_conf::dt_;
      G += vel*dem_conf::dt_;
    }
    if (I != 0) {
      avel += torque/I*dem_conf::dt_;
      rotation += avel*dem_conf::dt_;
    }
  } else {
    for (auto& m : motions) {
      apply(m);
    }
  }

  force = VEC2(0, 0);
  torque = 0;
  force += mass*dem_conf::gravity_;
  nb_neigh = 0;
  nb_contacts = 0;
}

FLOAT Obstacle::getFriction() const {
  return friction;
}

void Obstacle::setFriction(FLOAT f) {
  friction = f;
}

FLOAT Obstacle::getMass() {
  return mass;
}

void Obstacle::setMotions(std::list<Motion> lm) {
  motions = lm;
}

void Obstacle::addMotion(Motion m) {
  motions.push_back(m);
}

bool Obstacle::isMoving() const {
  return !motions.empty();
}

void Obstacle::addForce(VEC2 f) {
  force+=f;
}


VEC2 Obstacle::getForce() const {
  return force;
}

void Obstacle::startStop() {
  for (auto& m : motions) {
    m.is_running = !m.is_running;
  }
}

void Obstacle::setDynamic(FLOAT m, FLOAT i) {
  dynamic = true;
  mass = m;
  I =i;
}

void Obstacle::setColor(VEC3 c) {
  color = c;
}
