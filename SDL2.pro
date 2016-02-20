TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    texture.cpp \
    button.cpp \
    tile.cpp \
    window.cpp \
    checkbox.cpp
LIBS += -L/usr/include -lSDL2 -lSDL2_image -lSDL2_ttf -lnoise

HEADERS += \
    texture.h \
    button.h \
    tile.h \
    window.h \
    checkbox.h
