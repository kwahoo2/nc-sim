#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T18:51:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = nc-sim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    decoder.cpp \
    serialdriver.cpp

HEADERS  += mainwindow.h \
    decoder.h \
    serialdriver.h

FORMS    += mainwindow.ui

CONFIG += c++11
