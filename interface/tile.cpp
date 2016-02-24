#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "tile.h"

Tile::Tile(std::string n, int index_, int x_, int y_) {
    name=n;
    index=index_;
    if(name=="desert") index=0;
    x=x_;
    y=y_;
}

void Tile::setBeaches(std::vector<int> b) {
    beaches=b;
}
