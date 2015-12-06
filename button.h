#ifndef BUTTON_H
#define BUTTON_H

class Button {
public:
    //Constructors & Deconstructors
    Button(){state=0;x=0;y=0;} //Default Constructor
    Button(SDL_Renderer* Renderer, std::string path, int x_, int y_); //Initializes All Variables

    //Rendering & Events
    void handleEvent(SDL_Event* e);
    void render(SDL_Renderer* Renderer);

    //Accessors
    int getState();
    int getWidth();
    int getHeight();
    int getX();
    int getY();
    bool getActivate() {return activate;}
    void setActivate(bool act) {activate=act;}

    //Modifiers
    void setWidth(int w);
    void setHeight(int h);
    void setX(int x_) {x=x_;}
    void setY(int y_) {y=y_;}

private:
    Texture texture0;
    Texture texture1;
    Texture texture2;
    int state,x,y;
    bool activate=false;
};

#endif // BUTTON_H
