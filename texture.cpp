#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "button.h"

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
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

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

void Texture::free() {
    //Free texture if it exists
    if(texture != NULL) {
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void Texture::render(SDL_Renderer* Renderer, int x, int y, int _size) {
    SDL_Rect renderQuad;
    if(_size==0) {
        renderQuad = { x, y, width, height };
    }
    else {
        renderQuad = {x,y,_size,_size};
    }
    SDL_RenderCopy(Renderer,texture, NULL, &renderQuad );
}
