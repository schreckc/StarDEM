/**
 * \file PoissonGenerator.cpp
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

#include "PoissonGenerator.hpp"

using namespace PoissonGenerator;


  FLOAT PoissonGenerator::GetDistance(const sPoint& P1, const sPoint& P2)
  {
    return (P1.v - P2.v).norm();
  }

  sGridPoint PoissonGenerator::ImageToGrid( const sPoint& P, float CellSize )
  {
    return sGridPoint( ( int )( P.v(0) / CellSize ), ( int )( P.v(1) / CellSize ), ( int )( P.v(2) / CellSize ) );
  }


sPoint PoissonGenerator::PopRandom( std::vector<sPoint>& Points, PRNG& Generator )
{
  const int Idx = Generator.RandomInt( Points.size()-1 );
  const sPoint P = Points[ Idx ];
  Points.erase( Points.begin() + Idx );
  return P;
}


sPoint PoissonGenerator::GenerateRandomPointAround( const sPoint& P, FLOAT MinDist, PRNG& Generator )
{
  // start with non-uniform distribution
  FLOAT R1 = Generator.RandomFloat();
  FLOAT R2 = Generator.RandomFloat();
  FLOAT R3 = Generator.RandomFloat();

  // radius should be between MinDist and 2 * MinDist
  FLOAT Radius = MinDist * ( R1 + 1.0f );

  // random angle
  FLOAT Angleh = 2 * 3.141592653589 * R2;
  FLOAT Anglez = 0;

  // the new point is generated around the point (x, y)
  VEC3 V(P.v(0) + Radius * cos(Angleh),
	 P.v(1) + Radius * sin(Angleh),
	 0);
  return sPoint(V);
}

/**
   Return a vector of generated points
   NewPointsCount - refer to bridson-siggraph07-poissondisk.pdf for details (the value 'k')
   Circle  - 'true' to fill a circle, 'false' to fill a rectangle
   MinDist - minimal distance estimator, use negative value for default
**/

std::list<VEC3> PoissonGenerator::GeneratePoissonPointsR(size_t NumPoints,
							    PRNG& Generator,
							    int NewPointsCount,
							    VEC3 RectSize,
							    FLOAT MinDist) {
  if ( MinDist < 0.0f ) {
    //    MinDist = pow( FLOAT(NumPoints), 1.0/3.0 ) / FLOAT(NumPoints);
    MinDist = pow(RectSize(0)*RectSize(1)/FLOAT(NumPoints), 1.0/3.0 );
  }

  std::vector<sPoint> SamplePoints;
  std::vector<sPoint> ProcessList;

  // create the grid
  FLOAT CellSize = MinDist / sqrt( 3.0f );

  int GridW = ( int )ceil( 1.0f / CellSize );
  int GridH = ( int )ceil( 1.0f / CellSize );
  int GridT = ( int )ceil( 1.0f / CellSize );

  sGrid Grid( GridW, GridH, GridT, CellSize );

  sPoint FirstPoint;
  do {
    FirstPoint = sPoint( Generator.RandomFloat(), Generator.RandomFloat(),0);	     
  } while (!(FirstPoint.IsInRectangle(RectSize)));

  // update containers
  ProcessList.push_back( FirstPoint );
  SamplePoints.push_back( FirstPoint );
  Grid.Insert( FirstPoint );

  // generate new points for each point in the queue
  while ( !ProcessList.empty() && SamplePoints.size() < NumPoints )
    {
#if POISSON_PROGRESS_INDICATOR
      // a progress indicator, kind of
      if ( SamplePoints.size() % 100 == 0 ) std::cout << ".";
#endif // POISSON_PROGRESS_INDICATOR

      sPoint Point = PopRandom( ProcessList, Generator );

      for ( int i = 0; i < NewPointsCount; i++ )
	{
	  sPoint NewPoint = GenerateRandomPointAround( Point, MinDist, Generator );

	  bool Fits = NewPoint.IsInRectangle(RectSize);

	  if ( Fits && !Grid.IsInNeighbourhood( NewPoint, MinDist, CellSize ) )
	    {
	      ProcessList.push_back( NewPoint );
	      SamplePoints.push_back( NewPoint );
	      Grid.Insert( NewPoint );
	      continue;
	    }
	}
    }

#if POISSON_PROGRESS_INDICATOR
  std::cout << std::endl << std::endl;
#endif // POISSON_PROGRESS_INDICATOR
  std::list<VEC3> out;
  for (auto &p : SamplePoints) {
    out.push_back(p.v);
  }
  return out;
}


std::list<VEC3> PoissonGenerator::GeneratePoissonPointsC(size_t NumPoints,
							    PRNG& Generator,
							    int NewPointsCount,
							    VEC3 SphereSize,
							    FLOAT MinDist) {
  if ( MinDist < 0.0f ) {
    //    MinDist = pow( FLOAT(NumPoints), 1.0/3.0 ) / FLOAT(NumPoints);
    MinDist = pow( 4.0/3.0*M_PI/FLOAT(NumPoints), 1.0/3.0 );
  }

  std::vector<sPoint> SamplePoints;
  std::vector<sPoint> ProcessList;

  // create the grid
  FLOAT CellSize = MinDist / sqrt( 3.0f );

  int GridW = ( int )ceil( 1.0f / CellSize );
  int GridH = ( int )ceil( 1.0f / CellSize );
  int GridT = ( int )ceil( 1.0f / CellSize );

  sGrid Grid( GridW, GridH, GridT, CellSize );

  sPoint FirstPoint;
  do {
    FirstPoint = sPoint( Generator.RandomFloat(), Generator.RandomFloat(), 0 );	     
  } while (!FirstPoint.IsInCircle());

  // update containers
  ProcessList.push_back( FirstPoint );
  SamplePoints.push_back( FirstPoint );
  Grid.Insert( FirstPoint );

  // generate new points for each point in the queue
  while ( !ProcessList.empty() && SamplePoints.size() < NumPoints )
    {
#if POISSON_PROGRESS_INDICATOR
      // a progress indicator, kind of
      if ( SamplePoints.size() % 100 == 0 ) std::cout << ".";
#endif // POISSON_PROGRESS_INDICATOR

      sPoint Point = PopRandom( ProcessList, Generator );

      for ( int i = 0; i < NewPointsCount; i++ )
	{
	  sPoint NewPoint = GenerateRandomPointAround( Point, MinDist, Generator );

	  bool Fits = NewPoint.IsInCircle();

	  if ( Fits && !Grid.IsInNeighbourhood( NewPoint, MinDist, CellSize ) )
	    {
	      ProcessList.push_back( NewPoint );
	      SamplePoints.push_back( NewPoint );
	      Grid.Insert( NewPoint );
	      continue;
	    }
	}
    }

#if POISSON_PROGRESS_INDICATOR
  std::cout << std::endl << std::endl;
#endif // POISSON_PROGRESS_INDICATOR
  std::list<VEC3> out;
  for (auto &p : SamplePoints) {
    out.push_back(p.v);
  }
  return out;
}

