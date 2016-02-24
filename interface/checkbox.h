#ifndef CHECKBOX_H
#define CHECKBOX_H


class Checkbox {
public:
    //Constructors & Deconstructors
    Checkbox(){state=0;x=0;y=0;} //Default Constructor
    Checkbox(SDL_Renderer* Renderer, std::string path, int x_, int y_); //Initializes All Variables

    //Rendering & Events
    void handleEvent(SDL_Event* e);
    void render(SDL_Renderer* Renderer);

    //Accessors
    int getState();
    int getWidth();
    int getHeight();
    int getX();
    int getY();
    bool getActivate() {return state;}
    void setActivate(bool act) {state=act;}

    //Modifiers
    void setWidth(int w);
    void setHeight(int h);
    void setX(int x_);
    void setY(int y_);

private:
    Texture texture0;
    Texture texture1;
    int state,x,y;
    int tmp=0;
};

#endif // CHECKBOX_H
