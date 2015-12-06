#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "slider.h"

Slider::Slider(SDL_Renderer* Renderer, std::string path, int x_, int y_,int type) {
    texture.loadFromFile(Renderer,path);
    if(type==1) {
        ignorey=true;
    }
    else if(type==2) {
        ignorex=true;
    }
    x=x_;
    y=y_;
    active=false;
    x_mod=0;
    y_mod=0;

    x_min=0;
    y_min=0;

    x_max=640;
    y_max=480;
}

void Slider::handleEvent(SDL_Event* e) {
    int xm,ym;
    bool inside=true;
    SDL_GetMouseState(&xm,&ym);
    if(xm>x+texture.getWidth() || xm<x || ym>y+texture.getHeight() || ym<y) {
        inside=false;
    }
    if(!inside) {
        if(e->type==SDL_MOUSEBUTTONUP) {
            active=false;
        }
    }
    else {
        if(e->type==SDL_MOUSEBUTTONDOWN) {
            x_mod=xm-x;
            y_mod=ym-y;
            active=true;
        }
        if(e->type==SDL_MOUSEBUTTONUP) {
            active=false;
        }
    }
    if(active) {
        if(!ignorex) {
            x=xm-x_mod;
            if(x>x_max) {
                x=x_max;
            }
            if(x<x_min) {
                x=x_min;
            }
        }
        if(!ignorey) {
            y=ym-y_mod;
            if(y>y_max) {
                y=y_max;
            }
            if(y<y_min) {
                y=y_min;
            }
        }
    }
}

void Slider::render(SDL_Renderer* Renderer) {
    texture.render(Renderer,x,y);
}
