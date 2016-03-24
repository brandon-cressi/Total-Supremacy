#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"

Texture::Texture() {
    //Initialize
    texture = NULL;
    width = 0;
    height = 0;
}

Texture::Texture(SDL_Renderer* Renderer, std::string path) {
    //Initialize
    loadFromFile(Renderer, path);
}

Texture::Texture(SDL_Texture* t) {
    free();
    int w, h;
    SDL_QueryTexture(t,NULL,NULL,&w,&h);
    width=w;
    height=h;
    texture=t;
}

Texture::~Texture() {
    //Deallocate
    free();
}

bool Texture::loadFromFile(SDL_Renderer* Renderer, std::string path) {
    //Get rid of preexisting texture
    free();
    //The final texture
    SDL_Texture* newTexture = NULL;
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 255, 127, 127 ) );
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
        if( newTexture == NULL ) {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else {
            //Get image dimensions
            width = loadedSurface->w;
            height = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    //Return success
    texture = newTexture;
    return texture != NULL;
}

bool Texture::setTexture(SDL_Texture *t) {
    bool success=true;
    free();
    int w, h;
    SDL_QueryTexture(t,NULL,NULL,&w,&h);
    width=w;
    height=h;
    texture=t;
    return success;
}

void Texture::free() {
    //Free texture if it exists
    if(texture != NULL) {
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void Texture::render(SDL_Renderer* Renderer, int x, int y, int w, int h) {
    SDL_Rect renderQuad;
    if(w==0 || h==0) {
        renderQuad = { x, y, width, height };
    }
    else {
        renderQuad = {x,y,w,h};
    }
    SDL_RenderCopy(Renderer,texture, NULL, &renderQuad );
}

void Texture::renderRect(SDL_Renderer* Renderer, SDL_Rect* dstrect, SDL_Rect* srcrect) {
    SDL_RenderCopy(Renderer,texture, srcrect, dstrect);
}
