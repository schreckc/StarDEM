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

#include "Contact.hpp"

#include "Sphere.hpp"
#include "error.hpp"
#include "dem_conf.hpp"
#include "Smarticle.hpp"


Contact::Contact(): col(VEC3(1, 1, 1)), force(VEC2(0, 0)), I1(VEC2(0, 0)), I2(VEC2(0, 0))  {}


Contact::Contact(VEC2 v, VEC2 n, FLOAT d): Contact() {
  pt = v;
  normal = n;
  dist = d;
}

void Contact::setColor(VEC3 c) {
  col = c;
}

void Contact::setColor(FLOAT r, FLOAT g, FLOAT b) {
  col = VEC3(r, g, b);
}

void Contact::merge(Contact *c) {
  if (c->dist < dist) {
    dist = c->dist;
    normal = c->normal;
    pt = c->pt;
  }
  
}

VEC2 Contact::computeForce() {
  VEC2 ra = pt - p1->G;
  VEC2 r2 = pt;
  if (p2 != NULL) {
    r2 -= p2->G;
  }

  VEC2 v1 = p1->vel + p1->avel*VEC2(-ra(1), ra(0));
  VEC2 v2 = VEC2(0, 0);
  if (p2 != NULL) {
    v2 = p2->vel + p2->avel*VEC2(-r2(1), r2(0));
  }
  du =  v1 - v2;

  
  //contact force
  FLOAT du_n = normal.dot(du);
  
  VEC2 F = VEC2(0, 0);
  FLOAT fn = -dem_conf::kn_*dist;
  fn -= dem_conf::damping_*du_n;
  if (fn < 0) {
    fn = 0;
  }
  F += fn*normal;
  
  //    friction
  VEC2 dun = du_n*normal;
  VEC2 dut = du - dun;
  FLOAT du_t = dut.norm();
  if (du_t > 0) {
    VEC2 t = dut/du_t;
    FLOAT ft = dem_conf::ks_*du_t*dem_conf::dt_;
    if (ft < 0) {
      ft = 0;
    }
	
    if (ft > dem_conf::friction_coef_*fn) {
      ft = dem_conf::friction_coef_*fn;
    }
    F -= ft*t;
    
  }
  force = F;
  torque1 = ra(0)*F(1) - ra(1)*F(0);
  if (p2 != NULL) {
    torque2 = -r2(0)*F(1) + r2(1)*F(0);
  }
  return F;
}

void Contact::addForce() {
  p1->force += force;
  p1->torque += torque1;
  ++p1->nb_contacts;

  if (p2 != NULL) {
    p2->force -= force;
    p2->torque += torque2;
    ++p2->nb_contacts;
  }
}

void Contact::findP2Influence() {
  if (p2->smarticle && p1->smarticle) {
    Smarticle *sm = (Smarticle *)p2;
    Smarticle *sm1 = (Smarticle *)p1;
    VEC2 d1 = I1-sm->pos;
    FLOAT theta1 = atan(d1(1)/d1(0));
    if (d1(0) < 0) {
      theta1 += M_PI;
    }
    if (d1(0) == 0) {
      theta1 = d1(1)/abs(d1(1))*M_PI/2.0;
    }
    VEC2 d2 = I2-sm->pos;
    FLOAT theta2 = atan(d2(1)/d2(0));
    if (d2(0) < 0) {
      theta2 += M_PI;
    }
    if (d2(0) == 0) {
      theta2 = d2(1)/abs(d2(1))*M_PI/2.0;
    }

    while (theta1 >= 2*M_PI) {
      theta1-=2*M_PI;
    }
    while (theta1 < 0) {
      theta1+=2*M_PI;
    }
      
    while (theta2 >= 2*M_PI) {
      theta2-=2*M_PI;
    }
    while (theta2 < 0) {
      theta2+=2*M_PI;
    }
      
    int nstep = dem_conf::sm_resolution;
    FLOAT astep = 2*M_PI/nstep;
    int i = floor((theta2-sm->orientation)/astep);
    int i2 = ceil((theta1-sm->orientation)/astep);
    if (i < 0) {
      i += nstep;
    }
    if (i2 < 0) {
      i2 += nstep;
    }

    while (i <= i2) {
      VEC2 n;
      FLOAT dist;
      int ind = i%nstep;
      if (ind < 0) {
	ind += nstep;
      }
      assert(ind >= 0 && ind < dem_conf::sm_resolution);
      ++i;
	
      FLOAT a = ind*astep;
      FLOAT rc = sm->radius*Smarticle::rs[ind];
      VEC2 ra = VEC2(rc*cos(a+sm->orientation), rc*sin(a+sm->orientation));
      VEC2 v = ra + sm->pos;
      FLOAT dist_cur = 0;
      bool coll = sm1->getCollisionValues(v, dist_cur, n);
      if (coll && dist_cur < dist) {
	col = VEC3(1, 1,0);
	dist = dist_cur;
	pt = v;
	normal = -n;
      }
    }
  }
  
  
}


void Contact::draw() {
  VEC2 c1 = p1->G;
  VEC2 c2;
  if (p2 != NULL && (p2->G - pt).norm() < 1.1*Smarticle::getRadius()) {
    c2 = p2->G;
  } else {
    c2 = pt;
  }
  FLOAT coef = 0.0005;
  if (dem_conf::display_contact == 2) {
    Sphere s(0.2);
#ifdef _WHITE
    s.setColor(1, 0, 0);
#else
    s.setColor(col(0), col(1), col(2));
#endif

    glLineWidth(5.0f);
    glPushMatrix();
    glTranslatef(pt(0), pt(1), 0); 
    s.draw();
    glBegin(GL_LINES);
#ifdef _WHITE
    glColor3f(0, 0, 0);
#else 
    glColor3f(col(0), col(1), col(2));
#endif
    glVertex3f(0, 0, 0.1);
    glVertex3f(2*normal(0), 2*normal(1), 0.1);
    glEnd();
    glPopMatrix();
  }

  
  
  if (dem_conf::display_contact == 1) {
    FLOAT af = force(0)*force(0) + force(1)*force(1);  
    FLOAT eps = dem_conf::eps_c_display*pow(af, 0.3);
    VEC2 d1 = VEC2(-c1(1) + pt(1), c1(0) - pt(0));
    d1.normalize();
    VEC2 d2 = VEC2(-c2(1) + pt(1), c2(0) - pt(0));
    d2.normalize();

#ifdef _WHITE
    glColor3f(0.8, 0.8, 0.8);
#else
    glColor3f(1, 1, 1);
#endif
    glBegin(GL_TRIANGLES);
    glVertex3f(pt(0)+eps*d1(0), pt(1)+eps*d1(1), 0.11);
    glVertex3f(c1(0)+eps*d1(0), c1(1)+eps*d1(1), 0.11);
    glVertex3f(pt(0)-eps*d1(0), pt(1)-eps*d1(1), 0.11);
  
    glVertex3f(pt(0)-eps*d1(0), pt(1)-eps*d1(1), 0.11);
    glVertex3f(c1(0)+eps*d1(0), c1(1)+eps*d1(1), 0.11);
    glVertex3f(c1(0)-eps*d1(0), c1(1)-eps*d1(1), 0.11);

    glVertex3f(pt(0)+eps*d2(0), pt(1)+eps*d2(1), 0.11);
    glVertex3f(c2(0)+eps*d2(0), c2(1)+eps*d2(1), 0.11);
    glVertex3f(pt(0)-eps*d2(0), pt(1)-eps*d2(1), 0.11);
  
    glVertex3f(pt(0)-eps*d2(0), pt(1)-eps*d2(1), 0.11);
    glVertex3f(c2(0)+eps*d2(0), c2(1)+eps*d2(1), 0.11);
    glVertex3f(c2(0)-eps*d2(0), c2(1)-eps*d2(1), 0.11);

    glEnd();
  
  }
}
