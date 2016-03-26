#ifndef WINDOW_H
#define WINDOW_H


class Window {
public:
    //Constructors & Deconstructors
    Window(); //Default Initializer
    Window(SDL_Renderer* Renderer, int x, int y, int w, int h); //Initialize All Variables
    ~Window(); //Deallocate Memory

    //Rendering & Events
    void render(SDL_Renderer* Renderer); //Renders the window
    void handleEvent(SDL_Event* e); //Handles changes due to events

    //Accessors
    bool returnInside() {return inside;} //Return true if mouse is within window
    bool returnHide() {return hide;} //Return true if window is set to hidden
    bool returnResize() {return resize;} //Returns true if window is set to resize
    bool returnMove() {return move;} //Returns true if window is set to move
    int returnX() {return x;} //Return X coordinate of window
    int returnY() {return y;} //Return Y coordinate of window
    int returnWidth() {return width;} //Return width of window
    int returnHeight() {return height;} //Return height of window
    int returnUsuableX() {return x+1;} //Return the display part of the window
    int returnUsuableY() {return y+6;} //Return the display part of the window
    SDL_Rect returnUsuableViewport();//Return the display part of the window

    //Modifiers
    void setHide(bool hide_) {hide=hide_;} //Allows external sources to hide or unhide window

    //Miscellaneous
    void free(); //Used by deconstructor to deallocate memory

private:
    //Window Parameters
    int x, y, width, height;
    int max_w, max_h;

    //Window Settings
    bool move, resize, inside, hide;

    //Mouse Modifiers
    int mouse_x=0;
    int mouse_y=0;

    //Window Textures
    Texture tresize;
    Texture tclose;
    Texture tmin;
};

#endif // WINDOW_H
