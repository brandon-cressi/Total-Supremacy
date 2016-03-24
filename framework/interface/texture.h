#ifndef TEXTURE
#define TEXTURE

class Texture {
    public:
        //Constructors & Deconstructors
        Texture(); //Default Constructor
        Texture(SDL_Renderer* Renderer, std::string path); //Initialize All Variables
        Texture(SDL_Texture* t);
        ~Texture(); //Deallocates Memory

        //Rendering & Events
        bool loadFromFile(SDL_Renderer* Renderer, std::string path); //Load texture from image file
        void render(SDL_Renderer* Renderer, int x, int y, int w=0, int h=0); //Renders the texture
        void renderRect(SDL_Renderer* Renderer, SDL_Rect* dstrect, SDL_Rect* srcrect);

        //Accessors
        SDL_Texture* getTexture() {return texture;} //return the texture
        int getWidth() {return width;} //Get width of the texture
        int getHeight() {return height;} //Get height of the texture

        //Modifiers
        void setWidth(int width_) {width=width_;} //Set the width of the texture
        void setHeight(int height_) {height=height_;} //Set the height of the texture
        bool setTexture(SDL_Texture* t); //Sets texture

        //Miscellaneous
        void free();//Used by deconstructor to deallocate memory

    private:
        //The texture
        SDL_Texture* texture;

        //Parameters
        int width;
        int height;
};

#endif // TEXTURE

