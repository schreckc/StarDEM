#ifndef RBOX_OBSTACLE_HPP
#define RBOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class RBoxObstacle : public Obstacle {
private :
  VEC2 pos;
  VEC2 n;
  VEC2 t;
  VEC2 hsize;
  //  FLOAT ca, sa;
  
public :
  RBoxObstacle(VEC2 pos = VEC2(0, 0), VEC2 n = VEC2(0, 1), VEC2 size = VEC2(1, 1));
  ~RBoxObstacle();

  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw(); 
#endif
  
  VEC2 getNormal(VEC2 p) const;
  FLOAT distance(VEC2 v) const;
  void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const;
  void exportMitsuba(std::ofstream & file) const;
  void exportSVG(std::ofstream & file, int id) const;
};

#endif
