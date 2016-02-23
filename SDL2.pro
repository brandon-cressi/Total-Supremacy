TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    interface/button.cpp \
    interface/checkbox.cpp \
    interface/texture.cpp \
    interface/tile.cpp \
    interface/window.cpp \
    main.cpp

LIBS += -L/framework -lSDL2 -lSDL2_image -lSDL2_ttf -lnoise

HEADERS += \
    interface/button.h \
    interface/checkbox.h \
    interface/texture.h \
    interface/tile.h \
    interface/window.h
