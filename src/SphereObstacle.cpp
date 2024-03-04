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


#include "SphereObstacle.hpp"
#include "utils.hpp"

SphereObstacle::SphereObstacle(VEC2 p, FLOAT r, FLOAT ha, VEC2 hn) : Obstacle(){
  pos = p;
  radius = r;

  hole_angle = ha;
  hole_normal = hn;
  cosangle = cos(hole_angle);

  G = pos;
} 

SphereObstacle::~SphereObstacle() {}

void SphereObstacle::animate(FLOAT t) {
  Obstacle::animate(t);
  pos = G;
}
  

void SphereObstacle::apply(Motion m) {
  pos += m.translation;
  radius *= m.scale;
  hole_normal = VEC2(cos(m.rotation)*hole_normal(0)-sin(m.rotation)*hole_normal(1), sin(m.rotation)*hole_normal(0)+cos(m.rotation)*hole_normal(1));
}

#ifndef NO_GRAPHICS_ 
void SphereObstacle::draw() {
  glColor3f(color(0), color(1), color(2));
  glPushMatrix();
  glTranslatef(pos(0), pos(1), 0);

  int nstep = dem_conf::sm_resolution_display;
  FLOAT a = 0;
  FLOAT astep = 2*M_PI/nstep;
  FLOAT eps = 1;
  glBegin(GL_LINES);
     
  for (uint i = 0; i <= nstep; ++i) {
    FLOAT cosalpha = hole_normal(0)*cos(a+0.5*astep) + hole_normal(1)*sin(a+0.5*astep);
    if (cosangle > cosalpha) {
      glVertex3f(radius*cos(a), radius*sin(a), 0);
      glVertex3f(radius*cos(a+astep), radius*sin(a+astep), 0);
      glVertex3f((radius+2*eps)*cos(a), (radius+2*eps)*sin(a), 0);
      glVertex3f((radius+2*eps)*cos(a+astep), (radius+2*eps)*sin(a+astep), 0);
    }
    a+=astep;
  }
  glEnd();
  glPopMatrix();

}
#endif
  
VEC2 SphereObstacle::getPosition() const {
  return pos;
}

FLOAT SphereObstacle::getRadius() const {
  return radius;
}


VEC2 SphereObstacle::getNormal(VEC2 p) const {
  VEC2 normal;
  
  VEC2 n = p - pos;
  FLOAT norm = n.norm();
  assert(norm != 0);
  n /= norm;

  normal = -n;
  
  FLOAT cosalpha = n.dot(hole_normal);
  FLOAT sinalpha = n(0)*hole_normal(1) - n(1)*hole_normal(0);
  if (cosangle < cosalpha) {
    FLOAT a = hole_angle;
    if (sinalpha < 0) {
      a = -a;
    }
    VEC2 dir = VEC2(n(0)*cos(a) + n(1)*sin(a), n(1)*cos(a) - n(0)*sin(a));  

    VEC2 closest = pos + radius * dir;
    normal = p - closest;
    normal.normalize();
  }
  return normal;
}

FLOAT SphereObstacle::distance(VEC2 v) const {
  FLOAT dist;
  VEC2 n = v - pos;
  FLOAT norm = n.norm();
  dist = radius - norm;

  VEC2 normal;
    
  FLOAT cosalpha = n.dot(hole_normal);
  FLOAT sinalpha = n(0)*hole_normal(1) - n(1)*hole_normal(0);
  if (cosangle < cosalpha) {
    FLOAT a = hole_angle;
    if (sinalpha < 0) {
      a = -a;
    }
    VEC2 dir = VEC2(n(0)*cos(a) + n(1)*sin(a), n(1)*cos(a) - n(0)*sin(a));  

    VEC2 closest = pos + radius * dir;
    normal = v - closest;
    dist = normal.norm();
  }
  

  return dist;
}

void SphereObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const {
  FLOAT eps = 1;
  VEC2 n = p - pos;
  FLOAT norm = n.norm();
  assert(norm != 0);
  n /= norm;

  normal = -n;
  dist = radius - norm;
  if (norm > radius + eps) {
    dist = norm - radius + 2*eps;
  }
   
  FLOAT cosalpha = n.dot(hole_normal);
  FLOAT sinalpha = n(0)*hole_normal(1) - n(1)*hole_normal(0);
  if (cosangle < cosalpha) {
    FLOAT a = hole_angle;
    if (sinalpha < 0) {
      a = -a;
    }
    VEC2 dir = VEC2(n(0)*cos(a) + n(1)*sin(a), n(1)*cos(a) - n(0)*sin(a));  
    VEC2 closest = pos + radius * dir;
    VEC2 closest2 = pos + (radius + 2*eps) * dir;
    VEC2 d = closest2 - closest;
    FLOAT coef;
    VEC2 proj = utils::projectionOrtho(p, closest, d, coef);
    if (coef < 0) {
      normal = p - closest;
    } else if (coef > 1) {
      normal = p - closest2;
    } else {
      normal = p - proj;
    }
    dist = normal.norm();
    normal.normalize();
  }
}


void SphereObstacle::exportSVG(std::ofstream & file, int id) const {
  TEST(false);//TODO
}
