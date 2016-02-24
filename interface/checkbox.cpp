#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "checkbox.h"

Checkbox::Checkbox(SDL_Renderer* Renderer, std::string path, int x_, int y_) {
    texture0.loadFromFile(Renderer,"..//SDL2//ui//normal_"+path+".png");
    texture1.loadFromFile(Renderer,"..//SDL2//ui//hover_"+path+".png");
    x=x_;
    y=y_;
    state=0;
}

void Checkbox::handleEvent(SDL_Event* e) {
    int xm,ym;
    bool inside=true;
    SDL_GetMouseState(&xm,&ym);
    if(xm>x+texture0.getWidth() || xm<x || ym>y+texture0.getHeight() || ym<y) {
        inside=false;
    }
    if(!inside) {
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
                tmp=tmp;
                break;

            case SDL_MOUSEBUTTONUP:
                if(tmp) {
                    state=!state;
                }
                tmp=0;
                break;
    }
    }
    else {
        switch(e->type) {
            case SDL_MOUSEBUTTONUP:
                if(tmp=1) {
                    state=!state;
                }
                tmp=0;
                break;

            case SDL_MOUSEBUTTONDOWN:
                tmp=1;
                break;
        }
    }
}
void Checkbox::render(SDL_Renderer* Renderer) {
    if(state==0) {
        texture0.render(Renderer,x,y);
    }
    if(state==1) {
        texture1.render(Renderer,x,y);
    }
}

int Checkbox::getState() {
    return state;
}

int Checkbox::getWidth() {
    return texture0.getWidth();
}

int Checkbox::getHeight() {
    return texture0.getHeight();
}

void Checkbox::setWidth(int w) {
    texture0.setWidth(w);
    texture1.setWidth(w);
}

void Checkbox::setHeight(int h) {
    texture0.setHeight(h);
    texture1.setHeight(h);
}
