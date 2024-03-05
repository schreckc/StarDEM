CONFIG       += ordered
TEMPLATE      = subdirs
SUBDIRS       = QGLViewer designerPlugin


ROOT_DIRECTORY = .
INCLUDE_DIR = $${ROOT_DIRECTORY}
OBJECTS_DIR = obj/
EIGEN=/usr/include/eigen3

# Include path, taking QGLViewer path prefix into account
INCLUDEPATH *= $${INCLUDE_DIR}
               DEPENDPATH  *= $${INCLUDE_DIR}

               INCLUDEPATH += $${EIGEN}
               INCLUDEPATH += $${OPTIM}

CONFIG -= debug debug_and_release
CONFIG *= release

TEMPLATE = app
TARGET   = dem2d_ng

SRC_DIR = src/
HEADERS  = $${SRC_DIR}*.hpp
HEADERS  += $${SRC_DIR}*.h
SOURCES  = $${SRC_DIR}*.cpp

QMAKE_CXXFLAGS += -g -fopenmp -O3 -D__MODE_DEBUG=0 -DNO_GRAPHICS_ -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -DOPTIM_ENABLE_EIGEN_WRAPPERS
LIBS += -lpthread -lgomp -fopenmp -lSDL2 -lSDL2_image -L/usr/libs
include( ./examples.pri )
