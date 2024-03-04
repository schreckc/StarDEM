#include "PlaneObstacle.hpp"
#include "utils.hpp"

PlaneObstacle::PlaneObstacle(VEC2 p, VEC2 d, FLOAT l) : Obstacle() {
  pos = p;
  G = pos;
  dir = d;
  dir.normalize();
  normal = VEC2(-dir(1), dir(0));
  length = l;

  rotation = atan(dir(1)/dir(0));
}


PlaneObstacle::~PlaneObstacle() {}

void PlaneObstacle::apply(Motion m) {
  // if (m.rotation_center_def) {
  //   VEC3 diff = pos - m.center;
  //   diff = m.rotation*diff;
  //   pos = m.center + diff;
  // } else {
  //   pos += m.translation;
  //   length *= m.scale;
  //   width *= m.scale;
  // }
  // normal = m.rotation*normal;
  // v1 = m.rotation*v1;
  // v2 = m.rotation*v2;
  // v2.normalize();
  // v1.normalize();
  // normal.normalize();
  // rotation = m.rotation*rotation;
  TEST(false); //TODO
}
  
#ifndef NO_GRAPHICS_ 
void PlaneObstacle::draw() {
  FLOAT size = 1;
  FLOAT l = 100;
  if (length != 0) {
    l = length;
  }
  VEC2 c0 = pos + 0.5*l*dir;
  VEC2 c1 = pos -+ 0.5*l*dir;

  glColor3f(1, 1, 1);
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex3f(c0(0), c0(1), 0);
  glVertex3f(c1(0), c1(1), 0);
  glVertex3f(pos(0), pos(1), 0);
  glVertex3f(pos(0)+0.2*normal(0), pos(1)+0.2*normal(1), 0);
  glEnd();
  }
#endif

VEC2 PlaneObstacle::getPosition() const {
  return pos;
}

VEC2 PlaneObstacle::getNormal(VEC2 v) const {
  return normal;
}

VEC2 PlaneObstacle::getNormal() const {
  return normal;
}

FLOAT PlaneObstacle::distance(VEC2 v) const {
  VEC2 proj = utils::projectionOrtho(v, pos, dir);
  FLOAT dist = 10;
  if (length == 0 || fabs(dir.dot(proj-pos)) < 0.5*length) {
    dist = (proj - v).norm();
    if (normal.dot(v-pos) < 0) {
      dist = -dist;
    }
  }

  return dist;
}

void PlaneObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &n) const {
  VEC2 proj = utils::projectionOrtho(p, pos, dir);
  dist = 10;
  if (length == 0 || fabs(dir.dot(proj-pos)) < 0.5*length) {
    dist = (proj - p).norm();
    if (normal.dot(p-pos) < 0) {
      dist = -dist;
    }
  }
  n = normal;
}

void PlaneObstacle::rotate(FLOAT angle) {
  // VEC3 n = utils::rotation(normal, angle, axe);
  // normal = n;
  // rotation = utils::rotation(angle, axe)*rotation;
  TEST(false); //TODO
}


void PlaneObstacle::exportSVG(std::ofstream & file, int id) const {
  FLOAT c = 1000;
  FLOAT l = dem_conf::size_grid_.norm();
  if (length != 0) {
    l = length;
  }
  VEC2 c0 = c*(pos + 0.5*l*dir);
  VEC2 c1 = c*(pos - 0.5*l*dir);

    TR(c0(0) <<","<< c0(1));
  TR(c1(0) <<","<< c1(1));

  
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   file<<c0(0) <<","<< c0(1)<<" ";
   file<<c1(0) <<","<< c1(1);

   file<<"\"\n";
   file<<"       id=\"pathOBout"<<id<<"\" />\n";
}
