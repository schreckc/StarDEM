#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <list>
#include <fstream>

#include "Grid.hpp"
#include "Smarticle.hpp"
#include "Obstacle.hpp"

class Simulation {

private :
  std::string conf_file;
  std::string import_path;
  std::string export_path;
  std::string import_path_grid;
  std::string export_path_grid;
  std::string scene_path;
  bool import_;
  bool export_;
  bool import_grid;
  bool export_grid;
  bool load_conf_;
  uint nb_file_i;
  mutable uint nb_file_e;
  uint time;
  
  Grid grid;
  std::vector<Smarticle*> smarticles;

  bool pop_wall;

public :
  std::list<Obstacle*> obstacles;
  
  Simulation();
  ~Simulation();

  void init();
  void clear();
  void clearParticules();
  
  void animate();
#ifndef NO_GRAPHICS_ 
  void draw();
#endif
  void oneStep();
  bool stopCondition();
  FLOAT meanSpeed();

  void importParticules(std::ifstream & file);
  void importParticulesAll(std::ifstream & file);
  void exportParticules(std::ofstream & file) const;
  void exportParticulesAll(std::ofstream & file) const;
  void importSim();
  void exportSim() const;

  void exportSVG(std::ofstream &file) const;
  void exportSVG(std::string file_name) const;

  void setLoad(std::string s);
  void setExport(std::string s);
  void setExportB(bool b);
  void setImport(std::string s);
  void setScene(std::string s);
  void setExportGrid(std::string s);
  void setImportGrid(std::string s);
  
  void backward(uint n);
  void initPoissonSampling(uint i1, uint i2, uint j1, uint j2, uint k1, uint k2);

  void loadScene();
};



#endif
