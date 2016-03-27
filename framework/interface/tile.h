#ifndef TILE_H
#define TILE_H


class Tile {
public:
    //Constructors & Deconstructors
    Tile(){;} //Default Initializer
    Tile(std::string n);
    Tile(Tile t, int i, int x_, int y_);
    Tile(std::string n, int index_, int x_, int y_); //Initialize All Variables

    void setBeaches(std::vector<int> b);

    std::string returnName() {return name;}
    int returnIndex() {return index;}
    int returnX() {return x;}
    int returnY() {return y;}
    int returnCapacity() {return max_capacity;}
    int returnMobility() {return mobility;}
    int returnLevel() {return level;}
    std::string returnBelow() {return below;}
    std::string returnEdges() {return edges;}

    void setX(int x_) {x=x_;}
    void setY(int y_) {y=y_;}
    void setCapacity(int c) {max_capacity=c;}
    void setMobility(int m) {mobility=m;}
    void setLevel(int l) {level=l;}
    void setBelow(std::string b) {below=b;}

    std::vector<std::string> returnCommodities() {return commodities;}

private:
    std::string name,below;
    int index, x, y, level;

    std::vector<std::pair<int,std::string>> resources;
    std::vector<std::string> commodities;

    int mobility; //How quickly units can move through this tile
    int current_capacity,max_capacity; //How many units this tile can support (current amount & max)
    std::string edges;


};

#endif // TILE_H
