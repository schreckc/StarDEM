#include "Simulation.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"
#include "WallObstacle.hpp"
#include "SphereObstacle.hpp"
#include "BoxObstacle.hpp"
#include "RBoxObstacle.hpp"
#include "OBoxObstacle.hpp"
#include "ROBoxObstacle.hpp"
#include "BallObstacle.hpp"
#include "Times.hpp"
#include "utils.hpp"
#include "Sphere.hpp"

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.hpp"

Simulation::Simulation() {
  import_ = false;
  export_ = false;
  load_conf_ = false;
  verbose = false;

  INFO(3, "Creation Simulation");
}

Simulation::~Simulation() {
  clear();
}

void Simulation::init() {
  clear();
  nb_file_i = 0;
  nb_file_e = 0;

  if (load_conf_) {
    dem_conf::loadConf(conf_file);
  }
  
  loadScene();
  if (dem_conf::grid_spacing_ < 2.1*Smarticle::getRadius()) {
    dem_conf::grid_spacing_ = 2.1*Smarticle::getRadius();
  }
  grid = Grid(dem_conf::size_grid_(0), dem_conf::size_grid_(1), dem_conf::grid_spacing_); 
  
  if (import_) {
    importSim();
  }
  
  time = 0;
  Times::TIMES->init();
}

void Simulation::clearParticules() {
  for (auto& s : smarticles) {
    delete s;
  }
  smarticles.clear();
  
}
  
void Simulation::clear() {
  INFO(3, "Clear Simulation");
  clearParticules();
  for (auto& ob : obstacles) {
    delete ob;
  }
  obstacles.clear();
}


void Simulation::oneStep() {
  


  // update position of the particles in space and in the grid
#pragma omp parallel for
  for (uint i=0; i<smarticles.size();++i) {
    smarticles[i]->animate();
  }
  grid.nextStep(smarticles);

  // Smarticles: detection collisions with obstacles
#pragma omp parallel for
  for (uint i=0; i<smarticles.size();++i) {
    Smarticle *p = smarticles[i];
    for (auto &o : obstacles) {
      p->collision(o);
    }
  }


  Times::TIMES->tick(Times::coll_detection_time_);
  // Smarticles: detection collision between smarticles inside a same grid cell
#pragma omp parallel for collapse(2)
  for (int i = 0; i < grid.i_max; ++i) {
    for (int j = 0; j < grid.j_max; ++j) {
      const std::list<Smarticle*> & cell_part = grid.getSmarticles(i, j);
      std::list<Smarticle*>::const_iterator it;
      for (it = cell_part.begin(); it != cell_part.end(); ++it) {
   	Smarticle *p = *it;
   	std::list<Smarticle*>::const_iterator it_rest = it;
   	++it_rest;
   	for (; it_rest != cell_part.end(); ++it_rest) {
   	  collision(p, *it_rest);
   	}
      }      
    }
  }

  // Smarticles: detection collision with smarticles in neighbour grid cells
#pragma omp parallel for collapse(2)
  for (int i = 0; i < grid.i_max; ++i) {
    for (int j = 0; j < grid.j_max; ++j) {  
      const std::list<Smarticle*> & cell_part = grid.getSmarticles(i, j);
      std::list<Smarticle*>::const_iterator it;
      const std::list<Smarticle*> & np1 = grid.getSmarticles(i+1, j);
      const std::list<Smarticle*> & np2 = grid.getSmarticles(i+1, j+1);
      const std::list<Smarticle*> & np3 = grid.getSmarticles(i, j+1);
      const std::list<Smarticle*> & np4 = grid.getSmarticles(i-1, j+1);	    
      for (it = cell_part.begin(); it != cell_part.end(); ++it) {
	Smarticle *p = *it;
	for (auto & n: np1) {
	  collision(p,n);
	}
	for (auto & n: np2) {
	  collision(p,n);
	}
	for (auto & n: np3) {
	  collision(p,n);
	}
	for (auto & n: np4) {
	  collision(p,n);
	}
      }      
	
    }
  }

  // Computation of the collision forces
#pragma omp parallel for 
  for (uint i=0; i<smarticles.size();++i) {
    smarticles[i]->computeCollision();
  }
  Times::TIMES->tock(Times::coll_detection_time_);    

  // Add the collision forces on each particles (not in parallele, the forces
  // detected on one smarticles affect the other ones in contact) 
  for (uint i=0; i<smarticles.size();++i) {
    smarticles[i]->addForceCollision();
  }
  
  // Smarticles: add gravity
#pragma omp parallel for
  for (uint i=0; i<smarticles.size();++i) {
    smarticles[i]->gravity();
  }


  //Obstacle
  for (auto &ob : obstacles) {
    ob->animate(time*dem_conf::dt_);
  }

  ++time;
}

// true if no smarticles is moving
bool Simulation::stopCondition() {
  bool stop = true;
  for (uint i=0; i<smarticles.size();++i) {
    stop &= !smarticles[i]->isMoving();
  }
  return stop;
}

FLOAT Simulation::meanSpeed() {
  FLOAT speed = 0;
  for (uint i=0; i<smarticles.size();++i) {
    speed += smarticles[i]->getVelocity().squaredNorm();
  }
  speed /= smarticles.size();
  return speed;
}


void Simulation::animate() {
  uint n = dem_conf::frame_step/dem_conf::dt_;    
  if (!import_) {
    // main animation loop, run n physics step
    for (uint i = 0; i < n; ++i) {
      Times::TIMES->tick(Times::simu_time_);
      oneStep();
      Times::TIMES->tock(Times::simu_time_);
      Times::TIMES->next_loop();
    }
    // for the rest_angle scenario
    if (pop_wall && time/n == 400) {
      obstacles.pop_back();
      obstacles.pop_back();
    }
  } else {
    importSim();
    //Obstacle animation
    for (uint i = 0; i < n; ++i) {
      for (auto &ob : obstacles) {
	ob->animate();
      }
    }
  }
  
  if (export_) {
    exportSim();
  }

  if (verbose) {
    std::cout<<"\n\n**************** Step: "<<time<<", frame: "<<time/n<<" ******************\n"<<std::endl;
    std::cout<<"*** Simulation times *** \n One step average: "<<std::endl;
    std::cout <<"   "<<Times::TIMES->getAverageStepTimeLastFrame(Times::simu_time_)<< "(over the last frame)"<<std::endl;
    std::cout <<"   "<<Times::TIMES->getAverageStepTime(Times::simu_time_)<<" (all time)"<<std::endl;
    std::cout <<"Time for the last frame "<<Times::TIMES->getTimeLastFrame(Times::simu_time_)<<std::endl;
   std::cout <<"Average time for a frame "<<Times::TIMES->getAverageFrameTime(Times::simu_time_)<<std::endl;
  std::cout<<"\n*** Display Time "<<Times::TIMES->getTimeLastFrame(Times::display_time_)<<std::endl;

  std::cout<<"\n*** Detection of collision times *** \n One step average: "<<std::endl;
  std::cout <<"   "<<Times::TIMES->getAverageStepTimeLastFrame(Times::coll_detection_time_)<< "(over the last frame)"<<std::endl;
    std::cout <<"   "<<Times::TIMES->getAverageStepTime(Times::coll_detection_time_)<<" (all time)"<<std::endl;
    std::cout <<"Time for the last frame "<<Times::TIMES->getTimeLastFrame(Times::coll_detection_time_)<<std::endl;
   std::cout <<"Average time for a frame "<<Times::TIMES->getAverageFrameTime(Times::coll_detection_time_)<<std::endl;
  Times::TIMES->next_frame();
  }
}

#ifndef NO_GRAPHICS_ 
void Simulation::draw() {  
  Times::TIMES->tick(Times::display_time_);
  for (auto& s : smarticles) {
    s->draw();
  }
  for (auto& ob : obstacles) {
    ob->draw();
  }
  if (dem_conf::display_contact > 0) {
    for (auto& s : smarticles) {
      s->drawContact();
    }
  }
  grid.draw(); 
  Times::TIMES->tock(Times::display_time_);
}
#endif




void Simulation::importParticules(std::ifstream & file) {
  smarticles.clear();
  uint nb_line = 0;
  std::string line;
  while (getline(file, line)) {
    ++nb_line;
    if (line.substr(0,3) == "ns ") {
      std::istringstream s(line.substr(3));
      int n = 0;
      FLOAT r = 1;
      s >> n;
      s >> r;
      
      smarticles = std::vector<Smarticle *>(n);
      getline(file, line);
      ++nb_line;
      ERROR(line.substr(0,2) == "c ", "Error importing smarticles ", line);
      s = std::istringstream(line.substr(2));
      int nb_coef;
      s >> nb_coef;
      std::vector<FLOAT> coefs = std::vector<FLOAT>(nb_coef);
      for (uint i = 0; i < nb_coef; ++i) {
	s >> coefs[i];
      }
      Smarticle::setCoefs(coefs);
      Smarticle::computeTables();
      for (int i = 0; i < n; ++i) {
	getline(file, line);
	++nb_line;
 	ERROR(line.substr(0,2) == "v ", "Error importing smarticles ", line);
	s = std::istringstream(line.substr(2));
	VEC2 v;
	s >> v(0); 
	s >> v(1);
	smarticles[i] = new Smarticle(v);
	smarticles[i]->setRadius(r);
	if (i%3 == 0) {
	  smarticles[i]->setColor(1, 0, 0);
	} else if (i%3 == 1) {
	  smarticles[i]->setColor(0, 1, 0);
	} else {
	  smarticles[i]->setColor(0.5, 0.5, 1);
	}
      }
      for (int i = 0; i < n; ++i) {
	getline(file, line);
	++nb_line;
	ERROR(line.substr(0,2) == "r ", "Error importing particles ", line);
	s = std::istringstream(line.substr(2));
	FLOAT r;
	s >> r;
	smarticles[i]->rotate(r);
      }
    } else if (line[0] == '#') {
    } else {
      WARNING(false, "Imported file possibly corrupted (line "<<nb_line<<")", line);
    }
  }

}

void Simulation::exportParticules(std::ofstream & file) const {
  FLOAT radius = 1;

  if (!smarticles.empty()) {
    radius = smarticles.front()->getRadius();
  }
  file << "# smarticles \n";
  file <<"ns "<<smarticles.size()<<" "<<radius<<"\n";
  const std::vector<FLOAT> & coefs = Smarticle::getCoefs();
  file <<"c "<<coefs.size()<<" ";
  for (auto c: coefs) {
    file <<c<<" ";
  }
  file <<"\n";
  for (auto &s : smarticles) {
    VEC2 v = s->getPosition();
    file<<"v "<<v(0)<<" "<<v(1)<<"\n";
  }
  for (auto &s : smarticles) {
    FLOAT r = s->getRotation();
    file<<"r "<<r<<"\n";
  }
}



void Simulation::importSim() {
  std::stringstream ss;
  ss <<import_path<<nb_file_i<<".obj";
  std::string str(ss.str());
  std::ifstream file(str.c_str());
  INFO(1, "Import file \""<<str<<"\"  >>"<<dem_conf::replay_speed_);
  if (file.good()) {
    clearParticules();
    importParticules(file);
    if ((int)nb_file_i >= -dem_conf::replay_speed_) {
      nb_file_i += dem_conf::replay_speed_;
    } else {
      nb_file_i = 0;
    }
  } else {
    nb_file_i = 0;
  }
  file.close();
}

void Simulation::exportSim() const {
  if (nb_file_e % dem_conf::export_step_ == 0) {
    std::stringstream ss;
    ss <<export_path<<nb_file_e/dem_conf::export_step_<<".obj";
    std::string str(ss.str());
    std::ofstream file(str.c_str());
    ERROR(file.good(), "cannot open file \""<<str<<"\"", "");
    INFO(1, "Export file \""<<str<<"\"");
    exportParticules(file);
    file.close();
  }
  ++nb_file_e;
}

void Simulation::exportSVG(std::ofstream &file) const {
  VEC2 min = 1000*grid.getMin();
  VEC2 max = 1000*grid.getMax();
  VEC2 size = max - min;
  
  file<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
  file<<"<svg\n";
  file<<"   width=\""<<size(0)<<"mm\"\n";
  file<<"   height=\""<<size(1)<<"mm\"\n";
  file<<"   viewBox=\"0 0 "<<size(0)<<" "<<size(1)<<"\"\n";
  file<<"   version=\"1.1\"\n";
  file<<"   id=\"svg5\">\n";
  file<<"  <defs\n";
  file<<"     id=\"defs2\" />\n";
  file<<"  <g\n";
  file<<"      id=\"layer1\">\n";

  uint i =0;
  for (auto ob: obstacles) {
    ob->exportSVG(file, i);
    ++i;
  }
  file<<"  </g>\n";
  file<<"</svg>";
}

void Simulation::exportSVG(std::string file_name) const {
  std::stringstream ss;
  ss<<file_name<<"_sc.svg";
  std::ofstream file;
  file.open(ss.str());
  exportSVG(file);
  file.close();

  if (!smarticles.empty()) {
    std::stringstream ssp;
    ssp<<file_name<<"_part.svg";
    std::ofstream filep;
    filep.open(ssp.str());
    smarticles.front()->exportSVG(filep);
    filep.close();
  }

}

void Simulation::setLoad(std::string s) {
  conf_file = s;
  load_conf_ = true;
}

void Simulation::setExport(std::string s) {
  export_path = s;
  export_ = true;
}

void Simulation::setExportB(bool b) {
  export_ = b;
}

void Simulation::setImport(std::string s) {
  import_path = s;
  import_ = true;
}


void Simulation::setScene(std::string s) {
  scene_path = s;
}

void Simulation::backward(uint n) {
  for (uint i = 0; i < n; ++i) {
    if (nb_file_i >= 1) {
      nb_file_i -= 1;
    } else {
      nb_file_i = 0;
      break;
    }
  }
}

void randomRotation(FLOAT & R) {
  R = (FLOAT)rand()/(FLOAT)RAND_MAX*2*M_PI;
}

void smallRandomRotation(FLOAT & R) {
  R = 0.1*(FLOAT)rand()/(FLOAT)RAND_MAX*2*M_PI;
}

void Simulation::loadScene() {
  std::ifstream file(scene_path.c_str());
  std::string line;
  INFO(3, "load SCENE "<<scene_path);
  pop_wall = false;
  while (getline(file, line)) {
    if (line.substr(0,1) == "#") {
      // comment
    } else if (line.substr(0,11) == "<obstacles>") { // obstacle
      getline(file, line);
      std::list<Motion> motions;
      FLOAT fric = dem_conf::friction_coef_;
      while (line.substr(0,12) != "</obstacles>") {
	if (line.substr(0,1) == "#") {
	  //comment
	} else if (line.substr(0,9) == " <motion>") {
	  getline(file, line);
	  Motion m;
	  while (line.substr(0,10) != " </motion>") {
	    if (line.substr(0,9) == "  <begin>") {
	      std::istringstream s(line.substr(9));
	      s >> m.begin_time;
	    } else if (line.substr(0,7) == "  <end>") {
	      std::istringstream s(line.substr(7));
	      s >> m.end_time;
	    } else if (line.substr(0,9) == "  <scale>") {
	      std::istringstream s(line.substr(9));
	      s >> m.scale;
	    } else if (line.substr(0,15) == "  <translation>") {
	      std::istringstream s(line.substr(15));
	      for (uint i = 0; i < 2; ++i) {
		s >> m.translation(i);
	      }
	      m.translation *= dem_conf::dt_;
	    } else if (line.substr(0,12) == "  <rotation>") {
	      std::istringstream s(line.substr(12));
	      s >> m.rotation;
	      m.rotation *= dem_conf::dt_;
	    } else if (line.substr(0,10) == "  <center>") {
	      std::istringstream s(line.substr(10));
	      for (uint i = 0; i < 2; ++i) {
		s >> m.center(i);
	      }
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  motions.push_back(m);
	} else if (line.substr(0,11) == " <friction>") {
	  std::istringstream s(line.substr(11));
	  s >> fric;
	} else if (line.substr(0,9) == " <sphere>") { // sphere obstacle
	  VEC2 center(0, 0);
	  FLOAT radius = 0;
	  FLOAT hr = 0.0;
	  VEC2 hn = VEC2(0, 0);
	  getline(file, line);
	  while (line.substr(0,10) != " </sphere>") {
	    INFO(3, "sphere "<<line);
	    if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      for (uint i = 0; i < 2; ++i) {
		s >> center(i);
	      }
	    } else if (line.substr(0,10) == "  <radius>") {
	      std::istringstream s(line.substr(10));
	      s >> radius;
	    } else if (line.substr(0,15) == "  <hole normal>") {
	      std::istringstream s(line.substr(15));
	      for (uint i = 0; i < 2; ++i) {
		s >> hn(i);
	      }
	    } else if (line.substr(0,15) == "  <hole radius>") {
	      std::istringstream s(line.substr(15));
	      s >> hr;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  SphereObstacle *o = new SphereObstacle(center, radius, hr, hn);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end sphere obstacle
	} else if (line.substr(0,6) == " <box>") { // box obstacle (aligne with axis)
	  VEC2 min(0, 0);
	  VEC2 max(0, 0);
	  bool out = false;
	  getline(file, line);
	  while (line.substr(0,7) != " </box>") {
	    if (line.substr(0,11) == "  <min pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 2; ++i) {
		s >> min(i);
	      }
	    } else if (line.substr(0,11) == "  <max pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 2; ++i) {
		s >> max(i);
	      }
	    } else if (line.substr(0,7) == "  <out>") {
	      out = true;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  Obstacle *o;
	  if (out) {
	    o = new OBoxObstacle(min, max);
	  } else {
	    o = new BoxObstacle(min, max);
	  }
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end box obstacle

	} else if (line.substr(0,7) == " <rbox>") { // box obstacle (not aligned with axis)
	  VEC2 pos(0, 0);
	  VEC2 n(0, 1);
	  VEC2 size(1, 1);
	  bool out = false;
	  getline(file, line);
	  while (line.substr(0,8) != " </rbox>") {
	    if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      for (uint i = 0; i < 2; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,10) == "  <normal>") {
	      std::istringstream s(line.substr(10));
	      for (uint i = 0; i < 2; ++i) {
		s >> n(i);
	      }
	    } else if (line.substr(0,8) == "  <size>") {
	      std::istringstream s(line.substr(8));
	      for (uint i = 0; i < 2; ++i) {
		s >> size(i);
	      }
	    } else if (line.substr(0,7) == "  <out>") {
	      out = true;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  Obstacle *o;
	  if (out) {
	    o = new ROBoxObstacle(pos, n, size);
	  } else {
	    o = new RBoxObstacle(pos, n, size);
	  }
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end rbox obstacle

	}  else if (line.substr(0,8) == " <plane>") { // plane obstacle
	  VEC2 pos(0.5, 0.5);
	  VEC2 dir(0, 0);
	  FLOAT l = 0;
	  getline(file, line);
	  while (line.substr(0,9) != " </plane>") {
	    if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      for (uint i = 0; i < 2; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,7) == "  <dir>") {
	      std::istringstream s(line.substr(7));
	      for (uint i = 0; i < 2; ++i) {
		s >> dir(i);
	      }
	    } else if (line.substr(0,10) == "  <length>") {
	      std::istringstream s(line.substr(10));
	      s >> l;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  PlaneObstacle *o = new PlaneObstacle(pos, dir, l);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end plane obstacle
	}  else if (line.substr(0,7) == " <wall>") { // wall obstacle (infinite length)
	  FLOAT pos = 0;
	  uint axis = 0;
	  int dir = 1;
	  getline(file, line);
	  while (line.substr(0,8) != " </wall>") {
	    if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      s >> pos;
	    } else if (line.substr(0,8) == "  <axis>") {
	      std::istringstream s(line.substr(8));
	      s >> axis;
	    } else if (line.substr(0,7) == "  <dir>") {
	      std::istringstream s(line.substr(7));
	      s >> dir;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  WallObstacle *o = new WallObstacle(axis, pos, dir);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end wall obstacle

	  // } else if (line.substr(0,11) == " <cylinder>") {
	  //   VEC2 pos(0, 0);
	  //   VEC2 dir(0, 0);
	  //   FLOAT r = 0;
	  //   getline(file, line);
	  //   while (line.substr(0,12) != " </cylinder>") {
	  //     if (line.substr(0,12) == "  <position>") {
	  //       std::istringstream s(line.substr(12));
	  //       for (uint i = 0; i < 2; ++i) {
	  // 	s >> pos(i);
	  //       }
	  //     } else if (line.substr(0,13) == "  <direction>") {
	  //       std::istringstream s(line.substr(13));
	  //       for (uint i = 0; i < 2; ++i) {
	  // 	s >> dir(i);
	  //       }
	  //     } else if (line.substr(0,7) == "  <ray>") {
	  //       std::istringstream s(line.substr(7));
	  //       s >> r;
	  //     } else {
	  //       std::cerr<<"Line not recognized in file\""<<scene_path<<"\": "<<line<<std::endl;
	  //       exit(-1);
	  //     }
	  //     getline(file, line);
	  //   }
	  //   // CylinderObstacle *o = new CylinderObstacle(pos, dir, r, 0);
	  //   // obstacles.push_back(o);
	  //   // o->setFriction(fric);
	  //   // o->setMotions(motions);
	  //   // end cylinder

	} else if (line.substr(0,7) == " <ball>") {
	  VEC2 pos(0, 0);
	  FLOAT r = 0;
	  getline(file, line);
	  while (line.substr(0,8) != " </ball>") { // ball obstacle
	    if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      for (uint i = 0; i < 2; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,10) == "  <radius>") {
	      std::istringstream s(line.substr(10));
	      s >> r;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  BallObstacle *o = new BallObstacle(pos, r);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end ball obstacle
	    
	} else {
	  std::cerr<<"Line not recognized in file\""<<scene_path<<"\": "<<line<<std::endl;
	  exit(-1);
	}
	getline(file, line);
      }
      // end obstacles
	
    } else if (line.substr(0,12) == "<particules>") {
      //srand(42);
      FLOAT rotation = 0;
      bool random = false;
      getline(file, line);
      uint nb_sub = 0;
      FLOAT m = dem_conf::mass_;
      FLOAT r = 0.0;
      FLOAT v = 40.0;
      FLOAT k = 1000;
      FLOAT d = 5;
      FLOAT xmin = 0, xmax = 0, ymin = 0, ymax = 0;
      FLOAT w=1, l=1;
      VEC2 vel(0, 0);
      std::vector<FLOAT> coefs;

      std::list<VEC2> points;
      uint nb_part;
      while (line.substr(0,13) != "</particules>") {
	if (!import_) {
	  if (line.substr(0,1) == "#") {
	    //comment
	  } else if (line.substr(0,7) == " <mass>") {
	    std::istringstream s(line.substr(7));
	    s >> m;
	  } else if (line.substr(0,9) == " <radius>") {
	    std::istringstream s(line.substr(9));
	    s >> r;
	  } else if (line.substr(0,9) == " <volume>") {
	    std::istringstream s(line.substr(9));
	    s >> v;
	  } else if (line.substr(0,12) == " <stiffness>") {
	    std::istringstream s(line.substr(12));
	    s >> k;
	  } else if (line.substr(0,10) == " <damping>") {
	    std::istringstream s(line.substr(10));
	    s >> d;
	  } else if (line.substr(0,11) == " <rotation>") {
	    std::istringstream s(line.substr(11));
	    s >> rotation;
	  } else if (line.substr(0,18) == " <random rotation>") {
	    random = true;
	  } else if (line.substr(0,11) == " <velocity>") {
	    std::istringstream s(line.substr(11));
	    for (uint i = 0; i < 2; ++i) {
	      s >> vel(i);
	    }
	  } else if (line.substr(0,11) == " <position>") {
	    std::istringstream s(line.substr(11));
	    VEC2 pos;
	    for (uint i = 0; i < 2; ++i) {
	      s >> pos(i);
	    }
	    points.push_back(pos);
	  } else if (line.substr(0,8) == " <coefs>") { // Fourier coefficients
	    std::istringstream s(line.substr(8));
	    int nb_coef;
	    s >> nb_coef;
	    coefs = std::vector<FLOAT>(nb_coef);
	    for (uint i = 0; i < nb_coef; ++i) {
	      s >> coefs[i];
	    }
	  } else if (line.substr(0,13) == " <rand coefs>") { // random coefs between -1 and 1
	    std::istringstream s(line.substr(13));
	    int nb_coef;
	    s >> nb_coef;
	    coefs = std::vector<FLOAT>(nb_coef);
	    FLOAT ss = 0;
	    for (uint i = 0; i < nb_coef; ++i) {
	      coefs[i] = 2*(FLOAT)rand()/(FLOAT)RAND_MAX - 1;
	      ss += abs(coefs[i]);
	    }
	  } else if (line.substr(0,17) == " <rand coefs pos>") { // random coefs between 0 and 1
	    std::istringstream s(line.substr(17));
	    int nb_coef;
	    s >> nb_coef;
	    coefs = std::vector<FLOAT>(nb_coef);
	    FLOAT ss = 0;
	    for (uint i = 0; i < nb_coef; ++i) {
	      coefs[i] = (FLOAT)rand()/(FLOAT)RAND_MAX;
	      ss += abs(coefs[i]);
	    }
	  } else if (line.substr(0,9) == " <cuboid>") { // poisson sampling
	    getline(file, line);
	    while (line.substr(0,10) != " </cuboid>") {
	      if (line.substr(0,5) == "  <x>") {
		std::istringstream s(line.substr(5));
		s >> xmin;
		s >> xmax;
	      } else  if (line.substr(0,5) == "  <y>") {
		std::istringstream s(line.substr(5));
		s >> ymin;
		s >> ymax;
	      } else if (line.substr(0,17) == "  <nb particules>") {
		std::istringstream s(line.substr(17));
		s >> nb_part;
	      } else {
		std::cerr<<"Line not recognized in file\""<<scene_path<<"\": "<<line<<std::endl;
		exit(-1);
	      }
	      getline(file, line);
	    }
	    w = xmax - xmin; l = ymax - ymin;
	    PoissonGenerator::PRNG prng;
	    if (!import_) {
	      std::list<VEC3> poissons;
	      poissons = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, VEC3(w, l, 0)/std::max(w, l));
	      for (auto &v: poissons) {
		points.push_back(std::max(w, l)*VEC2(v(0), v(1)) + VEC2(xmin, ymin));
	      }
	    }
	  } else if (line.substr(0,16) == " <grid_sampling>") { // staggered grid sampling
	    float density = 1.0;
	    getline(file, line);
	    while (line.substr(0,17) != " </grid_sampling>") {
	      if (line.substr(0,5) == "  <x>") {
		std::istringstream s(line.substr(5));
		s >> xmin;
		s >> xmax;
	      } else  if (line.substr(0,5) == "  <y>") {
		std::istringstream s(line.substr(5));
		s >> ymin;
		s >> ymax;
	      } else if (line.substr(0,11) == "  <density>") { // n particles per unit
		std::istringstream s(line.substr(11));
		s >> density;
	      } else {
		std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
		exit(-1);
	      }
	      getline(file, line);
	    }
	    if (!import_) {
	      float dx = 1.0/density;
	      float coef = 0.5*dx - r;
	      for (float xc = xmin; xc < xmax; xc += dx) {
		for (float yc = ymin; yc < ymax; yc += dx) {
		  float rx = coef*rand()/RAND_MAX;
		  float ry = coef*rand()/RAND_MAX;
		  points.push_back(VEC2(xc+rx, yc+ry));
		}
	      }
	    }	    
	    //end grid sampling	    
	  } else {
	    std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	    exit(-1);
	  }
	}
	getline(file, line);	
      }
      nb_part = points.size();
      uint i = 0;
      if (nb_part > 0) {
	Smarticle::setMass(m);
	Smarticle::setRadius(r);
	Smarticle::setArea(v);
	Smarticle::setCoefs(coefs);

	Smarticle::computeTables();
	Smarticle::computeArea();
	Smarticle::computeNumericalArea();

	// Creation of the smarticles
	for (auto &v: points) {
	  Smarticle *s = new Smarticle(v, m, vel, VEC2(0, 1));
	  if (random) {
	    rotation = (FLOAT)rand()/(FLOAT)RAND_MAX*2*M_PI;
	  }
	  s->rotate(rotation);
#ifdef _WHITE
	  if (i%3 == 0) {
	    s->setColor(0.7, 0.4, 1);
	  } else if (i%3 == 1) {
	    s->setColor(0.1, 0.7, 0.9);
	  } else {
	    s->setColor(0.5, 0.5, 1);
	  }
#else
	  if (i%3 == 0) {
	    s->setColor(1, 0, 0);
	  } else if (i%3 == 1) {
	    s->setColor(0, 1, 0);
	  } else {
	    s->setColor(0.5, 0.5, 1);
	  }
#endif	
	  smarticles.push_back(s);
	  ++i;
	}
      }
	      
      if (!smarticles.empty()) {  // distance and error distance plotting
	// std::string name = "flower";
	// std::string option = "";
	
	// std::ofstream file(name+".svg");
	// smarticles.front()->exportSVG(file);
	// file.close();

	// TR("plotting "<<name<<"_dist_"<<option<<".dat");
	//  file = std::ofstream(name+"_dist_"+option+".dat");
	//  smarticles.front()->plotDistance(file, 0);
	//  file.close();
	
	//  file = std::ofstream(name+"_rdist_"+option+".dat");
	//  smarticles.front()->plotDistance(file, 1);
	//  file.close();

	//  file = std::ofstream(name+"_ndist_"+option+".dat");
	//  smarticles.front()->plotDistance(file, 2);
	//  file.close();

	// file = std::ofstream(name+"_err_"+option+".dat");
	// smarticles.front()->plotDistance(file, 3);
	// file.close();

	// file = std::ofstream(name+"_rerr_"+option+".dat");
	// smarticles.front()->plotDistance(file, 4);
	// file.close();
      }

      INFO(1, "Creating "<<smarticles.size()<<" smarticles (m="<<m<<")");
      //end particules
    }  else if (line.substr(0,5) == "<pop>") {
      pop_wall = true;
    } else {
      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
      exit(-1);
    }
      
  } //end main loop

    // std::ofstream file_sc("scene.svg");
    // exportSVG(file_sc);
    // file_sc.close();

  // TEST DYNAMIC OBSTACLES
  
  // BallObstacle *o = new BallObstacle(VEC2(300,350), 20);
  // o->setDynamic(10, 15);
  // o->setFriction(0);
  // obstacles.push_back(o);

  // OBoxObstacle *o = new OBoxObstacle(VEC2(240, 610),VEC2(360, 620));
  // o->setDynamic(10, 0);
  // o->setFriction(0);
  // obstacles.push_back(o);

  // ROBoxObstacle *o = new ROBoxObstacle(VEC2(300, 150),VEC2(0, 1), VEC2(60, 3));
  // o->setDynamic(0, 1000);
  // o->setFriction(0);
  // obstacles.push_back(o);
    
} // end loadScene

