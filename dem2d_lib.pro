CONFIG       += ordered
TEMPLATE      = subdirs
SUBDIRS       = QGLViewer designerPlugin


ROOT_DIRECTORY = .
INCLUDE_DIR = $${ROOT_DIRECTORY}
OBJECTS_DIR = obj/
EIGEN=/usr/include/eigen3
PYTHON=/usr/include/python3.10

# Include path, taking QGLViewer path prefix into account
INCLUDEPATH *= $${INCLUDE_DIR}
               DEPENDPATH  *= $${INCLUDE_DIR}

               INCLUDEPATH += $${EIGEN}
               INCLUDEPATH += $${OPTIM}
               INCLUDEPATH += $${PYTHON}
               INCLUDEPATH += ./voro++-0.4.6/src

CONFIG -= debug debug_and_release
CONFIG *= release

TEMPLATE = lib
TARGET   = dem2d_functions

SRC_DIR = src/
HEADERS  = $${SRC_DIR}*.hpp
HEADERS  += $${SRC_DIR}*.h
SOURCES  = $${SRC_DIR}*.cpp

QMAKE_CXXFLAGS += -fPIC -DTRILIBRARY -DANSI_DECLARATORS -g -fopenmp -O3 -D__MODE_DEBUG=0 -DNO_GRAPHICS_ -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -DOPTIM_ENABLE_EIGEN_WRAPPERS $(python3 -m pybind11 --includes)
           LIBS +=   -L./voro++-0.4.6/src -lvoro++ -lpthread -lgomp -fopenmp -lSDL2 -lSDL2_image -L/usr/lib -fPIC
# -loptim -lblas
include( ./examples.pri )
