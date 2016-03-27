
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "window.h"

Window::Window() {
    x=0;y=0;width=0;height=0;move=0;resize=0;inside=false;hide=false;
}

Window::Window(SDL_Renderer* Renderer, int x_, int y_, int w_, int h_) {
    tresize.loadFromFile(Renderer, "..//SDL2//assets//ui//resize.png");
    tclose.loadFromFile(Renderer,"..//SDL2//assets//ui//close.png");
    tmin.loadFromFile(Renderer,"..//SDL2//assets//ui//min.png");
    x=x_;
    y=y_;
    width=w_;
    height=h_;
    resize=false; move=false; inside=false; hide=false;
}

Window::~Window() {
    free();
}

void Window::render(SDL_Renderer *Renderer) {
    if(!hide) {
        SDL_Rect window_background;
        window_background.x=x;window_background.y=y;window_background.w=width;window_background.h=height;

        SDL_Rect window_heading;
        window_heading.x=x; window_heading.y=y; window_heading.w=width; window_heading.h=6;

        SDL_Rect window_heading2;
        window_heading2.x=x+1; window_heading2.y=y+1; window_heading2.w=width-2; window_heading2.h=4;

        SDL_Rect window_border_bottom;
        window_border_bottom.x=x; window_border_bottom.y=y+height-1; window_border_bottom.w=width; window_border_bottom.h=1;

        SDL_Rect window_border_right;
        window_border_right.x=x+width-1; window_border_right.y=y; window_border_right.w=1; window_border_right.h=height;

        SDL_Rect window_border_left;
        window_border_left.x=x; window_border_left.y=y; window_border_left.w=1; window_border_left.h=height;

        SDL_SetRenderDrawColor(Renderer,255,255,255,150);
        SDL_RenderFillRect(Renderer,&window_background);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(Renderer,&window_heading);
        SDL_RenderFillRect(Renderer,&window_border_right);
        SDL_RenderFillRect(Renderer,&window_border_bottom);
        SDL_RenderFillRect(Renderer,&window_border_left);
        SDL_SetRenderDrawColor(Renderer,70,70,70,255);
        SDL_RenderFillRect(Renderer,&window_heading2);
        SDL_SetRenderDrawColor(Renderer, 255,255, 255, 255);
        tresize.render(Renderer,x+width-20, y+height-20);
        tclose.render(Renderer,x+width-20,y);
        tmin.render(Renderer,x+width-40,y);
    }
}

void Window::handleEvent(SDL_Event *e) {
    int xm,ym;
    SDL_GetMouseState(&xm,&ym);
    if(hide) {
        return;
    }
    if(xm<x+width && xm>x && ym<y+height && ym>y) {
        inside=true;
    }
    else {
        inside=false;
    }
    if(move) {
        switch(e->type) {
            case SDL_MOUSEMOTION:
                x=xm-mouse_x;
                y=ym-mouse_y;
                break;

            case SDL_MOUSEBUTTONUP:
                move=false;
                mouse_y=0;
                mouse_x=0;
                break;
        }
    }
    else if(resize) {
        switch(e->type) {
            case SDL_MOUSEMOTION:
                if(xm-x+mouse_y>40) {
                    width=xm-x+mouse_x;
                }
                if(ym-y+mouse_y>26) {
                    height=ym-y+mouse_y;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                resize=false;
                mouse_y=0;
                mouse_x=0;
                break;
        }
    }
    else if(xm<x+width && xm>x+width-20 && ym<y+6 && ym>y) {
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
                free();
                break;
        }
    }
    else if(xm<=x+width-20 && xm>x+width-40 && ym<y+6 && ym>y) {
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
                hide=true;
                break;
        }
    }
    else if(xm<x+width && xm>x && ym<y+6 && ym>y) {
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
                move=true;
                mouse_x=xm-x;
                mouse_y=ym-y;
                break;
        }
    }
    else if(xm<x+width && xm>x+width-20 && ym<y+height && ym>y+height-20) {
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
                resize=true;
                mouse_x=x+width-xm;
                mouse_y=y+height-ym;
                break;
        }
    }
}

SDL_Rect Window::returnUsuableViewport() {
    SDL_Rect tmp;
    tmp.x=x+1; tmp.y=y+6; tmp.w=width-2; tmp.h=height-7;
    return tmp;
}

void Window::free() {
    x=0;
    y=0;
    width=0;
    height=0;
    tresize.~Texture();
    tclose.~Texture();
    tmin.~Texture();
}
