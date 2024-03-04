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

#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <list>
#include "Object.hpp"
#include <fstream>
#include "error.hpp"
#include "dem_conf.hpp"

struct Motion {
  VEC2 translation;
  FLOAT scale;
  FLOAT rotation;
  VEC2 center;
  FLOAT begin_time, end_time;
  bool is_running;
  
  Motion(VEC2 trans = VEC2(0, 0), FLOAT sc = 1, FLOAT rot = 0, VEC2 c = VEC2(0, 0)) :
    translation(trans), scale(sc), rotation(rot), begin_time(0), end_time(0), is_running(false), center(c) {}
  Motion(FLOAT rot) : translation(VEC2(0, 0)), scale(1), rotation(rot), begin_time(0), end_time(0), center(VEC2(0, 0)) {}
  bool isRunning(FLOAT time) {return is_running && time >= begin_time && (end_time == 0 || time < end_time);}
};    


class Obstacle : public Object {

private :
  FLOAT friction;
  std::list<Motion> motions;

protected:
  FLOAT rotation;
  FLOAT mass;
  FLOAT I;
  /** WARNING dynamic object are still under development, only kinda working right now.*/
  bool dynamic;
  VEC3 color;
  
public :
  Obstacle();
  void animate();
  virtual void animate(FLOAT t);
  virtual void apply(Motion m) = 0;
  
#ifndef NO_GRAPHICS_ 
  virtual void draw() = 0;
#endif
  
  virtual FLOAT distance(VEC2 v) const = 0;
  virtual VEC2 getNormal(VEC2 v) const = 0;
  virtual void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const = 0;

  FLOAT getFriction() const;
  void setFriction(FLOAT f);

  void setMotions(std::list<Motion> lm);
  void addMotion(Motion m);
  bool isMoving() const;

  void addForce(VEC2 f);
  VEC2 getForce() const;

  FLOAT getMass();

  void startStop();
  void setDynamic(FLOAT m, FLOAT i);

  void setColor(VEC3 c);

  virtual void exportSVG(std::ofstream & file, int id = 0) const = 0;
};

#endif
