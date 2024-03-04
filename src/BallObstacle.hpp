#ifndef BALL_OBSTACLE_HPP
#define BALL_OBSTACLE_HPP

#include "Obstacle.hpp"

class BallObstacle : public Obstacle {
private :
  VEC2 pos;
  FLOAT radius;
  
public :
  BallObstacle(VEC2 p = VEC2(0, 0), FLOAT r = 1);
  ~BallObstacle();

  void animate(FLOAT t);
  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw();
#endif

  
  
  VEC2 getPosition() const;
  FLOAT getRadius() const;
  VEC2 getNormal(VEC2 p) const;
   
  FLOAT distance(VEC2 v) const;
  void getCollisionValues(VEC2 p, FLOAT & dist, VEC2 &normal) const;
  void exportMitsuba(std::ofstream & file) const;
  void exportSVG(std::ofstream & file, int id) const;
};

#endif
