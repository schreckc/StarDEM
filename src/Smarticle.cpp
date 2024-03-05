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


#include "Smarticle.hpp"
#include "Sphere.hpp"
#include "Grid.hpp"
#include "Simulation.hpp"
#include "error.hpp"
#include "utils.hpp"
#include "dem_conf.hpp"

#include <iostream>
#include <sstream>

std::vector<FLOAT> Smarticle::coefs = std::vector<FLOAT>();
std::vector<FLOAT> Smarticle::rs = std::vector<FLOAT>();
std::vector<FLOAT> Smarticle::rs_dist = std::vector<FLOAT>();
std::vector<FLOAT> Smarticle::drs_dist = std::vector<FLOAT>();
std::vector<FLOAT> Smarticle::rs_display = std::vector<FLOAT>();

VEC2 Smarticle::g_n = VEC2(0, 0);

FLOAT Smarticle::radius = 0;
FLOAT Smarticle::mass = 0;
FLOAT Smarticle::I = 0;
FLOAT Smarticle::area = 0;

int Smarticle::nb_bbcol = 0;
int Smarticle::nb_pcol = 0;
int Smarticle::nb_collisions = 0;

/*** Careful, if you change this, you have to change the analytic computeArea ***/
FLOAT Smarticle::r(FLOAT x) {
  FLOAT res = abs(coefs[0]);
  for (uint i = 1; i<(coefs.size()+1)/2; ++i) {
    res += abs(coefs[2*i-1]) + coefs[2*i-1]*cos(i*x);
    res += abs(coefs[2*i]) + coefs[2*i]*sin(i*x);
  }
  assert(res > 0);
  return res;
}

FLOAT Smarticle::dr(FLOAT x) {
  FLOAT res = 0;
  for (uint i = 1; i<(coefs.size()+1)/2; ++i) {
    res += coefs[2*i]*i*cos(i*x);
    res += -coefs[2*i-1]*i*sin(i*x);
  }
  return res;
}

Smarticle::Smarticle() : Object() {
  pos = VEC2(0, 0);
  vel = VEC2(0, 0);

  is_moving = true;
  color = VEC3(1, 0, 0);
  
  nb_neigh = 0;
  nb_contacts = 0;
  smarticle = true;
}

Smarticle::Smarticle(VEC2 p, FLOAT m, VEC2 v, VEC2 n) : Smarticle() {
  smarticle = true;
  pos = p;
  normal = n;
  vel = v;
  avel = 0;
  G = VEC2(g_n(0)*cos(orientation) - g_n(1)*sin(orientation), g_n(0)*sin(orientation) + g_n(1)*cos(orientation));


  force = VEC2(0, 0);
  torque = 0;

  orientation = 0;
  
  FLOAT h = dem_conf::grid_spacing_;
  cell = VEC2I(floor(p(0)/h), floor(p(1)/h));

  if (normal.squaredNorm() == 0) {
    normal = VEC2(0, 1);
  }

}


void Smarticle::animate() {
  for (auto c: all_contacts) {
    delete c;
  }
  contacts_c.clear();
  all_contacts.clear();
  
  nb_neigh = 0;
  nb_contacts = 0;
  
  VEC2 vel_prev = vel;
  if (mass != 0) {
    vel += force/mass*dem_conf::dt_;
  }
  torque -= dem_conf::damping_t*avel;
  if (I != 0) {
    avel += torque/I*dem_conf::dt_;
  }
  orientation += avel*dem_conf::dt_;
  
  while (orientation >= 2*M_PI) {
    orientation-=2*M_PI;
  }
  while (orientation < 0) {
    orientation+=2*M_PI;
  }
    
  // forces are applied on the gravity center
  G += vel*dem_conf::dt_;
  pos = G + VEC2(-g_n(0)*cos(orientation) + g_n(1)*sin(orientation), -g_n(0)*sin(orientation) - g_n(1)*cos(orientation));   

  if (/*abs(avel) > 20 ||*/ vel.squaredNorm() > 5000) {
    is_moving = true;
  } else {
    is_moving = false;
  }
    
  force = VEC2(0, 0);
  torque = 0;
  
  FLOAT h = dem_conf::grid_spacing_;
  cell = VEC2I(floor(pos(0)/h), floor(pos(1)/h));
}

#ifndef NO_GRAPHICS_ 
void Smarticle::draw() {
  

  glPushMatrix();
  glTranslatef(pos(0), pos(1), 0);

  VEC3 x = radius*VEC3(cos(orientation), sin(orientation), 0);
  VEC2 go = radius*VEC2(g_n(0)*cos(orientation) - g_n(1)*sin(orientation),
  			g_n(0)*sin(orientation) + g_n(1)*cos(orientation));


  int nstep = dem_conf::sm_resolution_display;
  FLOAT a = orientation;
  FLOAT astep = 2*M_PI/(FLOAT)nstep;

  //  if (is_moving) {
  //   color = VEC3(1, 0, 0);
  // } else {
  //   color = VEC3(0, 0, 1);
  // }
  


  if (!dem_conf::wireframe_) {
#ifdef _WHITE
    glColor3f(color(0), color(1), color(2));
#else
    glColor3f((1.0-color(0))*0.5, (1.0-color(1))*0.5, (1.0-color(2))*0.5);
#endif
    for (uint i = 0; i <= nstep; ++i) {
      glBegin(GL_TRIANGLES);
      float rc = radius*rs_display[i];
      float rn = radius*rs_display[(i+1)%nstep];
      glVertex3f(0, 0, 0);
      glVertex3f(rc*cos(a), rc*sin(a), 0);
      glVertex3f(rn*cos(a+astep), rn*sin(a+astep), 0);
      a+=astep;
      glEnd();
    }
    a = orientation;
  }

#ifdef _WHITE
  if (!dem_conf::wireframe_) {
    glLineWidth(2.0f);
    glColor3f(0, 0, 0);
  } else {
    glLineWidth(4.0f); 
    glColor3f(color(0), color(1), color(2));
  }
#else
  glLineWidth(2.0f);    
  glColor3f(1-color(0), 1-color(1), 1-color(2));
#endif
  glBegin(GL_LINES);
  for (uint i = 0; i < nstep; ++i) {
    float rc = radius*rs_display[i];
    float rn = radius*rs_display[(i+1)%nstep];
#ifndef _WHITE
    if (dem_conf::wireframe_ && i > nstep/2) {
      glColor3f(color(0), color(1), color(2));
    }
#endif
    glVertex3f(rc*cos(a), rc*sin(a), 0.1);
    glVertex3f(rn*cos(a+astep), rn*sin(a+astep), 0.1);
    a+=astep;
  }
  glEnd();

  glPopMatrix();


  if (dem_conf::display_bbox) {
    glPushMatrix();
    glTranslatef(pos(0), pos(1), 0);
    glLineWidth(1.0f);
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    glVertex3f(radius, radius, 0);
    glVertex3f(radius, -radius, 0);
    glVertex3f(radius, -radius, 0);
    glVertex3f(-radius, -radius, 0);
    glVertex3f(-radius, -radius, 0);
    glVertex3f(-radius, radius, 0);
    glVertex3f(-radius, radius, 0);
    glVertex3f(radius, radius, 0);
    glEnd();
    glPopMatrix();
  }
  
}

void Smarticle::drawContact() {
  for (auto c: all_contacts) {
    c->draw();
  }
  
  
  if (dem_conf::display_contact > 2) {
    glPushMatrix();
    glTranslatef(pos(0), pos(1), 0);
    for (auto c: contacts_c) {
#ifdef _WHITE
      glColor3f(0, 0, 0.1);
#else
      glColor3f(1, 1, 1);
#endif
      glBegin(GL_LINES);
      float rc = radius;
      glVertex3f(0, 0, 0);
      glVertex3f(rc*cos(c->a1), rc*sin(c->a1), 0.1);
      rc = radius;

#ifdef _WHITE
      glColor3f(0, 0, 1);
#else
      glColor3f(1, 1, 0);
#endif
      glVertex3f(0, 0, 0.1);
      glVertex3f(rc*cos(c->a2), rc*sin(c->a2), 0.1);
      glEnd();
    }
    
    glLineWidth(2.0f);
#ifdef _WHITE
    glColor3f(0.2, 0.2, 0.2);
#else
    glColor3f(0.8, 0.8, 0.8);
#endif
    int nstep = dem_conf::sm_resolution_display;
    FLOAT a = 0;
    FLOAT astep = 2*M_PI/(FLOAT)nstep;

    glBegin(GL_LINES);
    for (uint i = 0; i < nstep; ++i) {
      glVertex3f(radius*cos(a), radius*sin(a), 0.1);
      a+=astep;
      glVertex3f(radius*cos(a), radius*sin(a), 0.1);
    }
    glEnd();
    glPopMatrix();
  }
}
#endif

VEC2 Smarticle::getPosition() const {
  return pos;
}

void Smarticle::setPosition(VEC2 p) {
  pos = p;
  FLOAT h = dem_conf::grid_spacing_;
  cell = VEC2I(floor(p(0)/h), floor(p(1)/h));
}

VEC2 Smarticle::getVelocity() const {
  return vel;
}

void Smarticle::setVelocity(VEC2 velo) {
  vel = velo;
}

FLOAT Smarticle::getRotVelocity() const {
  return avel;
}

void Smarticle::setRotVelocity(FLOAT av) {
  avel = av;
}

bool Smarticle::isMoving() const {
  return is_moving;
}

void Smarticle::setCoefs(std::vector<FLOAT> cs) {
  coefs = cs;
}

const std::vector<FLOAT> & Smarticle::getCoefs() {
  return coefs;
}


// precompute radius function normalized (max radius = 1)
void Smarticle::computeTables() {
  int nstep = dem_conf::sm_resolution_dist;
  FLOAT astep = 2*M_PI/nstep;
  
  rs_dist = std::vector<FLOAT>(nstep);
  drs_dist = std::vector<FLOAT>(nstep);

  FLOAT max = 0;
  for (uint i = 0; i < nstep; ++i) {
    rs_dist[i] = r(i*astep);
    if (rs_dist[i] > max) {
      max = rs_dist[i];
    }
    drs_dist[i] = dr(i*astep);
  }
  TR(max);
  assert(max >0);

  for (uint i = 0; i < nstep; ++i) {
    rs_dist[i] /= max;
    drs_dist[i] /= max;
  }
      
  nstep = dem_conf::sm_resolution;
  astep = 2*M_PI/nstep;
    
  rs = std::vector<FLOAT>(nstep);
    
  for (uint i = 0; i < nstep; ++i) {
    rs[i] = r(i*astep)/max;
  }
  
  nstep = dem_conf::sm_resolution_display;
  astep = 2*M_PI/nstep;
    
  rs_display = std::vector<FLOAT>(nstep);
    
  for (uint i = 0; i < nstep; ++i) {
    rs_display[i] = r(i*astep)/max;
  }
  for (uint i = 0; i < coefs.size(); ++i) {
    coefs[i] /= max;
  }

  /* export smarticle shape as svg */
  // std::stringstream ss;
  // ss << "optim_stc/s";
  // for (uint i = 0; i < coefs.size(); ++i) {
  //   ss<<coefs[i]<<"_";
  // }
  // ss<<".svg";
  // std::string str = ss.str();
  // std::ofstream file;
  // file.open(str);
  // exportSVG(file);
}

bool Smarticle::in(FLOAT x, FLOAT y) {
  FLOAT theta = atan(y/x);
    
  if (x < 0) {
    theta += M_PI;
  }
  if (x == 0) {
    theta = y/abs(y)*M_PI/2.0;
  }

  while (theta < 0) {
    theta += 2*M_PI;
  }
  
  FLOAT d2 = x*x + y*y;
  
  FLOAT astep = 2*M_PI/dem_conf::sm_resolution_dist;
  int ind = floor(theta/astep);
  int ind2 = ind+1;
  FLOAT c = theta/astep - ind;

  ind = ind%dem_conf::sm_resolution_dist;
  ind2 = ind2%dem_conf::sm_resolution_dist;
      
  if (ind < 0) {
    ind += dem_conf::sm_resolution_dist;
  }
  if (ind2 < 0) {
    ind2 += dem_conf::sm_resolution_dist;
  }

  FLOAT rr = (1-c)*rs_dist[ind] + c*rs_dist[ind2];
  assert( c>= 0 && c <= 1);
  
  return d2 - rr*rr < 0;
}


// compute also gravity center and inertia
void Smarticle::computeNumericalArea() {
  FLOAT a = 0;
  FLOAT gx = 0;
  FLOAT gy = 0;
  FLOAT h = 0.01;
  FLOAT i = 0;
  for (float x = -10; x <= 10; x+=h) {
    for (float y = -10; y <= 10; y+=h) {
      if (in(x, y)) {
	++a;
	gx += x;
	gy += y;
      }
    }
  }
   
  gx/=a;
  gy/=a;
  g_n = VEC2(gx, gy);
   
  for (float x = -1; x <= 1; x+=h) {
    for (float y = -1; y <= 1; y+=h) {
      if (in(x, y)) {
	i += (VEC2(x, y) - g_n).squaredNorm();
      }
    }
  }
  i /= a;
  area = radius*radius*h*h*a;
  g_n *= radius;

  //  mass = 0.003*1.18*1e3*area; //m*kg/m^3*m^2
  mass = 3*1.18*1e-3*area; //mm*g/mm^3*mm^2
   
  I = mass*radius*radius*i;
}

// if area is fixed, adjust the radius
// if radius is fixed adjust area
float Smarticle::computeArea() {
  FLOAT a = 0;

  for (uint i = 0; i < coefs.size(); ++i) {
    a += abs(coefs[i]);
  }
  a = a*a;
  for (uint i = 1; i < coefs.size(); ++i) {
    a += 0.5*coefs[i]*coefs[i];
  }
  if (radius != 0) { //radius fixed
    a *= radius*radius*M_PI;
  } else { // volume fixed
    a *= M_PI;
    radius = sqrt(area/a);
  }
  return a;
}
 


FLOAT Smarticle::getMass() {
  return mass;
}

void Smarticle::setMass(FLOAT m) {
  mass = m;
}

FLOAT Smarticle::getRadius() {
  return radius;
}

void Smarticle::setRadius(FLOAT r) {
  radius = r;
}

FLOAT Smarticle::getArea() {
  return area;
}

void Smarticle::setArea(FLOAT a) {
  area = a;
}



VEC2I Smarticle::getCell() const {
  return cell;
}

uint Smarticle::getNbContact() const {
  return nb_contacts;
}

uint Smarticle::getNbNeighbors() const {
  return nb_neigh;
}

void Smarticle::addForce(VEC2 f) {
  force += f;
}

void Smarticle::gravity() {
  force += mass*dem_conf::gravity_;
}


FLOAT Smarticle::distance(VEC2 v) {
  FLOAT x = v(0);
  FLOAT y = v(1);
  return distance(x, y);
}


// distance using directly r and dr function (other fucntions use lookup table)
FLOAT Smarticle::distance(FLOAT x, FLOAT y) {
  FLOAT theta = atan(y/x);
  FLOAT d2 = x*x + y*y;
  FLOAT d = sqrt(d2);

  if (x < 0) {
    theta += M_PI;
  }
  if (x == 0) {
    theta = y/abs(y)*M_PI/2.0;
  }

  while (theta < 0) {
    theta += 2*M_PI;
  }

  FLOAT f = r(theta) - d;
  
  FLOAT dfdx = -y*dr(theta)/d2 - x/d;
  FLOAT dfdy = x*dr(theta)/d2 - y/d;
  VEC2 grad = VEC2(dfdx, dfdy);
  FLOAT ngrad = sqrt(dfdx*dfdx + dfdy*dfdy);

  return -f/ngrad;
}


FLOAT Smarticle::radialDistance(FLOAT x, FLOAT y) {
  FLOAT theta = atan(y/x);
  FLOAT d2 = x*x + y*y;
  FLOAT d = sqrt(d2);

  if (x < 0) {
    theta += M_PI;
  }
  if (x == 0) {
    theta = y/abs(y)*M_PI/2.0;
  }

  while (theta < 0) {
    theta += 2*M_PI;
  }


  FLOAT f = r(theta) - d;
  return -f;
}


FLOAT Smarticle::numericalDistance(FLOAT x, FLOAT y) {
  FLOAT d_min = 1;
  int nstep = dem_conf::sm_resolution_display;
  FLOAT astep = 2*M_PI/(FLOAT)nstep;
  for (uint i = 0; i < nstep; ++i) {
    FLOAT rc = rs_display[i];
    FLOAT rn = rs_display[(i+1)%nstep];
    VEC2 o = VEC2(rc*cos(i*astep), rc*sin(i*astep));
    VEC2 dir = VEC2(rn*cos((i+1)*astep), rn*sin((i+1)*astep)) - o;
    FLOAT d = utils::distancePointSegment(VEC2(x, y), o, dir);
    if (d < d_min) {
      d_min = d;
    }
  }
  return d_min;
}


bool Smarticle::getCollisionValues(VEC2 v, FLOAT & dist, VEC2 &normal) {
  FLOAT x = v(0) - pos(0);
  FLOAT y = v(1) - pos(1);
  FLOAT theta = atan(y/x);
  
  if (x < 0) {
    theta += M_PI;
  }
  if (x == 0) {
    theta = y/abs(y)*M_PI/2.0;
  }

  theta -= orientation;
  while (theta < 0) {
    theta += 2*M_PI;
  }
  
  FLOAT d2 = x*x + y*y;
  FLOAT d = sqrt(d2);

  FLOAT dist2;
  VEC2 normal2;
  
  FLOAT astep = 2*M_PI/dem_conf::sm_resolution_dist;
  int ind = floor(theta/astep);

  ind = ind%dem_conf::sm_resolution_dist;
   
  if (ind < 0) {
    ind += dem_conf::sm_resolution_dist;
  }

  FLOAT rr = rs_dist[ind];
 
   
  FLOAT f = d - radius*rr;
  if (f >= 0) {
    return false;
  }
  FLOAT drr = drs_dist[ind];
  
  FLOAT dfdx = x/d + y*radius*drr/d2;
  FLOAT dfdy = y/d - x*radius*drr/d2;
  VEC2 grad = VEC2(dfdx, dfdy);
  FLOAT ngrad = sqrt(dfdx*dfdx + dfdy*dfdy);
  normal = grad.normalized();
#ifndef _USE_RADIAL_DIST
  dist = f/ngrad;
#else
  dist = f;
#endif
  return true;
}

bool Smarticle::isIn(VEC2 v) const {
  FLOAT x = v(0) - pos(0);
  FLOAT y = v(1) - pos(1);
  FLOAT theta = atan(y/x);

  if (x < 0) {
    theta += M_PI;
  }
  if (x == 0) {
    theta = y/abs(y)*M_PI/2.0;
  }

  theta -= orientation;
  while (theta < 0) {
    theta += 2*M_PI;
  }
  
  FLOAT d2 = x*x + y*y;
  FLOAT d = sqrt(d2);
  
  FLOAT astep = 2*M_PI/dem_conf::sm_resolution_dist;
  int ind = floor(theta/astep);
  int ind2 = ind+1;
  FLOAT c = theta/astep - ind;

  ind = ind%dem_conf::sm_resolution_dist;
  ind2 = ind2%dem_conf::sm_resolution_dist;
   
   
  if (ind < 0) {
    ind += dem_conf::sm_resolution_dist;
  }
  if (ind2 < 0) {
    ind2 += dem_conf::sm_resolution_dist;
  }

  FLOAT rr = (1-c)*rs_dist[ind] + c*rs_dist[ind2];
  assert( c>= 0 && c <= 1);
  return d - radius*rr < 0;
}

void Smarticle::collision(Obstacle *o) {
  contacts_obs.clear();

  VEC2 n;
  FLOAT dist = o->distance(pos);
  if (dist > radius) {
    return;
  }
  
  int nstep = dem_conf::sm_resolution;
  FLOAT a = orientation;
  FLOAT astep = 2*M_PI/nstep;

  Contact *c = new  Contact(VEC2(0, 0), VEC2(0,0), 1);
  c->setColor(color);
  c->p1 = this;
  c->p2 = o;
  bool cb = false;
  bool ce = false;
  
  for (uint i = 0; i < nstep; ++i) {
    FLOAT rc = radius*rs[i];
    VEC2 ra = VEC2(rc*cos(a), rc*sin(a));
    a+=astep;
    VEC2 v = ra + pos;
    o->getCollisionValues(v, dist, n);
    if (dist >= 0 && c->dist < 0) {
      contacts_obs.push_back(c);
      all_contacts.push_back(c);
      c = new Contact(VEC2(0, 0), VEC2(0,0), 1e30);
      c->setColor(color);
      c->p1 = this;
      c->p2 = o;
    }
    if (dist < 0) {
      if (i==0) {
	cb = true;
      } if (i == nstep-1) {
	ce = true;
      }
      if (dist < c->dist) {
	c->dist = dist;
	c->pt = v;
	c->normal = n;
      }
    }
  }
  if (c->dist < 0) {
    contacts_obs.push_back(c);
    all_contacts.push_back(c);
  } else {
    delete c;
  }
  
  if (cb && ce) {
    contacts_obs.front()->merge(contacts_obs.back());
    delete contacts_obs.back();
    contacts_obs.pop_back();
    all_contacts.pop_back();
  }
}

void Smarticle::resetForce() {
  force = VEC2(0, 0);
}

void Smarticle::setColor(FLOAT r, FLOAT g, FLOAT b) {
  color = VEC3(r, g, b);
}


void Smarticle::rotate(FLOAT rot) {
  orientation += rot-M_PI/8.0;
  G = pos + VEC2(g_n(0)*cos(orientation) - g_n(1)*sin(orientation), g_n(0)*sin(orientation) + g_n(1)*cos(orientation));
}

FLOAT Smarticle::getRotation() const {
  return orientation;
}

void Smarticle::exportSVG(std::ofstream &file) {
  file<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
  file<<"<svg\n";
  file<<"   width=\"210mm\"\n";
  file<<"   height=\"297mm\"\n";
  file<<"   viewBox=\"0 0 210 297\"\n";
  file<<"   version=\"1.1\"\n";
  file<<"   id=\"svg5\">\n";
  file<<"  <defs\n";
  file<<"     id=\"defs2\" />\n";
  file<<"  <g\n";
  file<<"      id=\"layer1\">\n";
  file<<"    <path\n";
  file<<"       style=\"opacity:1;fill:none;stroke:#000000;stroke-width:1;stroke-linecap:square\"\n";

  file<<"       d=\"M ";
  
  int nstep = dem_conf::sm_resolution;
  FLOAT a = 0;
  FLOAT astep = 2*M_PI/nstep;
  for (uint i = 0; i < nstep; ++i) {
    FLOAT rc = radius*rs[i];
    FLOAT a = i*astep;
    file<<rc*cos(a)<<","<<rc*sin(a)<<" ";
  }
  file<<"z\"\n";
  file<<"       id=\"pathSM\" />\n";
  file<<"  </g>\n";
  file<<"</svg>";

}

void Smarticle::plotDistance(std::ofstream &file, int type) const {
  FLOAT d;
  FLOAT e_max = 0;
  FLOAT h = 0.0025;
  for (FLOAT x = -1.1; x <= 1.1; x+=h) {
    for (FLOAT y = -1.1; y <= 1.1; y+=h) {
      FLOAT r = radialDistance(x, y);
      d = 0;
      if (r < 0) {
	if (type == 0) { // first order
	  d = abs(distance(x, y)); 
	} else if (type == 1) { //radial
	  d = abs(radialDistance(x, y));
	} else if (type == 2) { // numerical
	  d = abs(numericalDistance(x, y));
	} else if (type == 3) { // error first order vs numerical
	  FLOAT n  = numericalDistance(x, y);
	  if (n > 0.001) {
	    d = abs(abs(distance(x, y)) - n)/n;
	    if (n < 0.1&& e_max < d) {
	      e_max = d;
	    }
	  }
	} else if (type == 4) {// error radial vs numerical
	  FLOAT n  = numericalDistance(x, y);
	  if (n> 0.001) {
	    d = abs(abs(radialDistance(x, y)) - n)/n;
	    if (n < 0.1 && e_max < d) {
	      e_max = d;
	    }
	  }
	}
	// if (type < 3 && d > 0.2) {
	//   d = 0.2;
	// }
      }
      file << x << " " << y << " " <<d <<std::endl;
    }
    file<<"\n";
  }

}

void collision(Smarticle *p1, Smarticle *p2) {

  std::list<Contact*> & contacts = p1->contacts;
  contacts.clear();
    
  VEC2 d = p2->pos-p1->pos;
  FLOAT l = d.norm();
  
  if (l > p1->radius+p2->radius) {
    return;
  }

  ++Smarticle::nb_bbcol;
  
  FLOAT theta = atan(d(1)/d(0));
  if (d(0) < 0) {
    theta += M_PI;
  }
  if (d(0) == 0) {
    theta = d(1)/abs(d(1))*M_PI/2.0;
  }

  
  int nstep = dem_conf::sm_resolution;
  FLOAT astep = 2*M_PI/nstep;
  FLOAT cosa = (p1->radius*p1->radius + l*l - p2->radius*p2->radius)/(2*l*p1->radius);
  FLOAT alpha1 = -acos(cosa) + theta;
  FLOAT alpha2 = acos(cosa) + theta;

  int i = floor((alpha1-p1->orientation)/astep);
  int i2 = ceil((alpha2-p1->orientation)/astep);
  uint nb_col = 0;

  Contact *c = new Contact(VEC2(0, 0), VEC2(0,0), 1e30);
  c->a1 = alpha1;
  c->a2 = alpha2;
  c->p1 = p1;
  c->p2 = p2;
  p1->contacts_c.push_back(c);
  

  bool cb = false;
  bool ce = false;

  assert(i <= i2);
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
    FLOAT rc = p1->radius*Smarticle::rs[ind];
    VEC2 ra = VEC2(rc*cos(a+p1->orientation), rc*sin(a+p1->orientation));
    VEC2 v = ra + p1->pos;
    bool coll = p2->getCollisionValues(v, dist, n);
    if (!coll && c->dist < p2->radius) {
      c->setColor(p1->color);
      c->I2 = v;
      contacts.push_back(c);
      p1->all_contacts.push_back(c);
      c = new Contact(VEC2(0, 0), VEC2(0,0), 1e30);
      c->p1 = p1;
      c->p2 = p2;
    }
    if (coll) {
      if (c->dist > 0) {
       	c->I1 = v;
      }
      if (i==0) {
	cb = true;
      } if (i == nstep) {
	ce = true;
      }
      if (dist < c->dist) {
	c->dist = dist;
	c->pt = v;
	c->normal = n;
      }
    }
  }

  if (c->dist < 0) {
    contacts.push_back(c);
    p1->all_contacts.push_back(c);
  } else {
    delete c;
  }

  if (cb && ce) {
    contacts.front()->merge(contacts.back());
    delete contacts.back();
    contacts.pop_back();
    p1->all_contacts.pop_back();
  }

  if (!contacts.empty()) {
    p1->nb_neigh++;
    p2->nb_neigh++;
    ++Smarticle::nb_pcol;
    Smarticle::nb_collisions += contacts.size();
  }

  for (auto c: contacts) {
    c->findP2Influence();
  }
}


void Smarticle::computeCollision() {
  std::list<Contact*>::iterator it = all_contacts.begin();
  for (; it != all_contacts.end(); ++it) {
    (*it)->computeForce();
  }
}


void Smarticle::addForceCollision() {
  std::list<Contact*>::iterator it = all_contacts.begin();
  for (; it != all_contacts.end(); ++it) {
    (*it)->addForce();
  }
}
