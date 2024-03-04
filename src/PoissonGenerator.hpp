/**
 * \file PoissonGenerator.h
 * \brief
 *
 * Poisson Disk Points Generator
 *
 * \version 1.1.4
 * \date 19/10/2016
 * \author Sergey Kosarevsky, 2014-2016
 * \author support@linderdaum.com   http://www.linderdaum.com   http://blog.linderdaum.com
 */

/*******************************************************************************8
The MIT License (MIT)

Copyright (c) 2014-2023 Sergey Kosarevsky

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
************************************************************************************/

/*
  Usage example:
  #define POISSON_PROGRESS_INDICATOR 1
  #include "PoissonGenerator.h"
  ...
  PoissonGenerator::DefaultPRNG PRNG;
  const auto Points = PoissonGenerator::GeneratePoissonPoints( NumPoints, PRNG );
*/

// Fast Poisson Disk Sampling in Arbitrary Dimensions
// http://people.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf

// Implementation based on http://devmag.org.za/2009/05/03/poisson-disk-sampling/

/* Versions history:
 *		1.1.4 	Oct 19, 2016		POISSON_PROGRESS_INDICATOR can be defined outside of the header file, disabled by default
 *		1.1.3a	Jun  9, 2016		Update constructor for DefaultPRNG
 *		1.1.3		Mar 10, 2016		Header-only library, no global mutable state
 *		1.1.2		Apr  9, 2015		Output a text file with XY coordinates
 *		1.1.1		May 23, 2014		Initialize PRNG seed, fixed uninitialized fields
 *    1.1		May  7, 2014		Support of density maps
 *		1.0		May  6, 2014
 */

#ifndef POISSON_GENERATOR_HPP
#define POISSON_GENERATOR_HPP

#include <vector>
#include <list>
#include <random>
#include <stdint.h>
#include <time.h>
#include "dem_conf.hpp"
#include "error.hpp"

namespace PoissonGenerator
{

//  const char* Version = "1.1.4 (19/10/2016)";

  class PRNG
  {
  public:
    PRNG()
      : m_Gen( std::random_device()() )
      , m_Dis( 0.0f, 1.0f )
    {
      // prepare PRNG
      m_Gen.seed( time( nullptr ) );
    }

    explicit PRNG( uint32_t seed )
      : m_Gen( seed )
      , m_Dis( 0.0f, 1.0f )
    {
    }

    float RandomFloat()
    {
      return static_cast<float>( m_Dis( m_Gen ) );
    }

    int RandomInt( int Max )
    {
      std::uniform_int_distribution<> DisInt( 0, Max );
      return DisInt( m_Gen );
    }

  private:
    std::mt19937 m_Gen;
    std::uniform_real_distribution<float> m_Dis;
  };

  struct sPoint
  {
    sPoint()
      : v(VEC3(0, 0, 0))
      , m_Valid( false )
    {}
    sPoint( FLOAT X, FLOAT Y, FLOAT Z )
      : v(VEC3(X, Y, Z))
      , m_Valid( true )
    {}
    sPoint( VEC3 V )
      : v(V)
      , m_Valid( true )
    {}
    VEC3 v;
    bool m_Valid;
    //
    bool IsInRectangle(VEC3 RectSize) const
    {
      bool in = true;
      for (uint i = 0; i < 3; ++i) {
	in = in && v(i) >= 0 && v(i) <= RectSize(i);
      }
      return in;
	//	v(0) >= 0 && v(1) >= 0	&& v(2) >= 0 && v(0) <= 1 && v(1) <= 1 && v(2) <= 1 ;
    }
    //
    bool IsInCircle() const
    {
      VEC3 fv = v - VEC3(0.5, 0.5, 0.5);
      return fv.squaredNorm() <= (FLOAT)0.25;
    }
  };

  struct sGridPoint
  {
    sGridPoint( int X, int Y, int Z ): vi(X, Y, Z) {}
    Vector3i vi;
  };

FLOAT GetDistance(const sPoint& P1, const sPoint& P2);

sGridPoint ImageToGrid( const sPoint& P, float CellSize );

  struct sGrid
  {
    sGrid( int W, int H, int T, FLOAT CellSize ): m_W( W ), m_H( H ), m_T( T ), m_CellSize( CellSize ) {
      m_Grid.resize( m_H );
      for ( auto i = m_Grid.begin(); i != m_Grid.end(); i++ ) {
	i->resize( m_W );
	for ( auto j = i->begin(); j != i->end(); j++ ) {
	  j->resize( m_T );
	}
      }
    }

   void Insert( const sPoint& P ) {
      sGridPoint G = ImageToGrid( P, m_CellSize );
      //INFO(3, G.vi);
      m_Grid[ G.vi[0] ][ G.vi[1] ][ G.vi[2] ] = P;
    }
    
    bool IsInNeighbourhood( sPoint Point, FLOAT MinDist, FLOAT CellSize ) {
      sGridPoint G = ImageToGrid( Point, CellSize );

      // number of adjucent cells to look for neighbour points
      const int D = 5;

      // scan the neighbourhood of the point in the grid
      for ( int i = G.vi(0) - D; i < G.vi(0) + D; i++ ) {
	for ( int j = G.vi(1) - D; j < G.vi(1) + D; j++ ) {
	  for ( int k = G.vi(2) - D; k < G.vi(2) + D; k++ ) {
	    if ( i >= 0 && i < m_W && j >= 0 && j < m_H && k >= 0 && k < m_T ) {
	      sPoint P = m_Grid[i][j][k];
	      if ( P.m_Valid && GetDistance( P, Point ) < MinDist ) {
		return true;
	      }
	    }
	  }
	}
      }
      return false;
    }
    
  private:
    int m_W;
    int m_H;
    int m_T;
    FLOAT m_CellSize;

    std::vector< std::vector< std::vector<sPoint> > > m_Grid;
  };

  sPoint PopRandom( std::vector<sPoint>& Points, PRNG& Generator );

  sPoint GenerateRandomPointAround( const sPoint& P, FLOAT MinDist, PRNG& Generator );

  /**
     Return a vector of generated points inside a cuboid 
     NewPointsCount - refer to bridson-siggraph07-poissondisk.pdf for details (the value 'k')
     RectSize  - size of the cuboid filled
     MinDist - minimal distance estimator, use negative value for default
  **/
  std::list<VEC3> GeneratePoissonPointsR(
					    size_t NumPoints,
					    PRNG& Generator,
					    int NewPointsCount = 30,
					    VEC3 RectSize = VEC3(1, 1, 1),
					    FLOAT MinDist = -1.0f);

    /**
     Return a vector of generated points inside a sphere or elliposoid, of center at (0, 0, 0) 
     NewPointsCount - refer to bridson-siggraph07-poissondisk.pdf for details (the value 'k')
     SphereSize  - size of the ellipoid filled
     MinDist - minimal distance estimator, use negative value for default
  **/
  std::list<VEC3> GeneratePoissonPointsC(size_t NumPoints,
					 PRNG& Generator,
					 int NewPointsCount = 30,
					 VEC3 SphereSize = VEC3(1, 1, 1),
					 FLOAT MinDist = -1.0f);
} // namespace PoissonGenerator

#endif
