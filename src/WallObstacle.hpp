#ifndef WALL_OBSTACLE_HPP
#define WALL_OBSTACLE_HPP

#include "Obstacle.hpp"

class WallObstacle : public Obstacle {
private :
  FLOAT pos;
  uint axis;
  VEC2 normal;
  int dir;
  
public :
  WallObstacle(uint a=0, FLOAT p=0, int d=1);
  ~WallObstacle();

  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw();
#endif
  
  FLOAT getPosition() const;
  VEC2 getNormal(VEC2 pos) const;
  VEC2 getNormal() const;
  
  FLOAT distance(VEC2 v) const;
  void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &n) const;

  void rotate(FLOAT angle);
  void exportSVG(std::ofstream & file, int id) const;
};

#endif
 
