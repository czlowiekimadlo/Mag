#-------------------------------------------------
#
# Project created by QtCreator 2012-04-23T19:07:57
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -frounding-math

QT       += core gui
QT       += opengl

TARGET = Mag
TEMPLATE = app

SOURCES += main.cpp\
        src/mainwindow.cpp \
    src/viewwidget.cpp \
    src/solidobject.cpp \
    src/tetrahedron.cpp \
    src/voronoi/voronoicell.cpp \
    src/voronoi/voronoiface.cpp \
    src/voronoi/voronoiedge.cpp \
    src/voronoi/voronoiplane.cpp \
    src/voronoi/voronoivertex.cpp \
    src/voronoi/voronoihalfedge.cpp \
    src/voronoi/voronoisplitedge.cpp

HEADERS  += src/mainwindow.h \
    src/viewwidget.h \
    src/solidobject.h \
    src/tetrahedron.h \
    src/voronoi/voronoicell.h \
    src/voronoi/voronoiface.h \
    src/voronoi/voronoiedge.h \
    src/voronoi/voronoiplane.h \
    src/voronoi/voronoivertex.h \
    src/voronoi/voronoihalfedge.h \
    src/voronoi/voronoisplitedge.h

FORMS    += src/mainwindow.ui

LIBS     += -lGLU
