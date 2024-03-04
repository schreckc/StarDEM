/* 
 * File: Times.cpp
 *
 * Copyright (C) 2019-2023  Camille Schreck
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


#include "Times.hpp"
#include <time.h>

using namespace boost::posix_time;

Times *Times::TIMES(new Times());
Times *Times::TIMES_UP(new Times());
Times::Times() {
  init();
}

void Times::init() {
  for (unsigned int i = 0; i < nTimes; ++i) {
    init_time[i] = not_a_date_time;
    loop_time[i] = 0;
    time_sum_frame[i] = 0;
    time_sum_all[i] = 0;
  }
  frame = 1;
  step = 1;
  s = 1;
}

void Times::tick(unsigned int i) {
  init_time[i] = microsec_clock::local_time();
}

void Times::tock(unsigned int i) {
  assert(init_time[i] != not_a_date_time);
  ptime t_end = microsec_clock::local_time();
  loop_time[i] += (t_end - init_time[i]).total_microseconds()*1e-6;
  init_time[i] = not_a_date_time;
}

double Times::getTimeLastStep(unsigned int i) {
  return loop_time[i];
}

double Times::getTimeLastFrame(unsigned int i) {
  return time_sum_frame[i];
}

double Times::getAverageStepTimeLastFrame(unsigned int i) {
  return time_sum_frame[i]/((double)s);
}

 double Times::getAverageStepTime(unsigned int i) {
   return time_sum_all[i]/((double)step);
 }

double Times::getAverageFrameTime(unsigned int i) {
  return time_sum_all[i] / ((double) frame);
}

double Times::getTotalTime(unsigned int i) {
  return time_sum_all[i];
}

double Times::getTotalTime() {
  double total = 0;
    for (unsigned int i = 0; i < nTimes; ++i) {
      total = time_sum_all[i];
    }
  return total;
}



void Times::next_loop() {
  for (unsigned int i = 0; i < nTimes; ++i) {
    time_sum_frame[i] += loop_time[i];
    time_sum_all[i] += loop_time[i];
    init_time[i] = not_a_date_time;
    loop_time[i] = 0;
  }
  ++s;
  ++step;
}

void Times::next_frame() {
  for (unsigned int i = 0; i < nTimes; ++i) {
    time_sum_frame[i] = 0;
  }
  s = 0;
  ++frame;
}
