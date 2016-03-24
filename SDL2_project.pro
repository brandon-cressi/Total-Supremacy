TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    framework/interface/window.cpp \
    framework/interface/tile.cpp \
    framework/interface/texture.cpp \
    framework/interface/checkbox.cpp \
    framework/interface/button.cpp

LIBS += -L/ -lSDL2 -lSDL2_image -lSDL2_ttf -lnoise -lboost_filesystem -lboost_system

HEADERS += \
    framework/interface/window.h \
    framework/interface/tile.h \
    framework/interface/texture.h \
    framework/interface/checkbox.h \
    framework/interface/button.h
