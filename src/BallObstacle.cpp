#include "BallObstacle.hpp"
#include "error.hpp"
#include "Sphere.hpp"
#include "dem_conf.hpp"

BallObstacle::BallObstacle(VEC2 p, FLOAT r) : Obstacle(){
  pos = p;
  radius = r;
  G = pos;
}

BallObstacle::~BallObstacle() {
}

void BallObstacle::animate(FLOAT t) {
  Obstacle::animate(t);
  pos = G;
}

void BallObstacle::apply(Motion m) {
  pos += m.translation;
  radius *= m.scale;
}

#ifndef NO_GRAPHICS_ 
void BallObstacle::draw() {
   glColor3f(1, 1, 1);
  glPushMatrix();
  glTranslatef(pos(0), pos(1), 0);

  int nstep = dem_conf::sm_resolution_display;
  FLOAT a = 0;
  FLOAT astep = 2*M_PI/nstep;
     glBegin(GL_LINES);
    for (uint i = 0; i <= nstep; ++i) {
      glVertex3f(0, 0, 0);
      glVertex3f(radius*cos(a), radius*sin(a), 0);
      glVertex3f(radius*cos(a), radius*sin(a), 0);
      glVertex3f(radius*cos(a+astep), radius*sin(a+astep), 0);
      a+=astep;
    }
    glEnd();
   glPopMatrix();
}
#endif

VEC2 BallObstacle::getPosition() const {
  return pos;
}

FLOAT BallObstacle::getRadius() const {
  return radius;
}


VEC2 BallObstacle::getNormal(VEC2 p) const {
  VEC2 n = p - pos;
  n.normalize();
  return n;
}

FLOAT BallObstacle::distance(VEC2 v) const {
  VEC2 n = v - pos;
  return n.norm() - radius;
}

void BallObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const {
  VEC2 n = p - pos;
  FLOAT norm = n.norm();
  assert(norm != 0);
  n /= norm;

  normal = n;
  dist = norm - radius;
}

void BallObstacle::exportMitsuba(std::ofstream & file) const {
  TEST(false); //TODO
}

void BallObstacle::exportSVG(std::ofstream & file, int id) const {
  TEST(false);//TODO
}
