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

#include "BoxObstacle.hpp"


BoxObstacle::BoxObstacle(VEC2 min_p, VEC2 max_p) : Obstacle(), min_pos(min_p), max_pos(max_p) {
  G = 0.5*(min_pos+max_pos);
}

BoxObstacle::~BoxObstacle() {}

void BoxObstacle::apply(Motion m) {
  min_pos += m.translation;
  max_pos += m.translation;

  VEC2 half = 0.5*(max_pos - min_pos);
  VEC2 middle = min_pos + half;
  half *= m.scale;
  min_pos = middle - half;
  max_pos = middle + half;
}
  
#ifndef NO_GRAPHICS_ 
void BoxObstacle::draw() {
  FLOAT eps = 1;
  glColor3f(color(0), color(1), color(2));
  glBegin(GL_LINES);
  glVertex3f(min_pos(0) - 2*eps, min_pos(1) - 2*eps, 0);
  glVertex3f(min_pos(0) - 2*eps, max_pos(1) + 2*eps, 0);
  
  glVertex3f(min_pos(0) - 2*eps, max_pos(1) + 2*eps, 0);
  glVertex3f(max_pos(0) + 2*eps, max_pos(1) + 2*eps, 0);
  
  glVertex3f(max_pos(0) + 2*eps, max_pos(1) + 2*eps, 0);
  glVertex3f(max_pos(0) + 2*eps, min_pos(1) - 2*eps, 0);

  glVertex3f(max_pos(0) + 2*eps, min_pos(1) - 2*eps, 0);
  glVertex3f(min_pos(0) - 2*eps, min_pos(1) - 2*eps, 0);

  glVertex3f(min_pos(0), min_pos(1), 0);
  glVertex3f(min_pos(0), max_pos(1), 0);
  
  glVertex3f(min_pos(0), max_pos(1), 0);
  glVertex3f(max_pos(0), max_pos(1), 0);
  
  glVertex3f(max_pos(0), max_pos(1), 0);
  glVertex3f(max_pos(0), min_pos(1), 0);

  glVertex3f(max_pos(0), min_pos(1), 0);
  glVertex3f(min_pos(0), min_pos(1), 0);
  glEnd();
  
}
#endif

VEC2 BoxObstacle::getNormal(VEC2 p) const {
  VEC2 normal;
  FLOAT eps = 0.01;
  VEC2 closest;

 if (p(0) < min_pos(0) -eps || p(1) < min_pos(1)-eps || p(0) > max_pos(0)+eps || p(1) > max_pos(1)+eps) {
    if (p(0) < min_pos(0) - 2*eps) {
      if ( p(1) > max_pos(1) + 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	normal.normalize();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	normal.normalize();
      } else {
	normal = VEC2(-1, 0);
      }
    } else if (p(0) > max_pos(0) + 2*eps) {
      if ( p(1) > max_pos(1)+2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	normal.normalize();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	normal.normalize();
      } else {
	normal = VEC2(1, 0);
      }
    } else if (p(1) < min_pos(1) - 2*eps) {
	normal = VEC2(0, -1);
    } else if (p(1) > max_pos(1) + 2*eps) {
      normal = VEC2(0, 1);
    } else {
      VEC2 dist_to_min, dist_to_max;
      FLOAT min_d = 10*(p(0) - min_pos(0) - 2*eps);
      int i_min = 0;
      int dir = 1;
      for (uint i = 0; i < 2; ++i) {
	dist_to_min(i) = p(i) - min_pos(i) + 2*eps;
	dist_to_max(i) = max_pos(i) + 2*eps - p(i);
	if (fabs(dist_to_min(i)) < fabs(min_d)) {
	  min_d = dist_to_min(i);
	  i_min = i;
	  dir = -1;
	}
	if (fabs(dist_to_max(i)) < fabs(min_d)) {
	  min_d = dist_to_max(i);
	  i_min = i;
	  dir = 1;
	}
      }
      normal(i_min) = (FLOAT)dir*1.0;
      normal((i_min+1)%2) = 0.0;
    }
 } else {
   VEC2 dist_to_min, dist_to_max;
   FLOAT min_d = 10*(p(0) - min_pos(0));
   int i_min = 0;
   int dir = 1;
   for (uint i = 0; i < 2; ++i) {
     dist_to_min(i) = p(i) - min_pos(i);
     dist_to_max(i) = max_pos(i) - p(i);
     if (fabs(dist_to_min(i)) < fabs(min_d)) {
       min_d = dist_to_min(i);
       i_min = i;
       dir = 1;
     }
     if (fabs(dist_to_max(i)) < fabs(min_d)) {
       min_d = dist_to_max(i);
       i_min = i;
       dir = -1;
     }
   }
   normal(i_min) = (FLOAT)dir*1.0;
   normal((i_min+1)%2) = 0.0;
 }
  
  return normal;
}


FLOAT BoxObstacle::distance(VEC2 p) const {
   FLOAT eps = 0.01;
  VEC2 closest;
  FLOAT dist;
  VEC2 normal;
 if (p(0) < min_pos(0) -eps || p(1) < min_pos(1)-eps || p(0) > max_pos(0)+eps || p(1) > max_pos(1)+eps) {
    if (p(0) < min_pos(0) - 2*eps) {
      if ( p(1) > max_pos(1) + 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	dist = normal.norm();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	dist = normal.norm();
      } else {
	dist =  min_pos(0) - 2*eps - p(0);
      }
    } else if (p(0) > max_pos(0) + 2*eps) {
      if ( p(1) > max_pos(1)+2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	dist = normal.norm();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	dist = normal.norm();
      } else {
	dist =  p(0) - max_pos(0) - 2*eps;
	normal = VEC2(1, 0);
      }
    } else if (p(1) < min_pos(1) - 2*eps) {
      	dist =  min_pos(1) - 2*eps - p(1);
	normal = VEC2(0, -1);
    } else if (p(1) > max_pos(1) + 2*eps) {
      dist =  p(1) - max_pos(1) - 2*eps;
      // TR("OUT "<<dist<<" "<<p(1));
    } else {
      VEC2 dist_to_min, dist_to_max;
      FLOAT min_d = 10*(p(0) - min_pos(0) - 2*eps);
      int i_min = 0;
      int dir = 1;
      for (uint i = 0; i < 2; ++i) {
	dist_to_min(i) = p(i) - min_pos(i) + 2*eps;
	dist_to_max(i) = max_pos(i) + 2*eps - p(i);
	if (fabs(dist_to_min(i)) < fabs(min_d)) {
	  min_d = dist_to_min(i);
	  i_min = i;
	  dir = -1;
	}
	if (fabs(dist_to_max(i)) < fabs(min_d)) {
	  min_d = dist_to_max(i);
	  i_min = i;
	  dir = 1;
	}
      }
      dist = -min_d;
    }
 } else {
   VEC2 dist_to_min, dist_to_max;
   FLOAT min_d = 10*(p(0) - min_pos(0));
   int i_min = 0;
   int dir = 1;
   for (uint i = 0; i < 2; ++i) {
     dist_to_min(i) = p(i) - min_pos(i);
     dist_to_max(i) = max_pos(i) - p(i);
     if (fabs(dist_to_min(i)) < fabs(min_d)) {
       min_d = dist_to_min(i);
       i_min = i;
       dir = 1;
     }
     if (fabs(dist_to_max(i)) < fabs(min_d)) {
       min_d = dist_to_max(i);
       i_min = i;
       dir = -1;
     }
   }
   dist = min_d;
 }
  
  return dist;
}

void BoxObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const {
  
  FLOAT eps = 10;
  VEC2 closest;
 if (p(0) < min_pos(0) -eps || p(1) < min_pos(1)-eps || p(0) > max_pos(0)+eps || p(1) > max_pos(1)+eps) {
    if (p(0) < min_pos(0) - 2*eps) {
      if ( p(1) > max_pos(1) + 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	dist = normal.norm();
	normal.normalize();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(min_pos(0) - 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	dist = normal.norm();
	normal.normalize();
      } else {
	dist =  min_pos(0) - 2*eps - p(0);
	normal = VEC2(-1, 0);
      }
    } else if (p(0) > max_pos(0) + 2*eps) {
      if ( p(1) > max_pos(1)+2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, max_pos(1) + 2*eps);
	normal = p - closest;
	dist = normal.norm();
	normal.normalize();
      } else if (p(1) < min_pos(1) - 2*eps) {
	closest = VEC2(max_pos(0) + 2*eps, min_pos(1) - 2*eps);
	normal = p - closest;
	dist = normal.norm();
	normal.normalize();
      } else {
	dist =  p(0) - max_pos(0) - 2*eps;
	normal = VEC2(1, 0);
      }
    } else if (p(1) < min_pos(1) - 2*eps) {
      	dist =  min_pos(1) - 2*eps - p(1);
	normal = VEC2(0, -1);
    } else if (p(1) > max_pos(1) + 2*eps) {
      dist =  p(1) - max_pos(1) - 2*eps;
      normal = VEC2(0, 1);
    } else {
      VEC2 dist_to_min, dist_to_max;
      FLOAT min_d = 10*(p(0) - min_pos(0) - 2*eps);
      int i_min = 0;
      int dir = 1;
      for (uint i = 0; i < 2; ++i) {
	dist_to_min(i) = p(i) - min_pos(i) + 2*eps;
	dist_to_max(i) = max_pos(i) + 2*eps - p(i);
	if (fabs(dist_to_min(i)) < fabs(min_d)) {
	  min_d = dist_to_min(i);
	  i_min = i;
	  dir = -1;
	}
	if (fabs(dist_to_max(i)) < fabs(min_d)) {
	  min_d = dist_to_max(i);
	  i_min = i;
	  dir = 1;
	}
      }
      normal(i_min) = (FLOAT)dir*1.0;
      normal((i_min+1)%2) = 0.0;
      dist = -min_d;
    }
 } else {
   VEC2 dist_to_min, dist_to_max;
   FLOAT min_d = 10*(p(0) - min_pos(0));
   int i_min = 0;
   int dir = 1;
   for (uint i = 0; i < 2; ++i) {
     dist_to_min(i) = p(i) - min_pos(i);
     dist_to_max(i) = max_pos(i) - p(i);
     if (fabs(dist_to_min(i)) < fabs(min_d)) {
       min_d = dist_to_min(i);
       i_min = i;
       dir = 1;
     }
     if (fabs(dist_to_max(i)) < fabs(min_d)) {
       min_d = dist_to_max(i);
       i_min = i;
       dir = -1;
     }
   }
   normal(i_min) = (FLOAT)dir*1.0;
   normal((i_min+1)%2) = 0.0;
   dist = min_d;
 }
}

void BoxObstacle::exportSVG(std::ofstream & file, int id) const {
  FLOAT eps = 0.01;
  FLOAT c = 1000;
  VEC2 p1 = c*VEC2(min_pos(0) - 2*eps, min_pos(1) - 2*eps);
  VEC2 p2 = c*VEC2(min_pos(0) - 2*eps, max_pos(1) + 2*eps);
  VEC2 p3 = c*VEC2(max_pos(0) + 2*eps, max_pos(1) + 2*eps);
  VEC2 p4 = c*VEC2(max_pos(0) + 2*eps, min_pos(1) - 2*eps);
  
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   file<<p1(0) <<","<< p1(1)<<" ";
   file<<p2(0) <<","<< p2(1)<<" ";
   file<<p3(0) <<","<< p3(1)<<" ";
   file<<p4(0) <<","<< p4(1)<<" ";

   file<<"z\"\n";
   file<<"       id=\"pathOBout"<<id<<"\" />\n";

   file<<"    <path\n";
   file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   file<<c*min_pos(0) <<","<< c*min_pos(1)<<" ";
   file<<c*min_pos(0) <<","<< c*max_pos(1)<<" ";
   file<<c*max_pos(0) <<","<< c*max_pos(1)<<" ";
   file<<c*max_pos(0) <<","<< c*min_pos(1)<<" ";

   file<<"z\"\n";
   file<<"       id=\"pathOBout"<<id<<"\" />\n";
}
