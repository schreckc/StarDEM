/****************************************************************************

 Copyright (C) 2002-2014 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.7.2.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License 
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain 
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

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
     } else if (s == "-h" || s == "-help") {
       std::cout<<"help"<<std::endl;
       help_parse();
     } else {
       std::cerr<<"\nERROR: Unknown option\n"<<std::endl;
       help_parse();
     }
   }

}


int main(int argc, char **argv) {
  uint _time = 1;

  uint stop_time = 2000;
  uint count = 0;
  std::list<FLOAT> densities;
  
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
  // Read command lines arguments.
  QApplication application(argc, argv);

  // Instantiate the viewer.
  Viewer viewer;

  viewer.setWindowTitle("DEM Viewer");
  viewer.treatArguments(argc, argv);

  
  // Make the viewer window visible on screen.
  viewer.show();

  // Run main loop.
  return application.exec();
}

#endif
