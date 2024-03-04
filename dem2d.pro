CONFIG       += ordered
TEMPLATE      = subdirs
SUBDIRS       = QGLViewer/QGLViewer.pro

QT_VERSION=$$[QT_VERSION]

equals (QT_MAJOR_VERSION, 5) {
	cache()
}
equals (QT_MAJOR_VERSION, 6) {
	cache()
}

QT *= xml opengl

equals (QT_MAJOR_VERSION, 5) {
	QT *= gui widgets
}
equals (QT_MAJOR_VERSION, 6) {
	QT *= gui widgets openglwidgets
}

ROOT_DIRECTORY = .
INCLUDE_DIR = $${ROOT_DIRECTORY}
LIB_DIR = $${ROOT_DIRECTORY}/QGLViewer
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
TARGET   = dem2d

SRC_DIR = src/
HEADERS  = $${SRC_DIR}*.hpp
SOURCES  = $${SRC_DIR}*.cpp

QMAKE_CXXFLAGS +=  -DTRILIBRARY -DANSI_DECLARATORS -g -fopenmp -O3 -D__MODE_DEBUG=0  -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -DOPTIM_ENABLE_EIGEN_WRAPPERS -fpermissive
LIBS += -L./QGLViewer -lGL -lgomp -fopenmp -L/usr/lib
include( ./examples.pri )

