/* 
 * File: Times.hpp
 *
 * Copyright (C) 2019-2024  Camille Schreck
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

#ifndef TIMES_HPP
#define TIMES_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

class Times {
  
public :
  static Times *TIMES;
  static Times *TIMES_UP;

  enum timing_t {
    simu_time_ = 0,
    coll_detection_time_,
    coll_force_time_,
    display_time_,
    nTimes};

private :
  boost::posix_time::ptime init_time[nTimes];
  double loop_time[nTimes];
  double time_sum_frame[nTimes];
  double time_sum_all[nTimes];

  //count
  unsigned int frame;
  unsigned int step;
  unsigned int s;

public :
  Times();
  
  void init();

  void tick(unsigned int i);
  void tock(unsigned int i);
  double getTimeLastStep(unsigned int i);
  double getTimeLastFrame(unsigned int i);
  double getAverageStepTimeLastFrame(unsigned int i); //average step time for the last frame
  double getAverageStepTime(unsigned int i); 
  double getAverageFrameTime(unsigned int i);
  double getTotalTime(unsigned int i);
  
  double getTotalTime();
  void next_loop();
  void next_frame();
};

#endif //TIMES_HPP
