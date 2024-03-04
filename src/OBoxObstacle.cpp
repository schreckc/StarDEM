#include "OBoxObstacle.hpp"

OBoxObstacle::OBoxObstacle(VEC2 min_p, VEC2 max_p) : Obstacle(), min_pos(min_p), max_pos(max_p) {
  G = 0.5*(min_pos+max_pos);
}

OBoxObstacle::~OBoxObstacle() {}

void OBoxObstacle::animate(FLOAT t) {
  VEC2 prev_G = G;
  Obstacle::animate(t);
  min_pos += G - prev_G;
  max_pos += G - prev_G;
}

void OBoxObstacle::apply(Motion m) {
  min_pos += m.translation;
  max_pos += m.translation;

  VEC2 half = 0.5*(max_pos - min_pos);
  VEC2 middle = min_pos + 0.5*half;
  half *= m.scale;
  min_pos = middle - half;
  max_pos = middle + half;
}
  
#ifndef NO_GRAPHICS_ 
void OBoxObstacle::draw() {
  glColor3f(color(0), color(1), color(2));
  glBegin(GL_LINES);
  glVertex3f(min_pos(0), min_pos(1), 0);
  glVertex3f(min_pos(0), max_pos(1), 0);
  
  glVertex3f(min_pos(0), max_pos(1), 0);
  glVertex3f(max_pos(0), max_pos(1), 0);
  
  glVertex3f(max_pos(0), max_pos(1), 0);
  glVertex3f(max_pos(0), min_pos(1), 0);

  glVertex3f(max_pos(0), min_pos(1), 0);
  glVertex3f(min_pos(0), min_pos(1), 0);
  glEnd();

  if (!dem_conf::wireframe_) {
    glColor3f(abs(color(0)-0.5), abs(color(1) - 0.5), abs(color(2)-0.5));
    glBegin(GL_TRIANGLES);

    glVertex3f(min_pos(0), min_pos(1), 0);
    glVertex3f(min_pos(0), max_pos(1), 0);
    glVertex3f(max_pos(0), max_pos(1), 0);
    
    glVertex3f(max_pos(0), max_pos(1), 0);
    glVertex3f(min_pos(0), min_pos(1), 0);
    glVertex3f(max_pos(0), min_pos(1), 0);

    glEnd();
  }
  
}
#endif

VEC2 OBoxObstacle::getNormal(VEC2 p) const {
  VEC2 normal;
  VEC2 closest;
  if (p(0) < min_pos(0)) {
    if ( p(1) > max_pos(1)) {
      closest = VEC2(min_pos(0) , max_pos(1) );
      normal = p - closest;
      normal.normalize();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(min_pos(0) , min_pos(1) );
      normal = p - closest;
      normal.normalize();
    } else {
      normal = VEC2(-1, 0);
    }
  } else if (p(0) > max_pos(0) ) {
    if ( p(1) > max_pos(1)) {
      closest = VEC2(max_pos(0) , max_pos(1) );
      normal = p - closest;
      normal.normalize();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(max_pos(0) , min_pos(1) );
      normal = p - closest;
      normal.normalize();
    } else {
      normal = VEC2(1, 0);
    }
  } else if (p(1) < min_pos(1) ) {
    normal = VEC2(0, -1);
  } else if (p(1) > max_pos(1) ) {
    normal = VEC2(0, 1);
  } else {
    VEC2 dist_to_min, dist_to_max;
    FLOAT min_d = 10*(p(0) - min_pos(0)) ;
    int i_min = 0;
    int dir = 1;
    for (uint i = 0; i < 2; ++i) {
      dist_to_min(i) = p(i) - min_pos(i) ;
      dist_to_max(i) = max_pos(i)  - p(i);
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
  
  return normal;
}


FLOAT OBoxObstacle::distance(VEC2 p) const {
  VEC2 closest;
  FLOAT dist;
  VEC2 normal;
  if (p(0) < min_pos(0) ) {
    if ( p(1) > max_pos(1) ) {
      closest = VEC2(min_pos(0) , max_pos(1) );
      normal = p - closest;
      dist = normal.norm();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(min_pos(0) , min_pos(1) );
      normal = p - closest;
      dist = normal.norm();
    } else {
      dist =  min_pos(0)  - p(0);
    }
  } else if (p(0) > max_pos(0) ) {
    if ( p(1) > max_pos(1)) {
      closest = VEC2(max_pos(0) , max_pos(1) );
      normal = p - closest;
      dist = normal.norm();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(max_pos(0) , min_pos(1) );
      normal = p - closest;
      dist = normal.norm();
    } else {
      dist =  p(0) - max_pos(0) ;
      normal = VEC2(1, 0);
    }
  } else if (p(1) < min_pos(1) ) {
    dist =  min_pos(1)  - p(1);
    normal = VEC2(0, -1);
  } else if (p(1) > max_pos(1) ) {
    dist =  p(1) - max_pos(1) ;
  } else {
    VEC2 dist_to_min, dist_to_max;
    FLOAT min_d = 10*(p(0) - min_pos(0)) ;
    int i_min = 0;
    int dir = 1;
    for (uint i = 0; i < 2; ++i) {
      dist_to_min(i) = p(i) - min_pos(i) ;
      dist_to_max(i) = max_pos(i)  - p(i);
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
  
  return dist;
}

void OBoxObstacle::getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const {
  
  VEC2 closest;
  if (p(0) < min_pos(0) ) {
    if ( p(1) > max_pos(1) ) {
      closest = VEC2(min_pos(0) , max_pos(1) );
      normal = p - closest;
      dist = normal.norm();
      normal.normalize();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(min_pos(0) , min_pos(1) );
      normal = p - closest;
      dist = normal.norm();
      normal.normalize();
    } else {
      dist =  min_pos(0)  - p(0);
      normal = VEC2(-1, 0);
    }
  } else if (p(0) > max_pos(0) ) {
    if ( p(1) > max_pos(1)) {
      closest = VEC2(max_pos(0) , max_pos(1) );
      normal = p - closest;
      dist = normal.norm();
      normal.normalize();
    } else if (p(1) < min_pos(1) ) {
      closest = VEC2(max_pos(0) , min_pos(1) );
      normal = p - closest;
      dist = normal.norm();
      normal.normalize();
    } else {
      dist =  p(0) - max_pos(0) ;
      normal = VEC2(1, 0);
    }
  } else if (p(1) < min_pos(1) ) {
    dist =  min_pos(1)  - p(1);
    normal = VEC2(0, -1);
  } else if (p(1) > max_pos(1) ) {
    dist =  p(1) - max_pos(1) ;
    normal = VEC2(0, 1);
  } else {
    VEC2 dist_to_min, dist_to_max;
    FLOAT min_d = 10*(p(0) - min_pos(0));
    int i_min = 0;
    int dir = 1;
    for (uint i = 0; i < 2; ++i) {
      dist_to_min(i) = p(i) - min_pos(i) ;
      dist_to_max(i) = max_pos(i)  - p(i);
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
}

void OBoxObstacle::exportSVG(std::ofstream & file, int id) const {
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
