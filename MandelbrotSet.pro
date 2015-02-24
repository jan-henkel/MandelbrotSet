#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T18:59:45
#
#-------------------------------------------------
CONFIG += c++11
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MandelbrotSet
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -Ofast

SOURCES += main.cpp\
        mandelbrotmainwindow.cpp \
    mandelbrotset.cpp \
    mathparser.cpp

HEADERS  += mandelbrotmainwindow.h \
    mandelbrotset.h \
    mathparser.h

FORMS    += mandelbrotmainwindow.ui
