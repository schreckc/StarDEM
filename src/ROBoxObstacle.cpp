#include "ROBoxObstacle.hpp"

ROBoxObstacle::ROBoxObstacle(VEC2 po, VEC2 no, VEC2 si) : Obstacle(), pos(po), n_init(no), hsize(si) {
  
  n_init.normalize();
  t_init = VEC2(n_init(1), -n_init(0));

  t = VEC2(t_init(0)*cos(rotation)-t_init(1)*sin(rotation),
	   t_init(1)*cos(rotation)+t_init(0)*sin(rotation));
  n = VEC2(n_init(0)*cos(rotation)-n_init(1)*sin(rotation),
	   n_init(1)*cos(rotation)+n_init(0)*sin(rotation));
  
  G = pos;
}

ROBoxObstacle::~ROBoxObstacle() {}

void ROBoxObstacle::animate(FLOAT time) {
  Obstacle::animate(time);
  t = VEC2(t_init(0)*cos(rotation)-t_init(1)*sin(rotation),
	   t_init(1)*cos(rotation)+t_init(0)*sin(rotation));
  n = VEC2(n_init(0)*cos(rotation)-n_init(1)*sin(rotation),
	   n_init(1)*cos(rotation)+n_init(0)*sin(rotation));
}

void ROBoxObstacle::apply(Motion m) {
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
void ROBoxObstacle::draw() {
  FLOAT eps = 0.01;
  glColor3f(color(0), color(1), color(2));
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


  glEnd();

  if (!dem_conf::wireframe_) {
    glColor3f(abs(color(0)-0.5), abs(color(1) - 0.5), abs(color(2)-0.5));
    glBegin(GL_TRIANGLES);
    glVertex3f(hsize(0)*t(0)+hsize(1)*n(0), hsize(0)*t(1)+hsize(1)*n(1), 0);
    glVertex3f(hsize(0)*t(0)-hsize(1)*n(0), hsize(0)*t(1)-hsize(1)*n(1), 0);
    glVertex3f(-hsize(0)*t(0)-hsize(1)*n(0), -hsize(0)*t(1)-hsize(1)*n(1), 0);

    glVertex3f(-hsize(0)*t(0)-hsize(1)*n(0), -hsize(0)*t(1)-hsize(1)*n(1), 0);
    glVertex3f(hsize(0)*t(0)+hsize(1)*n(0), hsize(0)*t(1)+hsize(1)*n(1), 0);
    glVertex3f(-hsize(0)*t(0)+hsize(1)*n(0), -hsize(0)*t(1)+hsize(1)*n(1), 0);
    glEnd();
  }

  glPopMatrix();
}
#endif

VEC2 ROBoxObstacle::getNormal(VEC2 v) const {
    VEC2 min_pos = pos - hsize;
  VEC2 max_pos = pos + hsize;
  VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));
  
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


  
 return VEC2(normal(0)*t(0) - normal(1)*t(1), normal(1)*t(0) + normal(0)*t(1));
}


FLOAT ROBoxObstacle::distance(VEC2 v) const {
  VEC2 min_pos = pos - hsize;
  VEC2 max_pos = pos + hsize;
  VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));

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
    //TR("IN "<<dist<<" "<<p(0));
  }


    return dist;
  
  }

void ROBoxObstacle::getCollisionValues(VEC2 v, FLOAT & dist, VEC2 &normal) const {
 VEC2 min_pos = pos - hsize;
 VEC2 max_pos = pos + hsize;
 VEC2 p = pos + VEC2((v(0)-pos(0))*t(0) + (v(1)-pos(1))*t(1), (v(1)-pos(1))*t(0) - (v(0)-pos(0))*t(1));

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
    //    TR("IN "<<p(0)<<" "<<p(1)<<" "<<min_d);
    normal(i_min) = (FLOAT)dir*1.0;
    normal((i_min+1)%2) = 0.0;
    dist = -min_d;
    
  }

  normal =  VEC2(normal(0)*t(0) - normal(1)*t(1), normal(1)*t(0) + normal(0)*t(1));
}


void ROBoxObstacle::exportSVG(std::ofstream & file, int id) const {
  FLOAT c = 1000;
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

   file<<"       d=\"M ";
   file<<c*pos(0)+c*hsize(0)*t(0)+c*hsize(1)*n(0)<<","<<c*pos(1)+c*hsize(0)*t(1)+c*hsize(1)*n(1)<<" ";
   file<<c*pos(0)+c*hsize(0)*t(0)-c*hsize(1)*n(0)<<","<<c*pos(1)+c*hsize(0)*t(1)-c*hsize(1)*n(1)<<" ";
   file<<c*pos(0)-c*hsize(0)*t(0)-c*hsize(1)*n(0)<<","<<c*pos(1)-c*hsize(0)*t(1)-c*hsize(1)*n(1)<<" ";
   file<<c*pos(0)-c*hsize(0)*t(0)+c*hsize(1)*n(0)<<","<<c*pos(1)-c*hsize(0)*t(1)+c*hsize(1)*n(1)<<" ";

   file<<"z\"\n";
   file<<"       id=\"pathOB"<<id<<"\" />\n";
}
