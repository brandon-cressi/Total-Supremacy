#ifndef TILE_H
#define TILE_H


class Tile {
public:
    //Constructors & Deconstructors
    Tile(){;} //Default Initializer
    Tile(std::string n, int index_, int x_, int y_); //Initialize All Variables

    void setBeaches(std::vector<int> b);

    std::string returnName() {return name;}
    int returnIndex() {return index;}
    int returnX() {return x;}
    int returnY() {return y;}
    void setX(int x_) {x=x_;}
    void setY(int y_) {y=y_;}
    std::vector<std::string> returnCommodities() {return commodities;}
    std::vector<int> returnBeaches() {return beaches;}

private:
    std::string name;
    int index, x, y;
    std::vector<int> beaches;

    std::vector<std::pair<int,std::string>> resources;
    std::vector<std::string> commodities;

    int movespeed; //How quickly units can move through this tile
    int current_capacity,max_capacity; //How many units this tile can support (current amount & max)


};

#endif // TILE_H
