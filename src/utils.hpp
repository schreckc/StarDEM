
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include "definitions.hpp"

#define EPS 0.0000001

using namespace Eigen;

namespace utils {

  /**
   * compute the intersection of the line of origine o1 and direction 
   * dir1 and the line of origine o2 and direction dir2 in the 2d space.
   * the vector coef contains the coefficents such that :
   * inter2d = o1 + coef(0)*dir1   and
   * inter2d = o2 + coef(1)*dir2.
   */ 
  VEC2 intersection2d(const VEC2 o1, 
				 const VEC2 dir1, 
				 const VEC2 o2, 
				 const VEC2 dir2, 
				 VEC2 & coef);

  VEC2 intersection2d(const VEC2 o1, 
			  const VEC2 dir1, 
			  const VEC2 o2, 
			  const VEC2 dir2);


  /**
   * compute the intersection (if existing) 
   * of the line of origine o1 and direction 
   * dir1 and the line of origine o2 and direction dir2 in the 3d space.
   * the vector coef contains the coefficents such that :
   * inter2d = o1 + coef(0)*dir1   and
   * inter2d = o2 + coef(1)*dir2.
   */ 
  VEC3 intersection3d(const VEC3 o1, 
				 const VEC3 dir1, 
				 const VEC3 o2, 
				 const VEC3 dir2, 
				 VEC2 & coef);

  /**
   * compute the intersection of the line of origine o1 and direction 
   * dir1 and the plane of origine o2 and direction dir2a and dir2b
   *  in the 3d space.
   * the vector coef contains the coefficents such that :
   * inter2d = o1 + coef(0)*dir1   and
   * inter2d = o2 + coef(1)*dir2a + coef(2)*dir2b.
   */ 
  VEC3 intersection3d(const VEC3 o1, 
				 const VEC3 dir1, 
				 const VEC3 o2, 
				 const VEC3 dir2a,
				 const VEC3 dir2b,
				 VEC3 & coef);

  /**
   * apply the rotation around the axe (with the angle a) to the
   * vector v
   */
  
  VEC3 rotation(VEC3 v, FLOAT angle, VEC3 axe);

  VEC2 rotation(VEC2 v, FLOAT angle);

  MAT3 rotation(FLOAT angle, VEC3 axe);


  VEC2 projection(const VEC2 p, const VEC2 origine, const VEC2 dir, const VEC2 dir_proj, FLOAT & coef);
  /**
   * project the vector p on the line passing through origine and 
   * of direction dir
   */
  VEC2 projectionOrtho(const VEC2 p, 
		      const VEC2 origine, const VEC2 dir);
  VEC2 projectionOrtho(const VEC2 p, 
		      const VEC2 origine, const VEC2 dir, 
		      FLOAT & coef);
  VEC3 projectionOrtho(const VEC3 p, 
		      const VEC3 origine, const VEC3 dir);
  VEC3 projectionOrtho(const VEC3 p, 
		      const VEC3 origine, const VEC3 dir, 
		      FLOAT & coef);
  VEC3 projectionOrtho(const VEC3 p, 
		      const VEC3 origine, const VEC3 dir1, VEC3 dir2);
  VEC3 projectionOrtho(const VEC3 p, 
		      const VEC3 origine, const VEC3 dir1, VEC3 dir2, 
		      VEC2 & coef);

  FLOAT distancePointDroite(const VEC3 p, const VEC3 origine, const VEC3 dir);
  FLOAT distancePointDroite(const VEC2 p, const VEC2 origine, const VEC2 dir);
  FLOAT distancePointSegment(const VEC2 p, const VEC2 origine, const VEC2 dir);
  /**
   * compute the coord of p for a basis of origin (0,0) and of axeX
   * and axeY
   */
  VEC2 newCoord(VEC2 p, VEC2 axeX, VEC2 axeY);

  /**
   * compute the coord of p for a basis of origin (0,0,0) and of axeX,
   * axeY and axeZ
   */
  VEC3 newCoord(VEC3 p, VEC3 axeX, VEC3 axeY, VEC3 axeZ);

  /**
   * return the area defined by the 2d vectors
   * v1, v2 and v3. 
   */
  FLOAT aire(const VEC2 x1, 
		     const VEC2 x2,
		     const VEC2 x3);

  /**
   * return the area defined by the 3d vectors
   * v1, v2 and v3. 
   */
  FLOAT aire(const VEC3 x1, 
		     const VEC3 x2,
		     const VEC3 x3);

  /**
   * compute the curvature between the three points
   */
  FLOAT curvature(const VEC3 p_prev,
			  const VEC3 p, 
			  const VEC3 p_next);

  /**
   * compute the gadient of the curvature between the three points
   */
  VEC3 gradientCurvature(const VEC3 p_prev,
				    const VEC3 p, 
				    const VEC3 p_next);

 
  /**
   * compute the dihedral angle between the triangles
   * e1,e2,s1 and e2, e1, s2
   */
  FLOAT dihedral_angle(VEC3 e1, 
			VEC3 e2,
			VEC3 s1,
			VEC3 s2);


  bool intersectionCircle(VEC2 & i1, VEC2 & i2, VEC2 c1, FLOAT r1, VEC2 c2, FLOAT r2);

   void rotate(VEC3 axe, FLOAT angle, VEC3 & v);
  
  FLOAT getDistanceToOrigine(VEC3 p, VEC3 n);

  void interpolationCubique(const VEC3 & A, const VEC3 & tanA,
				   const VEC3 & B, const VEC3 & tanB,
				   std::vector<VEC3> & coefs);
  
  VEC3 cubique(const std::vector<VEC3> & coefs, FLOAT param);

  bool isBetween(const VEC2 & x, const VEC2 & p1, const VEC2 & p2);

  bool intersection2Spheres(const VEC3 c1, const FLOAT r1, 
				 const VEC3 c2, const FLOAT r2, 
				   VEC3 & c_inter, FLOAT & r_inter);
  bool intersection3Spheres(const std::vector<VEC3> c_tab,
				 const std::vector<FLOAT> r_tab,
				   VEC3 & i1, VEC3 & i2);

  bool intersection3Spheres(const VEC3 c1, const FLOAT r1, 
				 const VEC3 c2, const FLOAT r2, 
				 const VEC3 c3, const FLOAT r3, 
			    VEC3 & i1, VEC3 & i2); 

  VEC3 projectionOnCircle(const VEC3 p, const VEC3 c, const FLOAT r, const VEC3 normal);

  bool isParallele(const VEC2 dir1, const VEC2 dir2);

  VEC2 orthogonal(const VEC2 v);

  bool isCrossing(VEC2 o1, VEC2 dir1, VEC2 o2, VEC2 dir2);


  VEC3 normal(const VEC3 & v1, const VEC3 & v2, const VEC3 & v3);
  
  bool isInExt(const VEC2 & v2d, const VEC2 & x1, const VEC2 & x2, const VEC2 & x3);
  
  bool computeEigen(MAT2 & M, VEC2 & eigen_val, MAT2 & eigen_vec);

  FLOAT signedAngle(VEC3 d1, VEC3 d2, VEC3 normal);
  FLOAT signedAngle(VEC2 d1, VEC2 d2);

  VEC3 bissectrice(VEC3 d1, VEC3 d2, VEC3 normal);

  VEC2 bissectrice(VEC2 d1, VEC2 d2);

  VEC2 symetrie(VEC2 p, VEC2 o, VEC2 dir);
};
#endif
