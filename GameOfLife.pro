#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T16:48:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameOfLife
TEMPLATE = app


SOURCES += main.cpp\
        gameoflifemain.cpp \
    colonymap.cpp

HEADERS  += gameoflifemain.h \
    colonymap.h

FORMS    += gameoflifemain.ui
