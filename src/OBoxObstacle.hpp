#ifndef OBOX_OBSTACLE_HPP
#define OBOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class OBoxObstacle : public Obstacle {
private :
  VEC2 min_pos;
  VEC2 max_pos;
  
public :
  OBoxObstacle(VEC2 min_p = VEC2(0, 0), VEC2 max_p = VEC2(0, 0));
  ~OBoxObstacle();

  void animate(FLOAT t);
  
  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw(); 
#endif
  
  VEC2 getNormal(VEC2 p) const;
  FLOAT distance(VEC2 v) const;
  void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const;
  void exportSVG(std::ofstream & file, int id) const;
};

#endif
