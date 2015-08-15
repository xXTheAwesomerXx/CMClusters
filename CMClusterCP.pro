#-------------------------------------------------
#
# Project created by QtCreator 2015-07-24T09:52:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CMClusterCP
TEMPLATE = app
LIBS += -LC:/OpenSSL-Win32/lib -lubsec
INCLUDEPATH += C:/OpenSSL-Win32/include
QT += network
QT += xml

SOURCES += main.cpp\
        clustermanagement.cpp \
    inputdialog.cpp \
    variables.cpp \
    tabbedmainwindow.cpp \
    clustertab.cpp \
    progressdialog.cpp

HEADERS  += clustermanagement.h \
    inputdialog.h \
    variables.h \
    tabbedmainwindow.h \
    clustertab.h \
    main.h \
    progressdialog.h

FORMS    += clustermanagement.ui \
    inputdialog.ui \
    tabbedmainwindow.ui \
    clustertab.ui \
    progressdialog.ui
