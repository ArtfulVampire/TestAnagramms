#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T18:37:07
#
#-------------------------------------------------

QT       += core gui

TARGET = TestAnagramms
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++1z

QMAKE_LFLAGS_RELEASE += -static -static-libgcc



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp\
        mainwindow.cpp \
    answers.cpp

HEADERS  += mainwindow.h \
    answers.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    answers.txt
