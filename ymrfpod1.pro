#-------------------------------------------------
#
# Project created by QtCreator 2018-10-08T17:26:45
#
#-------------------------------------------------

QT       += core gui network axcontainer serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ymrfpod1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    qvcamserver.cpp

HEADERS  += widget.h \
    qvcamserver.h

FORMS    += widget.ui

#INCLUDEPATH += D:/Matlab/extern/include\
#               D:/Matlab/extern/include/win32
#LIBS += D:/Qt/Qt5.4.2/qtexample/ymrfpod1/FLIR_LIB.lib
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -llibmx
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -lmclmcr
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -lmclmcrrt
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -llibmat
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -llibmex
#LIBS += -LD:/Matlab/extern/lib/win32/microsoft/ -llibeng

DEFINES += __MW_STDINT_H__
