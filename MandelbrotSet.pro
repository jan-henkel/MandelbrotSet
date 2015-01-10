#-------------------------------------------------
#
# Project created by QtCreator 2014-12-31T18:59:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MandelbrotSet
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

SOURCES += main.cpp\
        mandelbrotmainwindow.cpp \
    mandelbrotset.cpp \
    mathparser.cpp \
    complex.cpp

HEADERS  += mandelbrotmainwindow.h \
    mandelbrotset.h \
    mathparser.h \
    complex.h

FORMS    += mandelbrotmainwindow.ui
