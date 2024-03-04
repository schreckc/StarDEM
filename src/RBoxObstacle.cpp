#include "RBoxObstacle.hpp"

RBoxObstacle::RBoxObstacle(VEC2 po, VEC2 no, VEC2 si) : Obstacle(), pos(po), n(no), hsize(si) {
  
  n.normalize();
  t = VEC2(n(1), -n(0));
  G = pos;
}

RBoxObstacle::~RBoxObstacle() {}

void RBoxObstacle::apply(Motion m) {
  hsize *= m.scale;
  
  n = VEC2(cos(m.rotation)*n(0)-sin(m.rotation)*n(1), sin(m.rotation)*n(0)+cos(m.rotation)*n(1));
  t = VEC2(cos(m.rotation)*t(0)-sin(m.rotation)*t(1), sin(m.rotation)*t(0)+cos(m.rotation)*t(1));
  rotation += m.rotation;

  VEC2 d = pos - m.center;
  d = VEC2(cos(m.rotation)*d(0)-sin(m.rotation)*d(1), sin(m.rotation)*d(0)+cos(m.rotation)*d(1));
  pos = m.center + d;
  pos += m.translation;
  G = pos;
}
  
#ifndef NO_GRAPHICS_ 
void RBoxObstacle::draw() {
  FLOAT eps = 0.01;
  glColor3f(1, 1, 1);
  glPushMatrix();
  glTranslatef(pos(0), pos(1), 0);
  glBegin(GL_LINES);

  glVertex3f(hsize(0)*t(0)+hsize(1)*n(0), hsize(0)*t(1)+hsize(1)*n(1), 0);
  glVertex3f(hsize(0)*t(0)-hsize(1)*n(0), hsize(0)*t(1)-hsize(1)*n(1), 0);

  glVertex3f(hsize(0)*t(0)-hsize(1)*n(0), hsize(0)*t(1)-hsize(1)*n(1), 0);
  glVertex3f(-hsize(0)*t(0)-hsize(1)*n(0), -hsize(0)*t(1)-hsize(1)*n(1), 0);

  glVertex3f(-hsize(0)*t(0)-hsize(1)*n(0), -hsize(0)*t(1)-hsize(1)*n(1), 0);
  glVertex3f(-hsize(0)*t(0)+hsize(1)*n(0), -hsize(0)*t(1)+hsize(1)*n(1), 0);
  
  glVertex3f(-hsize(0)*t(0)+hsize(1)*n(0), -hsize(0)*t(1)+hsize(1)*n(1), 0);
  glVertex3f(hsize(0)*t(0)+hsize(1)*n(0), hsize(0)*t(1)+hsize(1)*n(1), 0);


  
  glVertex3f((hsize(0)+2*eps)*t(0)+(hsize(1)+2*eps)*n(0), (hsize(0)+2*eps)*t(1)+(hsize(1)+2*eps)*n(1), 0);
  glVertex3f((hsize(0)+2*eps)*t(0)-(hsize(1)+2*eps)*n(0), (hsize(0)+2*eps)*t(1)-(hsize(1)+2*eps)*n(1), 0);

  glVertex3f((hsize(0)+2*eps)*t(0)-(hsize(1)+2*eps)*n(0), (hsize(0)+2*eps)*t(1)-(hsize(1)+2*eps)*n(1), 0);
  glVertex3f(-(hsize(0)+2*eps)*t(0)-(hsize(1)+2*eps)*n(0), -(hsize(0)+2*eps)*t(1)-(hsize(1)+2*eps)*n(1), 0);

  glVertex3f(-(hsize(0)+2*eps)*t(0)-(hsize(1)+2*eps)*n(0), -(hsize(0)+2*eps)*t(1)-(hsize(1)+2*eps)*n(1), 0);
  glVertex3f(-(hsize(0)+2*eps)*t(0)+(hsize(1)+2*eps)*n(0), -(hsize(0)+2*eps)*t(1)+(hsize(1)+2*eps)*n(1), 0);
  
  glVertex3f(-(hsize(0)+2*eps)*t(0)+(hsize(1)+2*eps)*n(0), -(hsize(0)+2*eps)*t(1)+(hsize(1)+2*eps)*n(1), 0);
  glVertex3f((hsize(0)+2*eps)*t(0)+(hsize(1)+2*eps)*n(0), (hsize(0)+2*eps)*t(1)+(hsize(1)+2*eps)*n(1), 0);

  glEnd();

  glPopMatrix();
}
#endif

VEC2 RBoxObstacle::getNormal(VEC2 v) const {
    VEC2 min_pos = pos - hsize;
  VEC2 max_pos = pos + hsize;
  VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));
  
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
      if ( p(1) > max_pos(1) + 2*eps) {
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

 return VEC2(normal(0)*t(0) - normal(1)*t(1), normal(1)*t(0) + normal(0)*t(1));
}


FLOAT RBoxObstacle::distance(VEC2 v) const {
  VEC2 min_pos = pos - hsize;
  VEC2 max_pos = pos + hsize;
  VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));
  
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
      if ( p(1) > max_pos(1)+ 2*eps) {
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
      dist =  10*(min_pos(1) - 2*eps - p(1));
 	normal = VEC2(0, -1);
    } else if (p(1) > max_pos(1) + 2*eps) {
      dist =  p(1) - max_pos(1) - 2*eps;
    } else {
      VEC2 dist_to_min, dist_to_max;
      FLOAT min_d = p(0) - min_pos(0) - 2*eps;
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

void RBoxObstacle::getCollisionValues(VEC2 v, FLOAT & dist, VEC2 &normal) const {
 VEC2 min_pos = pos - hsize;
  VEC2 max_pos = pos + hsize;
    VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));

    FLOAT eps = 0.01;
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

  normal =  VEC2(normal(0)*t(0) - normal(1)*t(1), normal(1)*t(0) + normal(0)*t(1));
}

void RBoxObstacle::exportSVG(std::ofstream & file, int id) const {
  FLOAT eps = 0.01;
  FLOAT c = 1000;
  VEC2 nhsize = c*hsize;
  VEC2 npos = c*pos;
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   file<<npos(0) + nhsize(0)*t(0)+nhsize(1)*n(0)<<","<<npos(1) + nhsize(0)*t(1)+nhsize(1)*n(1)<<" ";
   file<<npos(0) + nhsize(0)*t(0)-nhsize(1)*n(0)<<","<<npos(1) + nhsize(0)*t(1)-nhsize(1)*n(1)<<" ";
   file<<npos(0) - nhsize(0)*t(0)-nhsize(1)*n(0)<<","<<npos(1) - nhsize(0)*t(1)-nhsize(1)*n(1)<<" ";
   file<<npos(0) - nhsize(0)*t(0)+nhsize(1)*n(0)<<","<<npos(1) - nhsize(0)*t(1)+nhsize(1)*n(1)<<" ";

   file<<"z\"\n";
   file<<"       id=\"pathOBin<<"<<id<<"\" />\n";
   
   file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   nhsize = c*(hsize + 2*VEC2(eps, eps));
   file<<npos(0) + nhsize(0)*t(0)+nhsize(1)*n(0)<<","<<npos(1) + nhsize(0)*t(1)+nhsize(1)*n(1)<<" ";
   file<<npos(0) + nhsize(0)*t(0)-nhsize(1)*n(0)<<","<<npos(1) + nhsize(0)*t(1)-nhsize(1)*n(1)<<" ";
   file<<npos(0) - nhsize(0)*t(0)-nhsize(1)*n(0)<<","<<npos(1) - nhsize(0)*t(1)-nhsize(1)*n(1)<<" ";
   file<<npos(0) - nhsize(0)*t(0)+nhsize(1)*n(0)<<","<<npos(1) - nhsize(0)*t(1)+nhsize(1)*n(1)<<" ";

   file<<"z\"\n";
   file<<"       id=\"pathOBout"<<id<<"\" />\n";
}
