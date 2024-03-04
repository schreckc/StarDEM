#ifndef DEM_CONF_HPP
#define DEM_CONF_HPP

#include "definitions.hpp"

#define _WHITE

namespace dem_conf {

  //Grid
  extern Eigen::VEC2 size_grid_; // millimeter
  extern Eigen::VEC2I size_grid_n_; // nb nodes per side
  extern FLOAT grid_spacing_; // mm  if ths cell size is too small compared to the size
             // of the particles, this is automatically change to be 2*particles radius.
  
  // elasticity values
  extern FLOAT mu_, lambda_;
  extern FLOAT young_modulus_, poisson_;

  extern  FLOAT ks_; // tangential stiffness 
  extern FLOAT kn_; // normal stiffness
  extern FLOAT damping_; // normal damping
  extern FLOAT damping_t; // rotational damping
  
  extern FLOAT dt_; // second
  extern FLOAT frame_step;
  
  extern Eigen::VEC2 gravity_; // m/s^2

  extern FLOAT mass_; // default mass of a particle
  
  extern FLOAT friction_coef_;

  extern int sm_resolution; // resolution of the smarticle for collision detection
  extern int sm_resolution_dist; // resolution of the lookup table use for computing distance
  extern int sm_resolution_display; // resolution used only for display

  extern bool wireframe_;
  extern int replay_speed_;
  
  extern uint export_step_; //export particules every x steps

  extern FLOAT scale_;

  extern bool display_grid;
  extern int display_contact;
  extern bool display_bbox;
  
  extern FLOAT eps_c_display;

  void loadConf(std::string file);
};

#endif
