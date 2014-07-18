######################################################################
# Automatically generated by qmake (2.01a) Sun Dec 16 15:40:21 2012
######################################################################

TEMPLATE = app
TARGET = alive
QT += core gui
QT += sql
QT += xml
QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:DEPENDPATH += "C:\gsl-1.8\lib"
#DEPENDPATH += .
win32:LIBS += "C:\gsl-1.8\lib\libgsl.a"
win32:LIBS += "C:\gsl-1.8\lib\libgslcblas.a"
win32:INCLUDEPATH += "C:\Program Files\boost\boost_1_50"
win32:INCLUDEPATH+="C:\gsl-1.8\include"
#LIBS += -static
unix:LIBS+= -lgsl -lgslcblas

# Input
HEADERS += \
datahandle.h \
mainwindow.h \
qcustomplot.h \
sqlconnection.h \
plateau.h \
calibration.h \
sqlhandle.h \
BinReader.hh \
info.h \
data.h \
doseresults.h \
tools.h \
dbsave.h \
mythread.h
FORMS += mainwindow.ui \
sqlconnection.ui \
plateau.ui \
calibration.ui \
info.ui \
doseresults.ui \
dbsave.ui
SOURCES += \
datahandle.cpp \
main.cpp \
mainwindow.cpp \
qcustomplot.cpp \
sqlconnection.cpp \
plateau.cpp \
calibration.cpp \
sqlhandle.cpp \
BinReader.cc \
info.cpp \
data.cpp \
doseresults.cpp \
tools.cpp \ 
dbsave.cpp \
mythread.cpp
