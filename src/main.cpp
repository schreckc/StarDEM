/* 
 * File: error.hpp
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

#ifdef NO_GRAPHICS_ 

#include "Simulation.hpp"
#include "error.hpp"
#include "Times.hpp"

Simulation sim;
bool svg;
std::string svg_file;
uint stop_time;


void help_parse() {
  std::cout<<"\n     *** READ MESH: Help ***\n"<<std::endl;
  std::cout<<"Synopsis: \n     .\\main <options>\n\nOptions:"<<std::endl;
  std::cout<<"     -config, -c <file.conf>\n"<<std::endl;
  std::cout<<"                         use configuration file\n"<<std::endl;
  std::cout<<"     -scene, -s <file.sc>\n"<<std::endl;
  std::cout<<"                         use scene from file.sc\n"<<std::endl;
  std::cout<<"     -e, -export <file>"<<std::endl;
  std::cout<<"                         export particles to file0.obj (work in progress)\n"<<std::endl;
  std::cout<<"     -i, -import <file>"<<std::endl;
  std::cout<<"                         import particles from file0.obj (work in progress)\n"<<std::endl;
  std::cout<<"     -svg <name>"<<std::endl;
  std::cout<<"                         export particle shape in name_part.svg and scene in name_sc.svg \n"<<std::endl;
  std::cout<<"     -stop <end>"<<std::endl;
  std::cout<<"                         stop the animation at t = end\n"<<std::endl;
  std::cout<<"     -verbose"<<std::endl;
  std::cout<<"                         enable verbose mode\n"<<std::endl;
  std::cout<<"     -h, -help"<<std::endl;
  std::cout<<"                         print help (this one)\n"<<std::endl;
  
  exit(0);
}

void treatArguments(int argc, char **argv) {

  bool record_data = false;
  std::string data_file = "";
   for (int i = 1;  i < argc; ++i) {
     std::string s(argv[i]);
     if (s == "-config" || s == "-c") {
       if (argc < i + 2) {
   	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
   	help_parse();
       }
       INFO(0, "Loading config file:"<<" "<<argv[i+1]);
       sim.setLoad(argv[i+1]);
       ++i;
     } else if (s == "-scene" || s == "-s") {
       if (argc < i + 2) {
	 std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	 help_parse();
       }
       INFO(0, "Loading scene file:"<<" "<<argv[i+1]);
       sim.setScene(argv[i+1]);
       ++i;
     } else if (s == "-svg") {
	if (argc < i + 2) {
	  std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	  help_parse();
	}
	std::cout<<"Saving svg scene and svg particle shape in :"<<" "<<argv[i+1]<<std::endl;
	svg = true;
	svg_file = argv[i+1];
	++i;
     } else if (s == "-e" || s == "-export") {
	if (argc < i + 2) {
	  std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	  help_parse();
	}
	std::cout<<"Exporting in :"<<" "<<argv[i+1]<<std::endl;
	sim.setExport(argv[i+1]);
	++i;
     } else if (s == "-i" || s == "-import") {
	if (argc < i + 2) {
	  std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	  help_parse();
	}
	std::cout<<"Importing in :"<<" "<<argv[i+1]<<std::endl;
	sim.setImport(argv[i+1]);
	++i;
     } else if (s == "-stop") {
       if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help_parse();
      }
       std::cout<<"Stop at t = "<<argv[i+1]<<std::endl;
       stop_time = atoi(argv[i+1]);
       ++i;
     } else if (s == "-verbose") {
       std::cout<<"Verbose Mode"<<std::endl;
       sim.verbose = true;
     } else if (s == "-h" || s == "-help") {
       help_parse();
     } else {
       std::cerr<<"\nERROR: Unknown option\n"<<std::endl;
       help_parse();
     }
   }

}


int main(int argc, char **argv) {
  uint _time = 1;
  stop_time = 2000;
  
  srand(time(NULL));
  
  treatArguments(argc, argv);

  sim.init();
    
  if (svg) {
    sim.exportSVG(svg_file);
  }
  FLOAT all_time = 0;
  FLOAT sumd = 0;
  while (true) {
    try {
    if (_time > stop_time) {
      std::exit(0);
    }

    sim.animate();     
    ++_time;
     
    } catch (std::exception& e) {
      std::cerr << "Exception catched : " << e.what() << std::endl;
      throw;
    }
    
  }
}

#else 

#include "viewer.hpp"
#include <qapplication.h>

int main(int argc, char **argv) {
  QApplication application(argc, argv);
  Viewer viewer;

  viewer.setWindowTitle("DEM Viewer");
  viewer.treatArguments(argc, argv);
 
  viewer.show();

  return application.exec();
}

#endif
