#ifndef R0BOX_OBSTACLE_HPP
#define R0BOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class ROBoxObstacle : public Obstacle {
private :
  VEC2 pos;
  VEC2 n_init;
  VEC2 t_init;
  VEC2 n;
  VEC2 t;
  VEC2 hsize;

  
public :
  ROBoxObstacle(VEC2 pos = VEC2(0, 0), VEC2 n = VEC2(0, 1), VEC2 size = VEC2(1, 1));
  ~ROBoxObstacle();

  void animate(FLOAT time);
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
