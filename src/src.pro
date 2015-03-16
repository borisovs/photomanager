QT += network \
    sql \
    webkit \
    xml
CONFIG += silent
TARGET = PhotoManager
TEMPLATE = app
SOURCES += main.cpp \
    photomanager.cpp \
    viewerwidget.cpp \
    messagebox.cpp \
    filemodel.cpp \
    itemobject.cpp \
    zoombutton.cpp \
    photoview.cpp
HEADERS += photomanager.h \
    viewerwidget.h \
    messagebox.h \
    filemodel.h \
    itemobject.h \
    zoombutton.h \
    photoview.h
FORMS += ../forms/photomanager.ui \
    ../forms/viewerwidget.ui \
    ../forms/messagebox.ui
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
RCC_DIR = ../build/rcc
DESTDIR = ../bin
