#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T14:02:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt5IrrKarrAss
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    irrlichtwindow.cpp

HEADERS  += mainwindow.h \
    irrlichtwindow.h

FORMS    += mainwindow.ui \
    irrlichtwindow.ui

# use c++11 standard

QMAKE_CXXFLAGS += -std=c++11

win32:INCLUDEPATH += C:\irrlicht-1.8\include
win32:LIBS += c:\irrlicht-1.8\lib\win32-gcc\libirrlicht.a
