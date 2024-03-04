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


#ifndef SMARTICLE_HPP
#define SMARTICLE_HPP

#include "Object.hpp"
#include "Obstacle.hpp"
#include "Contact.hpp"
#include <vector>

class Smarticle : public Object {

    
private :
  static std::vector<FLOAT> coefs;
  
  static std::vector<FLOAT> rs;
  static std::vector<FLOAT> rs_dist;
  static std::vector<FLOAT> rs_display;
  static std::vector<FLOAT> drs_dist;

  
  static FLOAT r(FLOAT x);
  static FLOAT dr(FLOAT x);

  VEC2 pos;
  FLOAT orientation;
  static VEC2 g_n; // G - pos
    
  VEC2I cell; //position in the grid

  static FLOAT radius;
  static FLOAT mass;
  static FLOAT I;
  static FLOAT area;

  // !! not used yet (static ??)!!
  static FLOAT ks;
  static FLOAT kn;
  static FLOAT damping_s;
  static FLOAT damping_n;
  static FLOAT friction;

  
  VEC3 color;

  VEC2 normal;

  bool is_moving;

  std::list<Contact*> contacts_c;
  std::list<Contact*> contacts;
  std::list<Contact*> contacts_obs;
  std::list<Contact*> all_contacts;
  
public:
  static int nb_bbcol;
  static int nb_pcol;
  static int nb_collisions;

  
  Smarticle();
  Smarticle(VEC2 p, FLOAT m=0, VEC2 v=VEC2(0, 0), VEC2 n=VEC2(0, 0));
    
  void animate();

  #ifndef NO_GRAPHICS_ 
  void draw();
  void drawContact();
  #endif
  
  VEC2 getPosition() const;
  void setPosition(VEC2 p);
  VEC2 getVelocity() const;
  void setVelocity(VEC2 velo);
  FLOAT getRotVelocity() const;
  void setRotVelocity(FLOAT av);
  bool isMoving() const;

  static void setCoefs(std::vector<FLOAT> cs);
  static const std::vector<FLOAT> & getCoefs();
  static void computeTables();
  static bool in(FLOAT x, FLOAT y);
  static void computeNumericalArea();
  static float computeArea();
  
  
  static FLOAT getMass();// const;
  static void setMass(FLOAT m);
  static FLOAT getRadius();// const;
  static void setRadius(FLOAT m);
  static FLOAT getArea();// const;
  static void setArea(FLOAT a);
  VEC2I getCell() const;
  uint getNbContact() const;
  uint getNbNeighbors() const;
  void addForce(VEC2 f);
  void gravity();

  
  static FLOAT distance(VEC2 v);
  static FLOAT distance(FLOAT x, FLOAT y);
  static FLOAT radialDistance(FLOAT x, FLOAT y);
  static FLOAT numericalDistance(FLOAT x, FLOAT y);
  bool getCollisionValues(VEC2 v, FLOAT & dist, VEC2 &normal);
  bool isIn(VEC2 v) const;
  void collision(Smarticle *p);
  void collision(Obstacle *p);
  void resetForce();
  
  void setColor(FLOAT r, FLOAT g, FLOAT b);

  void rotate(FLOAT rot);
  FLOAT getRotation() const;

  void exportMitsuba(std::ofstream &file) const;
  static void exportSVG(std::ofstream &file);
  void plotDistance(std::ofstream &file, int type = 0) const;
  
  friend void collision(Smarticle *p1, Smarticle *p2);

  void computeCollision();
  void addForceCollision();

  friend class Contact;
};





#endif
