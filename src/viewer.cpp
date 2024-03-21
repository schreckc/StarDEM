
/*** This is a modified version of the original file *****/


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

#ifndef NO_GRAPHICS_

#include <QCursor>
#include <QKeyEvent>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>

#include <sstream>
#include "viewer.hpp"
#include "Sphere.hpp"
#include "error.hpp"
#include "dem_conf.hpp"
#include "Times.hpp"

using namespace std;

Viewer::~Viewer() {}

void help_parse() {
  std::cout<<"\n     *** READ MESH: Help ***\n"<<std::endl;
  std::cout<<"Synopsis: \n     .\\main <options>\n\nOptions:"<<std::endl;
  std::cout<<"     -config, -c <file.conf>\n"<<std::endl;
  std::cout<<"                         use configuration file\n"<<std::endl;
  std::cout<<"     -scene, -s <file.sc>\n"<<std::endl;
  std::cout<<"                         use scene from file.sc\n"<<std::endl;
  std::cout<<"     -rec, -record <file>"<<std::endl;
  std::cout<<"                         record snapshot at each frame from file00000.jpg\n"<<std::endl;
  std::cout<<"     -e, -export <file>"<<std::endl;
  std::cout<<"                         export particles from file0.obj (work in progress)\n"<<std::endl;
  std::cout<<"     -i, -import <file>"<<std::endl;
  std::cout<<"                         import particles from file0.obj (work in progress)\n"<<std::endl;
  std::cout<<"     -svg <name>"<<std::endl;
  std::cout<<"                         export particle shape in name_part.svg and scene in name_sc.svg \n"<<std::endl;
  std::cout<<"     -v, -view <file>"<<std::endl;
  std::cout<<"                         set the view in qglviewer using <file> \n"<<std::endl;
  std::cout<<"     -stop <end>"<<std::endl;
  std::cout<<"                         stop the animation at t = end\n"<<std::endl;
  std::cout<<"     -r, -run"<<std::endl;
  std::cout<<"                         start automatically the animation\n"<<std::endl;
   std::cout<<"     -verbose"<<std::endl;
  std::cout<<"                         enable verbose mode\n"<<std::endl;
  std::cout<<"     -h, -help"<<std::endl;
  std::cout<<"                         print help (this one)\n"<<std::endl;
  
  exit(0);
}

void Viewer::treatArguments(int argc, char **argv) {
  running_ = false;
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
    } else if (s == "-rec" || s == "-record") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help_parse();
      }
      std::cout<<"Recording in :"<<" "<<argv[i+1]<<std::endl;
      record = true;
      record_file = argv[i+1];
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
      std::cout<<"Importing from:"<<" "<<argv[i+1]<<std::endl;
      sim.setImport(argv[i+1]);
      ++i;
    } else if (s == "-v" || s == "-view") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help_parse();
      }
      INFO(0, "Setting view with:"<<" "<<argv[i+1]);
      setStateFileName(argv[i+1]);
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
    } else if (s == "-r" || s == "-run") {
      running_ = true;
    } else if (s == "-h" || s == "-help") {
      help_parse();
    } else {
      std::cerr<<"\nERROR: Unknown option\n"<<std::endl;
      help_parse();
    }
  }
}

void Viewer::animate() {
  try {     
    sim.animate();
    ++_time;
    
    if (/*(_time > 10 && sim.stopCondition())||*/_time > stop_time) {
      stopAnimation();
    }
     
    if (record) {
      std::string s0;
      if (rframe < 10) {
	s0 = "0000";
      } else if (rframe < 100) {
	s0 = "000";
      } else if (rframe < 1000) {
	s0 = "00";
      } else if (rframe < 10000) {
	s0 = "0";
      }
      std::stringstream ss;
      ss <<record_file<<s0<<rframe<<".jpg";
      std::string str = ss.str();
      saveSnapshot(QString(str.c_str()));
      ++rframe;
    }
     
  } catch (std::exception& e) {
    std::cerr << "Exception catched : " << e.what() << std::endl;
    throw;
  }
}
  
void Viewer::draw() {

  float pos[4] = {1.0, 1.0, 1.0, 0.0};
  // Directionnal light
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  // Point light
  qglviewer::Vec pos2 = light2->position();
  pos[0] = float(pos2.x);
  pos[1] = float(pos2.y);
  pos[2] = float(pos2.z);
  glLightfv(GL_LIGHT2, GL_POSITION, pos);

  // drawing simulation
  glPushMatrix();
  glScalef(dem_conf::scale_, dem_conf::scale_, dem_conf::scale_);
  sim.draw();
  glPopMatrix();
}


void Viewer::init() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Light0 is the default ambient light
  glEnable(GL_LIGHT0);

  // Light2 is a classical directionnal light
  glEnable(GL_LIGHT2);
  const GLfloat light_ambient2[4] = {0.2f, 0.2f, 0.2f, 1.0};
  const GLfloat light_diffuse2[4] = {0.8f, 0.8f, 0.8f, 1.0};
  const GLfloat light_specular2[4] = {0.2, 0.2, 0.2, 1.0};

  glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);

  light2 = new qglviewer::ManipulatedFrame();
  light2->setPosition(0.0, 0.0, -1);

    // Restore previous viewer state.
  restoreStateFromFile();
  setStateFileName(".qglviewer.xml");

  // Add custom key description (see keyPressEvent).
  setKeyDescription(Qt::Key_W, "Toggles wireframe display");
  setKeyDescription(Qt::Key_G, "Toggles display of the grid");
  setKeyDescription(Qt::Key_C, "Toggles display of the contacts (1:force network, 2: contact point and normal, 3: intersection bounding circle)");
  setKeyDescription(Qt::Key_B, "Toggles display of the bounding circles");
  setKeyDescription(Qt::Key_Backspace, "Reset simulation");
  setKeyDescription(Qt::Key_Return, "Play/Pause");
  setKeyDescription(Qt::Key_X, "Advance one frame");
  setSceneRadius(30);

  srand(time(NULL));
  
  // Opens help window
  help();
  
  glDisable(GL_CULL_FACE);

  Sphere::create_array();
  reset();
  if (running_) {
    startAnimation();
  }
  if (svg) {
    sim.exportSVG(svg_file);
  }
#ifdef _WHITE
  glClearColor(1, 1, 1 ,1);
#endif
}

void Viewer::reset() {
  sim.init();
  _time = 1;
}

void Viewer::keyPressEvent(QKeyEvent *e) {
  // Get event modifiers key
  const Qt::KeyboardModifiers modifiers = e->modifiers();

  // A simple switch on e->key() is not sufficient if we want to take state key
  // into account. With a switch, it would have been impossible to separate 'F'
  // from 'CTRL+F'. That's why we use imbricated if...else and a "handled"
  // boolean.
  bool handled = false;
  if ((e->key() == Qt::Key_W) && (modifiers == Qt::NoButton)) {
    dem_conf::wireframe_ = !dem_conf::wireframe_;
    handled = true;
  } else if ((e->key() == Qt::Key_G) && (modifiers == Qt::NoButton)) {
    dem_conf::display_grid = !dem_conf::display_grid;
    handled = true;
  } else if ((e->key() == Qt::Key_C) && (modifiers == Qt::NoButton)) {
    dem_conf::display_contact = (dem_conf::display_contact+1)%4;
    handled = true;
  } else if ((e->key() == Qt::Key_B) && (modifiers == Qt::NoButton)) {
    dem_conf::display_bbox = !dem_conf::display_bbox;
    handled = true;
  } else if ((e->key() == Qt::Key_Backspace) && (modifiers == Qt::NoButton)) {
    reset();
    handled = true;
    update();
  } else if ((e->key() == Qt::Key_O) && (modifiers == Qt::NoButton)) {
    sim.obstacles.pop_back();
    sim.obstacles.pop_back();   
    handled = true;
  } else if ((e->key() == Qt::Key_M) && (modifiers == Qt::NoButton)) {
    sim.obstacles.back()->startStop();
    handled = true;
  } else if ((e->key() == Qt::Key_H) && (modifiers == Qt::NoButton)) {
    FLOAT m = sim.obstacles.back()->getMass();
    sim.obstacles.back()->setDynamic(m+1, 0);
    handled = true;
  } else if ((e->key() == Qt::Key_Return) && (modifiers == Qt::NoButton)) {
    if (animationIsStarted()) {
      displayMessage("Stop", 500);
    } else {
      displayMessage("Play", 500);
    }
  } else if ((e->key() == Qt::Key_X) && (modifiers == Qt::NoButton)) {
    animate();
    QString text("Frame: ");
    text += QString("%1").arg(_time);
    displayMessage(text, 500);
    update();
    handled = true;
  } else if ((e->key() == Qt::Key_R) && (modifiers == Qt::NoButton)) {
    record = !record;
    handled = true;
  }

  if (!handled)
    QGLViewer::keyPressEvent(e);
}

QString Viewer::helpString() const {
  QString text("<h2>S i m p l e V i e w e r</h2>");
  text += "Use the mouse to move the camera around the object. ";
  text += "You can respectively revolve around, zoom and translate with the "
    "three mouse buttons. ";
  text += "Left and middle buttons pressed together rotate around the camera "
    "view direction axis<br><br>";
  text += "Pressing <b>Alt</b> and one of the function keys "
    "(<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
  text += "Simply press the function key again to restore it. Several "
    "keyFrames define a ";
  text += "camera path. Paths are saved when you quit the application and "
    "restored at next start.<br><br>";
  text +=
    "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
  text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save "
    "a snapshot. ";
  text += "See the <b>Keyboard</b> tab in this window for a complete shortcut "
    "list.<br><br>";
  text += "Double clicks automates single click actions: A left button double "
    "click aligns the closer axis with the camera (if close enough). ";
  text += "A middle button double click fits the zoom of the camera and the "
    "right button re-centers the scene.<br><br>";
  text += "A left button double click while holding right button pressed "
    "defines the camera <i>Revolve Around Point</i>. ";
  text += "See the <b>Mouse</b> tab and the documentation web pages for "
    "details.<br><br>";
  text += "Press <b>Escape</b> to exit the viewer.";
  return text;
}

#endif
