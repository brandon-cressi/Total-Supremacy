#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "button.h"

Button::Button(SDL_Renderer* Renderer, std::string path, int x_, int y_) {
    texture0.loadFromFile(Renderer,"..//SDL2//assets//ui//normal_"+path+".png");
    texture1.loadFromFile(Renderer,"..//SDL2//assets//ui//hover_"+path+".png");
    texture2.loadFromFile(Renderer,"..//SDL2//assets//ui//pressed_"+path+".png");
    x=x_;
    y=y_;
    state=0;
}

void Button::handleEvent(SDL_Event* e) {
        activate=false;
        int xm,ym;
        bool inside=true;
        SDL_GetMouseState(&xm,&ym);
        if(xm>x+texture0.getWidth() || xm<x || ym>y+texture0.getHeight() || ym<y) {
            inside=false;
        }
        if(!inside) {
            if(state==1 || 0) {
                state=0;
            }

            switch(e->type) {
                case SDL_MOUSEBUTTONUP:
                    if(state==2) {
                        activate=true;
                    }
                    state=0;
                    break;
            }
        }
        else {
            if(state!=2) {
                state=1;
            }
            switch(e->type) {
                case SDL_MOUSEBUTTONDOWN:
                    state=2;
                    break;

                case SDL_MOUSEBUTTONUP:
                    if(state==2) {
                        activate=true;
                    }
                    state=1;
                    break;

                case SDL_MOUSEMOTION:
                    break;
        }
    }
}

void Button::render(SDL_Renderer* Renderer) {
    if(state==0) {
        texture0.render(Renderer,x,y);
    }
    if(state==1) {
        texture1.render(Renderer,x,y);
    }
    if(state==2) {
        texture2.render(Renderer,x,y);
    }
}

int Button::getState() {
    return state;
}

int Button::getWidth() {
    return texture0.getWidth();
}

int Button::getHeight() {
    return texture0.getHeight();
}

void Button::setWidth(int w) {
    texture0.setWidth(w);
    texture1.setWidth(w);
    texture2.setWidth(w);
}

void Button::setHeight(int h) {
    texture0.setHeight(h);
    texture1.setHeight(h);
    texture2.setHeight(h);
}
