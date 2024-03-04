#include "utils.hpp"
#include "error.hpp"

#include "Eigen/Dense"
#include "Eigen/Core"


VEC2 utils::intersection2d(const VEC2 o1, 
			   const VEC2 dir1, 
			   const VEC2 o2, 
			   const VEC2 dir2, 
			   VEC2 & coef) {
  IS_DEF(o1(0));
  IS_DEF(o1(1));
  IS_DEF(dir1(0));
  IS_DEF(dir1(1));
  IS_DEF(o2(0));
  IS_DEF(o2(1));
  IS_DEF(dir2(0));
  IS_DEF(dir2(1));
  
  if (!(!std::isnan(o1(0)) && !std::isnan(dir1(0)) && !std::isnan(o1(0)) && !std::isnan(dir1(0)))) {
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2 <<"\n\n"<<std::endl;
    INFO(3,"utils::intersection2d : NAN" << std::endl);
    TEST(false);
  }
  MAT2 M(2,2);
  VEC2 v = o2 - o1;
  if (fabs(dir1.dot(dir2)/(dir1.norm()*dir2.norm())) == 1) {
    INFO(3, "dir1.dot(dir2) dir1.norm dir2.norm \n"<< dir1.dot(dir2)<<" "<<dir1.norm()<<" "<<dir2.norm()<<" "<<dir1.dot(dir2)/(dir1.norm()*dir2.norm()));
    INFO(3, "utils::intersection2d : parallele \n"<<o1 <<"\n\n"
	 << dir1 <<"\n\n"
	 << o2 <<"\n\n"
	 << dir2 );
    coef(1) = 1;
    coef(0) = 0;
    return o2+dir2;
  }
  M << dir1, -dir2;
  coef = M.inverse()*v;
  VEC2 inter;
  if (!std::isnan(coef(0))) {
    inter = o1 + coef(0)*dir1;
  } else if (!std::isnan(coef(1))) {
    inter = o2 + coef(1)*dir2;
  } else {
    coef(1) = 1;
    coef(0) = 0;
    return o2+dir2;
  }
  if (std::isnan(inter(0))) {
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2 <<"\n\n"<<std::endl;
    std::cerr<<"inter non def"<<std::endl;
    coef(1) = 1;
    coef(0) = 0;
    return o2+dir2;
  }
  IS_DEF(inter(0));
  IS_DEF(inter(1));
  return inter;
}

VEC2 utils::intersection2d(const VEC2 o1, 
			   const VEC2 dir1, 
			   const VEC2 o2, 
			   const VEC2 dir2) {
  VEC2 coef;
  return intersection2d(o1, dir1, o2, dir2, coef);
}

VEC3 utils::intersection3d(const VEC3 o1, 
			   const VEC3 dir1, 
			   const VEC3 o2, 
			   const VEC3 dir2, 
			   VEC2 & coef) {
  MAT2 M(2,2);
  if (fabs(dir1.dot(dir2)/(dir1.norm()*dir2.norm())) == 1) {

    INFO(3, "utils::intersection3d : parallele" << std::endl);
    coef(1) = 1;
    coef(0) = 0;
    assert(false);
    return o2+dir2;
  }
  FLOAT d1dotd2 = dir1.dot(dir2);
  FLOAT n1 = dir1.squaredNorm();
  FLOAT n2 = dir2.squaredNorm();
  VEC3 diro = o2 - o1;
  M << n1, -d1dotd2, 
    d1dotd2, -n2;
  VEC2 v(diro.dot(dir1), diro.dot(dir2));

  coef =  M.inverse()*v;
  VEC3 inter;
  if (!std::isnan(coef(0))) {
    inter = o1 + coef(0)*dir1;
  } else if (!std::isnan(coef(1))) {
    inter = o2 + coef(1)*dir2;
  } else {
    std::cerr<<"echec utils::intersection3d \n"<<std::endl;
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2 <<"\n\n"<<std::endl;
    coef(1) = 1;
    coef(0) = 0;
    return o2+dir2;
  }
  if (std::isnan(inter(0))) {
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2 <<"\n\n"<<std::endl;
    std::cerr<<"inter non def"<<std::endl;
    assert(false);
  }
  return inter;
}

VEC3 utils::intersection3d(const VEC3 o1, 
			   const VEC3 dir1, 
			   const VEC3 o2, 
			   const VEC3 dir2a,
			   const VEC3 dir2b,
			   VEC3 & coef) {

  MAT3 M;
  VEC3 v = o2 - o1;
  M << dir1, -dir2a, -dir2b;
  //  coef =  M.householderQr().solve(v);
  coef =  M.inverse()*v;
  VEC3 inter;
  if (!std::isnan(coef(0))) {
    inter = o1 + coef(0)*dir1;
  } else if (!std::isnan(coef(1)) && !std::isnan(coef(2))) {
    inter = o2 + coef(1)*dir2a + coef(2)*dir2b ;
  } else {
    std::cerr<<"echec utils::intersection3d \n"<<std::endl;
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2a <<"\n\n"
	      <<dir2b<<std::endl;
    assert(false);
    coef(1) = 0;
    coef(0) = 1;
    coef(2) = 0;
    return o1+dir1;
  }
  if (std::isnan(inter(0))) {
    std::cout << o1 <<"\n\n"
	      << dir1 <<"\n\n"
	      << o2 <<"\n\n"
	      << dir2a <<"\n\n"
	      << dir2b <<std::endl;
    INFO(3, "coef \n"<<coef);
    std::cerr<<"inter non def"<<std::endl;
    TEST(false);
    return o1+dir1;
  }
  return inter;

}

MAT3 utils::rotation(FLOAT angle, VEC3 axe) {
  FLOAT cosx = cos(angle);
  FLOAT sinx = sin(angle);
  MAT3 Q(3,3);
  Q << 0, -axe(2), axe(1), 
    axe(2), 0, -axe(0), 
    -axe(1), axe(0), 0;
  
  MAT3 P = axe*axe.transpose();
  MAT3 R = P + (MAT3::Identity(3,3) - P) * cosx + Q * sinx;
  return R;
}

VEC3 utils::rotation(VEC3 v, FLOAT angle, VEC3 axe) {
  FLOAT cosx = cos(angle);
  FLOAT sinx = sin(angle);
  MAT3 Q(3,3);
  Q << 0, -axe(2), axe(1), 
    axe(2), 0, -axe(0), 
    -axe(1), axe(0), 0;
  
  MAT3 P = axe*axe.transpose();
  MAT3 R = P + (MAT3::Identity(3,3) - P) * cosx + Q * sinx;
  return R * v;
}

VEC2 utils::rotation(VEC2 v, FLOAT angle) {
  FLOAT cosx = cos(angle);
  FLOAT sinx = sin(angle);
  MAT2 R(2,2);
  R << cosx, -sinx, 
    sinx, cosx;
  IS_DEF(cosx);
  IS_DEF(sinx);
  VEC2 out(v[0]*cosx - v[1]*sinx, v[0]*sinx + v[1]*cosx);
  return out;
}

VEC2 utils::projection(const VEC2 p, const VEC2 origine, const VEC2 dir, const VEC2 dir_proj, FLOAT & coef) {
  IS_DEF(p(0));
  IS_DEF(p(1));
  IS_DEF(origine(0));
  IS_DEF(origine(1));
  IS_DEF(dir(0));
  IS_DEF(dir(1));
  TEST(dir.norm() > 0);

  VEC2 c;
  VEC2 proj = intersection2d(p, dir_proj, origine, dir, c);
  coef = c[1];
  return proj;
}

VEC2 utils::projectionOrtho(const VEC2 p, const VEC2 origine, const VEC2 dir, FLOAT & coef) {
  IS_DEF(p(0));
  IS_DEF(p(1));
  IS_DEF(origine(0));
  IS_DEF(origine(1));
  IS_DEF(dir(0));
  IS_DEF(dir(1));
  TEST(dir.norm() > 0);
  MAT3 M(3, 3);
  assert(dir.norm() != 0);
  M << 1, 0, -dir(0), 
    0, 1, -dir(1), 
    dir(0), dir(1), 0;
  VEC3 v(3);
  v << origine(0), origine(1), p.dot(dir);
  VEC3 result = M.inverse()*v;
  coef = result(2);
  IS_DEF(result(0));
  IS_DEF(result(1));
  IS_DEF(result(2));
  coef = result(2);
  return VEC2(result(0), result(1));
}

VEC2 utils::projectionOrtho(const VEC2 p, const VEC2 origine, const VEC2 dir) {
  FLOAT coef;
  return projectionOrtho(p, origine, dir, coef);
}

VEC3 utils::projectionOrtho(const VEC3 p, const VEC3 origine, const VEC3 dir) {
  FLOAT coef;
  return projectionOrtho(p, origine, dir, coef);
}



VEC3 utils::projectionOrtho(const VEC3 p, const VEC3 origine, const VEC3 dir, FLOAT & coef) {
  MAT4 M(4, 4);
  assert(dir.norm() != 0);
  M << 1, 0, 0, -dir(0), 
    0, 1, 0, -dir(1), 
    0, 0 ,1, -dir(2),
    dir(0), dir(1), dir(2), 0;
  VEC4 v(4);
  v << origine(0), origine(1), origine(2), p.dot(dir);
  VEC4 result = M.inverse()*v;
  coef = result(3);
  return VEC3(result(0), result(1), result(2));
}

VEC3 utils::projectionOrtho(const VEC3 p, const VEC3 origine, const VEC3 dir1, const VEC3 dir2) {
  VEC2 coef(2);
  return projectionOrtho(p, origine, dir1, dir2, coef);
}

VEC3 utils::projectionOrtho(const VEC3 p, const VEC3 origine, const VEC3 dir1, const VEC3 dir2, VEC2 & coef) {
  IS_DEF(p(0));
  IS_DEF(p(1));
  IS_DEF(p(2));
  IS_DEF(origine(0));
  IS_DEF(origine(1));
  IS_DEF(origine(2));
  IS_DEF(dir1(0));
  IS_DEF(dir1(1));
  IS_DEF(dir1(2));
  IS_DEF(dir2(0));
  IS_DEF(dir2(1));
  IS_DEF(dir2(2));

  TEST(dir1.norm() != 0);
  TEST(dir2.norm() != 0);

  VEC3 proj;
  FLOAT ps = dir1.dot(dir2);
  FLOAT normes = dir1.norm()*dir2.norm();
  IS_DEF(normes);
  //  VEC2 coef(2);
  if (fabs(ps) > normes - EPS && fabs(ps) < normes + EPS) {
    proj = projectionOrtho(p, origine, dir1, coef(0));
    coef(1) = 0;
  } else {
    MAT2 M(2, 2);
    M << dir1.dot(dir1), ps,
      ps, dir2.dot(dir2);
    VEC2 v(2);
    v(0) = (p-origine).dot(dir1);
    v(1) = (p-origine).dot(dir2);
    coef = M.inverse()*v;
    proj = origine + coef(0)*dir1 + coef(1)*dir2;
  }
  IS_DEF(coef(0));
  IS_DEF(coef(1));
  return proj;
}

FLOAT utils::distancePointDroite(const VEC3 p, const VEC3 origine, const VEC3 dir) {
  VEC3 proj = projectionOrtho(p, origine, dir);
  return (p-proj).norm();
}

FLOAT utils::distancePointDroite(const VEC2 p, const VEC2 origine, const VEC2 dir) {
  VEC2 proj = projectionOrtho(p, origine, dir);
  return (p-proj).norm();
}

FLOAT utils::distancePointSegment(const VEC2 p, const VEC2 origine, const VEC2 dir) {
  FLOAT coef;
  VEC2 proj = projectionOrtho(p, origine, dir, coef);
  if (coef < 0) {
    proj = origine;
  }
  if (coef > 1) {
    proj = origine + dir;
  }
  return (p-proj).norm();
}

VEC2 utils::newCoord(VEC2 p, VEC2 axeX, VEC2 axeY) {
  MAT2 M(2, 2);
  M << axeX, axeY;
  VEC2 result = M.inverse()*p;
  return result;
}

VEC3 utils::newCoord(VEC3 p, 
		     VEC3 axeX, 
		     VEC3 axeY, 
		     VEC3 axeZ) {
  MAT3 M(3, 3);
  M << axeX, axeY, axeZ;
  VEC3 result = M.inverse()*p;
  if (std::isnan(result(0))) {
    std::cout << p << "\n\n"
	      << axeX << "\n\n"
	      << axeY << "\n\n"
	      << axeZ << std::endl;
    assert(!std::isnan(result(0)));
  }
  return result;
}


FLOAT utils::aire(const VEC2 x1, 
		  const VEC2 x2,
		  const VEC2 x3) {  
  return fabs(((x2(0) - x1(0))*(x3(1) - x1(1)) - 
	       (x3(0) - x1(0))*(x2(1) - x1(1)))/2);
}

FLOAT utils::aire(const VEC3 x1, 
		  const VEC3 x2,
		  const VEC3 x3) {  
  return fabs(((x3 - x1).cross(x2 - x1)).norm()/2.0);
}

FLOAT utils::curvature(const VEC3 p_prev,
		       const VEC3 p, 
		       const VEC3 p_next) {
  VEC3 a = (p - p_prev);
  VEC3 b = (p - p_next);
  if (a.norm() != 0 && b.norm() != 0) {
    a.normalize();
    b.normalize();
    FLOAT teta = acos(a.dot(b));
    return fabs(teta - M_PI)/M_PI;
  } else {
    return 0;
  }
}

VEC3 utils::gradientCurvature(const VEC3 p_prev,
			      const VEC3 p, 
			      const VEC3 p_next) {
  VEC3 grad;
  VEC3 a = p_prev - p;
  VEC3 b = p_next - p;
  FLOAT adotb = a.dot(b);
  FLOAT na = a.norm();
  FLOAT nb = b.norm();
  if (fabs(adotb/(na*nb)) > 0.999) {
    grad = VEC3(0, 0, 0);
  } else {
    for (int k = 0; k < 3; ++k) {
      FLOAT x = p(k);
      FLOAT x1 =  p_prev(k);
      FLOAT x2 =  p_next(k);
	  
      FLOAT dcurv_dx = -((2*x - x1 - x2)/(na*nb) + 
			 adotb/pow(na*nb, 3)*((x1-x)*nb + (x2 - x)*na)) *
	1/sqrt(1 - pow(adotb/(na*nb),2));
      grad(k) = dcurv_dx * curvature(p_prev, p, p_next);
	  
    }
    if (a.dot(grad) < 0 || b.dot(grad) < 0) {
      grad = -grad;
    }
  }
  return grad;
}


FLOAT utils::dihedral_angle(VEC3 e1, 
			    VEC3 e2,
			    VEC3 s1,
			    VEC3 s2) {
  VEC3 u = e2 - e1;
  TEST(u.norm() != 0);
  u.normalize();
  
  VEC3 b1 = s1 - e1;
  VEC3 b2 = s2 - e1;
  b1.normalize();
  b2.normalize();
  TEST(b1.norm() != 0);
  TEST(b2.norm() != 0);

  VEC3 n1 = u.cross(b1);
  VEC3 n2 = -u.cross(b2);
  
  TEST(e1 != s2);
  TEST(e1 != s1);

  TEST(b1.norm() != 0);
  TEST(b2.norm() != 0);

  if (n1.norm() == 0 || n2.norm() == 0) {
    b1 = s1 - e1;
    b2 = s2 - e1;
    b1.normalize();
    b2.normalize();
    TEST(b1.norm() != 0);
    TEST(b2.norm() != 0);
    
    n1 = u.cross(b1);
    n2 = -u.cross(b2);
  }
  TEST(n1.norm() != 0);
  ERROR(n2.norm() != 0, "n2 NULL utils::dihedral_angle",
	e1<<"\n\n"<<
	e2<<"\n\n"<<
	s2);
  n1.normalize();
  n2.normalize();

  VEC3 v1 = n1.cross(u);
  VEC3 v2 = n2.cross(u);
  v1.normalize();
  v2.normalize();
  
  FLOAT angle;
  if (n1.dot(n2) < 1 + EPS && n1.dot(n2) > 1 - EPS) {
    angle = 0;
  }  else if (n1.dot(n2) < -1 + EPS && n1.dot(n2) > - 1 - EPS) {
    angle = M_PI;
  } else {
    angle = acos(n1.dot(n2));
  }
  TEST(e1 != e2);
  TEST(e1 != s2);
  TEST(e1 != s1);
  TEST(e2 != s2);
  TEST(e2 != s1);
  TEST(s1 != s2);
  ERROR(!std::isnan(angle) && !std::isinf(angle), "angle undef  utils::dihedral_angle", "n1 n2  : "<<n1.dot(n2)<<" "<<acos(n1.dot(n2))<<"\n\n"<<n1<<"\n\n"<<n2<<"\n\n"<<e1<<"\n\n"<<e2<<"\n\n"<<s1<<"\n\n"<<s2);
  return angle;
} 


bool utils::intersectionCircle(VEC2 & i1, VEC2 & i2, VEC2 c1, FLOAT r1, VEC2 c2, FLOAT r2) {
  FLOAT N = (r2*r2 - r1*r1 - c2(0)*c2(0) + c1(0)*c1(0) - c2(1)*c2(1) + c1(1)*c1(1)) / (2 * (c1(1) - c2(1))); 
  FLOAT quot = (c1(0) - c2(0))/(c1(1) - c2(1));
  FLOAT A = quot * quot + 1;
  FLOAT B = 2*c1(1)*quot - 2*N*quot - 2*c1(0);
  FLOAT C = c1(0)*c1(0) + c1(1)*c1(1) + N*N -r1*r1 - 2*c1(1)*N;

  FLOAT delta = B*B - 4*A*C;
  bool intersect = delta >= 0;
  if (intersect) {
    i1(0) = (-B - sqrt(delta))/(2*A);
    i2(0) = (-B + sqrt(delta))/(2*A);
    
    i1(1) = N - i1(0)*quot;
    i2(1) = N - i2(0)*quot;    
  }
  return intersect;
}


FLOAT utils::getDistanceToOrigine(VEC3 p, VEC3 n) {
  VEC3 tan1, tan2;

  if (n(0) != 0 || n(1) != 0) {
    tan1 = VEC3(-n(1), n(0), 0);
    tan2 = n.cross(tan1);
  } else {
    tan1 = VEC3(1, 0, 0);
    tan2 = VEC3(0,1,0);
  }
  VEC3 proj = utils::projectionOrtho(VEC3(0,0,0), p, tan1, tan2);
  FLOAT d;

  if (n.dot(proj) > 0) {
    d = - proj.norm();
  } else {
    d = proj.norm();
  }
  return d;
}


void utils::interpolationCubique(const VEC3 & A, const VEC3 & tanA,
				 const VEC3 & B, const VEC3 & tanB,
				 std::vector<VEC3> & coefs) {
  coefs = std::vector<VEC3>(4);
  coefs[3] = A;
  coefs[2] = tanA;
  VEC3 C = B - A - tanA;;
  VEC3 D = tanB - tanA;
  coefs[0] = D - 2*C;
  coefs[1] = 3*C - D;
  IS_DEF(coefs[0](0));
  IS_DEF(coefs[0](1));
  IS_DEF(coefs[0](2));

}

VEC3 utils::cubique(const std::vector<VEC3> & coefs, FLOAT param) {
  VEC3 out = coefs[0] * pow(param, 3) + coefs[1] * pow(param, 2) +
    coefs[2] * param + coefs[3];
  return out;
}


bool utils::isBetween(const VEC2 & x, const VEC2 & p1, const VEC2 & p2) {
  VEC2 dir1 = p1 - x;
  VEC2 dir2 = p2 - x;
  dir1.normalize();
  dir2.normalize();
  
  FLOAT prod = dir1.dot(dir2);
  bool between = prod > - 1 - EPS && prod < - 1 + EPS;
  return between;
}


bool utils::intersection2Spheres(const VEC3 c1, const FLOAT r1, 
				 const VEC3 c2, const FLOAT r2, 
				 VEC3 & c_inter, FLOAT & r_inter) {
  VEC3 dir = c2 - c1;
  FLOAT d = dir.norm();
  TEST(d != 0);
  dir /= d;
  bool inter;
  if (d < r1 + r2) {
    inter = true;   
    FLOAT x = (r1*r1 - r2*r2 + d*d)/(2*d);
    c_inter = c1 + x * dir;
    ERROR(r1*r1 - x*x >= 0, "x r1 d  utils::intersection2Spheres", x<<" "<<r1<<" "<<d);
    r_inter = sqrt(r1*r1 - x*x);
  } else {
    inter = false;
    r_inter = 0;
    c_inter = (c1 + r1 * dir + c2 - r2 * dir)/2.0;
  }
  return inter;
}


VEC3 utils::projectionOnCircle(const VEC3 p, const VEC3 c, const FLOAT r, const VEC3 normal) {
  TEST(normal.norm() != 0);
  VEC3 dir1;
  if (normal[0] != 0 ||  normal[1] != 0) {
    dir1 = VEC3(normal[1], -normal[0], 0);
  } else {
    dir1 = VEC3(normal[2], 0, -normal[0]);
  }
  dir1.normalize();
  VEC3 dir2 = normal.cross(dir1);
  VEC3 proj = projectionOrtho(p, c, dir1, dir2);
  
  VEC3 dir = proj - c;
  TEST(p != c);
  dir.normalize();
  VEC3 out = c + r * dir;
  return out;
}

bool utils::isParallele(const VEC2 dir1, const VEC2 dir2) {
  bool para = fabs(dir1.dot(dir2)/(dir1.norm()*dir2.norm())) == 1;
  return para;
}

VEC2 utils::orthogonal(const VEC2 v) {
  TEST(v.norm() != 0);
  IS_DEF(v(0));
  IS_DEF(v(1));
  VEC2 ortho = VEC2(v(1), -v(0));
  TEST(ortho.norm() != 0);
  ortho.normalize();
  return ortho;
}

bool utils::isCrossing(VEC2 o1, VEC2 dir1, VEC2 o2, VEC2 dir2) {
  bool para = isParallele(dir1, dir2);
  bool cr = false;
  if (!para) {
    VEC2 coef;
    intersection2d(o1, dir1, o2, dir2, coef);
    cr = coef(0) <= 1 && coef(0) >= 0 && coef(1) <=1 && coef(0) >= 0;
  }
  return cr;
}



VEC3 utils::normal(const VEC3 & v1, const VEC3 & v2, const VEC3 & v3) {
  VEC3 n = (v2 - v1).cross(v3 - v1);
  if (n.norm() != 0) {
    n.normalize();
  } else {
    n = VEC3(0, 0, 0);
  }
  return n;
}




bool utils::isInExt(const VEC2 & v2d, const VEC2 & x1, const VEC2 & x2, const VEC2 & x3) {
  bool isin;

  FLOAT a = x2(0) - x1(0);
  FLOAT c = x3(0) - x1(0);
  FLOAT b = x2(1) - x1(1);
  FLOAT d = x3(1) - x1(1);
  FLOAT v1 = v2d(0) - x1(0);
  FLOAT v2 = v2d(1) - x1(1);

  FLOAT det = a*d - b*c;

  FLOAT c1 = (v1*d - v2*c)/det;
  FLOAT c2 = (v2*a - v1*b)/det;
  
  isin = c1 >= /*-EPS*/0 && c2 >= /*-EPS*/0 && c1 + c2 <= 1 /*+ physicValues::PHYSICS->epsilon_l*/; 
  return isin;
} 


bool utils::computeEigen(MAT2 & M, VEC2 & eigen_val, MAT2 & eigen_vec) {
  FLOAT trace = M(0, 0) + M(1, 1);
  FLOAT det = M(0, 0)*M(1, 1) - M(1, 0)*M(0, 1);

  bool success = (pow(trace, 2)-4.0*det >= 0); 
  if (success) {
    eigen_val(0) = (trace - sqrt(pow(trace, 2)-4.0*det))/2.0;
    eigen_val(1) = (trace + sqrt(pow(trace, 2)-4.0*det))/2.0;
    if (M(1, 0) != 0) {
      eigen_vec(0, 0) = eigen_val(0) - M(1, 1);
      eigen_vec(1, 0) = M(1, 0);
      eigen_vec(0, 1) = eigen_val(1) - M(1, 1);
      eigen_vec(1, 1) =  M(1, 0);
    } else if (M(0, 1) != 0) {
      eigen_vec(0, 0) = M(0, 1);
      eigen_vec(1, 0) = eigen_val(0) - M(0, 0);
      eigen_vec(0, 1) = M(0, 1);
      eigen_vec(1, 1) = eigen_val(1) - M(0, 0);
    } else {
      eigen_vec(0, 0) = 1;
      eigen_vec(1, 0) = 0;
      eigen_vec(0, 1) = 0;
      eigen_vec(1, 1) = 1;
    }
    TEST(eigen_vec.col(0).norm() != 0);
    TEST(eigen_vec.col(1).norm() != 0);
    eigen_vec.col(0).normalize();
    eigen_vec.col(1).normalize();
  }
  return success;
}


FLOAT utils::signedAngle(VEC3 d1, VEC3 d2, VEC3 normal) {
  d1.normalize();
  d2.normalize();
  FLOAT dot = d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2];
  FLOAT det = d1[0]*d2[1]*normal[2] + d2[0]*normal[1]*d1[2] + normal[0]*d1[1]*d2[2] - 
    d1[2]*d2[1]*normal[0] - d2[2]*normal[1]*d1[0] - normal[2]*d1[1]*d2[0];

  FLOAT a = atan2(det, dot);
  return a;
}

FLOAT utils::signedAngle(VEC2 d1, VEC2 d2) {
  d1.normalize();
  d2.normalize();
  FLOAT a = atan2(d2[0]*d1[1]-d2[1]*d1[0], d2[0]*d1[0]+d2[1]*d1[1]);
  return a;
}


VEC3 utils::bissectrice(VEC3 d1, VEC3 d2, VEC3 normal) {
  VEC3 biss(0, 0, 0);
  if (d1.squaredNorm() != 0 && d2.squaredNorm() != 0) { 
    TEST(d1.norm() != 0);
    d1.normalize();
    TEST(d2.norm() != 0);
    d2.normalize();

    FLOAT dot = d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2];
    FLOAT det = d1[0]*d2[1]*normal[2] + d2[0]*normal[1]*d1[2] + normal[0]*d1[1]*d2[2] - 
      d1[2]*d2[1]*normal[0] - d2[2]*normal[1]*d1[0] - normal[2]*d1[1]*d2[0];

    FLOAT a = atan2(det, dot);
    if (a <= 0) {
      a += 2*M_PI;
    }
    a /= 2.0;
  
    biss = utils::rotation(d1, a, normal);
  }
  return biss;
}

VEC2 utils::bissectrice(VEC2 d1, VEC2 d2) {
  VEC2 biss(0, 0);
  if (d1.squaredNorm() != 0 && d2.squaredNorm() != 0) { 
    d1.normalize();
    d2.normalize();
    FLOAT a = atan2(d2[0]*d1[1]-d2[1]*d1[0], d2[0]*d1[0]+d2[1]*d1[1]);
    if (a <= 0) {
      a += 2*M_PI;
    }
    a /= 2.0;
  
    biss = utils::rotation(d2, a);
  }
  return biss;
}

VEC2 utils::symetrie(VEC2 p, VEC2 o, VEC2 dir) {
  VEC2 proj = projectionOrtho(p, o, dir);
  VEC2 out = 2*proj - p;
  return out;
}
