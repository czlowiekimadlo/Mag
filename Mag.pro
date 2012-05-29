#-------------------------------------------------
#
# Project created by QtCreator 2012-04-23T19:07:57
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

TARGET = Mag
TEMPLATE = app

SOURCES += main.cpp\
        src/mainwindow.cpp \
    src/viewwidget.cpp \
    src/solidobject.cpp

HEADERS  += src/mainwindow.h \
    src/viewwidget.h \
    src/solidobject.h

FORMS    += src/mainwindow.ui

LIBS     += -lGLU
