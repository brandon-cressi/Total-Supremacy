#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "texture.h"
#include "tile.h"

Tile::Tile(std::string n) {
    name=n;
}

Tile::Tile(Tile t, int i, int x_, int y_) {
    name=t.returnName();
    mobility=t.returnMobility();
    max_capacity=t.returnCapacity();
    level=t.returnLevel();
    edges=t.returnEdges();
    below=t.returnBelow();
    index=i;
    x=x_;
    y=y_;
}

Tile::Tile(std::string n, int index_, int x_, int y_) {
    name=n;
    index=index_;
    x=x_;
    y=y_;
}
