#include "WallObstacle.hpp"
#include "error.hpp"
#include "utils.hpp"

WallObstacle::WallObstacle(uint a, FLOAT p, int d) : Obstacle() {
  pos = p;
  axis = a;
  normal = VEC2(0, 0);
  dir = d;
  normal((a+1)%2) = d;
  G = VEC2(axis*pos, (1-axis)*pos);
}


WallObstacle::~WallObstacle() {}

void WallObstacle::apply(Motion m) {
  pos += m.translation(1-axis);  
}
  
#ifndef NO_GRAPHICS_ 
void WallObstacle::draw() {
  FLOAT size = 1000;
  glColor3f(color(0), color(1), color(2));
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex3f(axis*pos, (1-axis)*pos, 0);
  glVertex3f(axis*pos + (1-axis)*size, (1-axis)*pos + axis*size, 0);
  glVertex3f(axis*pos + (1-axis)*size/2, (1-axis)*pos + axis*size/2, 0);
  glVertex3f(axis*pos + (1-axis)*size/2+0.2*normal(0), (1-axis)*pos + axis*size/2+0.2*normal(1), 0);
  glEnd();
}
#endif

FLOAT WallObstacle::getPosition() const {
  return pos;
}

VEC2 WallObstacle::getNormal(VEC2 v) const {
  return normal;
}

VEC2 WallObstacle::getNormal() const {
  return normal;
}

FLOAT WallObstacle::distance(VEC2 v) const {
  FLOAT dist = dir*(v((axis+1)%2) - pos);
  return dist;
}

void WallObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &n) const {
  dist = dir*(p((axis+1)%2) - pos);
  n = normal;
}

void WallObstacle::rotate(FLOAT angle) {
  WARNING(false, "Wall cannot rotate", "");
}


void WallObstacle::exportSVG(std::ofstream & file, int id) const {
  FLOAT size = 600;
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

  file<<"       d=\"M ";
  file<<1000*axis*pos <<","<< (1-axis)*1000*pos<<" ";
  file<<axis*1000*pos + (1-axis)*size <<","<<(1-axis)*1000*pos + axis*size <<" ";
  file<<"z\"\n";
  file<<"       id=\"pathOB"<<id<<"\" />\n";
}
