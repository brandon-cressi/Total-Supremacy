#ifndef SLIDER_H
#define SLIDER_H


class Slider {
public:
    //Constructors & Deconstructors
    Slider(){} //Default Constructor
    Slider(SDL_Renderer* Renderer, std::string path, int x_, int y_,int type); //Initializes All Variables

    //Rendering & Events
    void handleEvent(SDL_Event* e);
    void render(SDL_Renderer* Renderer);

    //Accessors
    int getX() {return x;}
    int getY() {return y;}

    //Modifiers
    void setWidth(int w);
    void setHeight(int h);
    void setX(int x_) {x=x_;}
    void setY(int y_) {y=y_;}
    void setMaxX(int x_) {x_max=x_;}
    void setMinX(int x_) {x_min=x_;}
    void setMaxY(int y_) {y_max=y_;}
    void setMinY(int y_) {y_min=y_;}

private:
    Texture texture;
    bool active;
    int x,y;
    int x_mod=0;
    int y_mod=0;
    int x_max, x_min;
    int y_max, y_min;
    bool ignorex=false;
    bool ignorey=false;
};

#endif // SLIDER_H
