#include "dem_conf.hpp"
#include <fstream>
#include "error.hpp"
#include "utils.hpp"

using namespace Eigen;

namespace dem_conf {
  VEC2 size_grid_ = VEC2(1.0, 1.0);
  VEC2I size_grid_n_ = VEC2I(200, 200); 
  FLOAT grid_spacing_ = 0; 
  FLOAT young_modulus_ = 3.537e+7;
  FLOAT poisson_ = 0.3;
  FLOAT lambda_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
  FLOAT mu_ = young_modulus_/(2*(1+poisson_));

  FLOAT damping_ = 1.0;
  FLOAT damping_t = 1.0;

  FLOAT ks_ = 5000;
  FLOAT kn_ = 10000;
  
  FLOAT dt_ = 0.01;
  FLOAT frame_step = 0.033;
  
  VEC2 gravity_ = VEC2(0.0, -9.81); 

  FLOAT mass_ = 1.0; // default mass of a particle
  
  FLOAT friction_coef_ = 1.0;

  int sm_resolution = 100;
  int sm_resolution_dist = 1000;
  int sm_resolution_display = 100;

#ifdef _WHITE
  bool wireframe_ = false;
#else
  bool wireframe_ = true;
#endif
  int replay_speed_ = 1;
  
  uint export_step_ = 1;

  FLOAT scale_ = 1;

  bool display_grid = false;
  int display_contact = 0;
  bool display_bbox = false;

  //  bool use_G = true;

  FLOAT eps_c_display = 0.0001;

  void loadConf(std::string path_file) {
    std::ifstream file(path_file.c_str());
    bool young_mod_def = false;
    bool mu_def = false;
    bool shearing_def = false;
    if (file.good()) {
      std::string line;
      while (getline(file, line)) {
	if (line.substr(0,1) == "#") {
	  INFO(2, "COMMENT: "<<line);
	} else if (line.substr(0,11) == "<grid_size>") {
	  std::istringstream s(line.substr(11));
	  for (uint i = 0; i < 2; ++i) {
	    s >> size_grid_(i);
	  }
	}  else if (line.substr(0,14) == "<grid_spacing>") {
	  std::istringstream s(line.substr(14));
	  s >> grid_spacing_;
	}  else if (line.substr(0,4) == "<mu>") {
	  std::istringstream s(line.substr(4));
	  s >> mu_;
	  mu_def = true;
	}  else if (line.substr(0,8) == "<lambda>") {
	  std::istringstream s(line.substr(8));
	  s >> lambda_;
	  mu_def = true;
	}  else if (line.substr(0,15) == "<young_modulus>") {
	  std::istringstream s(line.substr(15));
	  s >> young_modulus_;
	  young_mod_def = true;
	}  else if (line.substr(0,15) == "<poisson_ratio>") {
	  std::istringstream s(line.substr(15));
	  s >> poisson_;
	  young_mod_def = true;
	}  else if (line.substr(0,11) == "<time_step>") {
	  std::istringstream s(line.substr(11));
	  s >> dt_;
	}  else if (line.substr(0,12) == "<frame_step>") {
	  std::istringstream s(line.substr(12));
	  s >> frame_step;
	}  else if (line.substr(0,11) == "<stiffness>") {
	  std::istringstream s(line.substr(11));
	  s >> kn_;
	  ks_ = 0.5*kn_;
	}  else if (line.substr(0,4) == "<kn>") {
	  std::istringstream s(line.substr(4));
	  s >> kn_;
	}  else if (line.substr(0,4) == "<ks>") {
	  std::istringstream s(line.substr(4));
	  s >> ks_;
	}  else if (line.substr(0,9) == "<damping>") {
	  std::istringstream s(line.substr(9));
	  s >> damping_;
	}  else if (line.substr(0,11) == "<damping_t>") {
	  std::istringstream s(line.substr(11));
	  s >> damping_t;	  
	}  else if (line.substr(0,9) == "<gravity>") {
	  std::istringstream s(line.substr(9));
	  for (uint i = 0; i < 2; ++i) {
	    s >> gravity_(i);
	  }
	}  else if (line.substr(0,6) == "<mass>") {
	  std::istringstream s(line.substr(6));
	  s >> mass_;
	}  else if (line.substr(0,10) == "<friction>") {
	  std::istringstream s(line.substr(10));
	  s >> friction_coef_;
	}  else if (line.substr(0,15) == "<sm_resolution>") {
	  std::istringstream s(line.substr(15));
	  s >> sm_resolution;
	}  else if (line.substr(0,20) == "<sm_resolution_dist>") {
	  std::istringstream s(line.substr(20));
	  s >> sm_resolution_dist;
	}  else if (line.substr(0,23) == "<sm_resolution_display>") {
	  std::istringstream s(line.substr(23));
	  s >> sm_resolution_display;
	}  else if (line.substr(0,16) == "<wireframe>") {
	  std::istringstream s(line.substr(16));
	  s >> wireframe_;
	}  else if (line.substr(0,7) == "<scale>") {
	  std::istringstream s(line.substr(7));
	  s >> scale_;
	}  else if (line.substr(0,15) == "<eps_c_display>") {
	  std::istringstream s(line.substr(15));
	  s >> eps_c_display;
	}  else if (line.substr(0,14) == "<display grid>" || line.substr(0,14) == "<display_grid>") {
	  display_grid = true;
	}  else if (line.substr(0,18) == "<display contacts>" || line.substr(0,18) == "<display_contacts>") {
	  display_contact = 2;
	}  else if (line.substr(0,19) == "<display contacts+>" || line.substr(0,19) == "<display_contacts+>") {
	  display_contact = 3;
	}  else if (line.substr(0,17) == "<display network>" || line.substr(0,17) == "<display_network>") {
	  display_contact = 1;
	}  else if (line.substr(0,14) == "<display bbox>" || line.substr(0,14) == "<display_bbox>") {
	  display_bbox = true;
	} else {
	  ERROR(false, "Invalid configuration file \""<<path_file<<"\"", line);
	}
      }
      if (young_mod_def) {
	lambda_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
	mu_ = young_modulus_/(2*(1+poisson_));
	WARNING(!mu_def, "both Lame parameters and Young's modulus and/or Poisson ratio defined: using Young's modulus and Poisson ratio values",
		young_modulus_<<" "<<poisson_<<" "<<mu_<<" "<<lambda_);
      }
    } else {
      ERROR(false, "Cannot found file \""<<path_file<<"\"", "");
    }
    file.close();
    INFO(1, "Time step "<<dt_);
    INFO(1, "Damping "<<damping_);
    INFO(1, "Stiffness "<<kn_<<" "<<ks_);
    INFO(1, "Friction "<<friction_coef_);
    INFO(1, "Resolution "<<sm_resolution);
  }

  
  
} //end namespace dem_conf
