//Standard C++ Libraries
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

//SDL2 C++ Libraries
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//Custom Classes
#include "texture.h"
#include "button.h"
#include "window.h"
#include "tile.h"
#include "checkbox.h"

//Screen dimension constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int SCROLL_DEPTH=100;

//Global Variables
SDL_Renderer* Renderer = NULL;
SDL_Window* window = NULL;
SDL_Surface* ScreenSurface = NULL;

struct Mouse_Resources {
    int x,y;
    int tile_location_x, tile_location_y;
    int x_modifier=0;
    int y_modifier=0;
};

struct Terrain_Resources {
    int size=100;
    int width, height;
    std::vector<Texture> terrain_textures;
    std::vector<Tile> terrain_individual_information;
    std::vector<std::pair<std::string,std::vector<std::string>>> terrain_type_information;
};

struct Minimap_Resources {
    int size=10;
    int current_x,current_y, current_width, current_height;
    int x_modifier=0;
    int y_modifier=0;
    std::vector<Texture> minimap_textures;
    std::vector<std::pair<int,std::pair<int,int>>> minimap_information;
};

//---------Text_Functions------------------------

int loadFromRenderedText(SDL_Renderer* Renderer,  std::string textureText, TTF_Font* Font, SDL_Color textColor, int x, int y) {
    SDL_Texture* texture;
    int Width;
    int Height;
    //Get rid of preexisting texture

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( Font, textureText.c_str(), textColor );
    if( textSurface == NULL ) {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(Renderer, textSurface );
        if( texture == NULL ) {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else {
            //Get image dimensions
            Width = textSurface->w;
            Height = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    SDL_Rect renderQuad = { x, y, Width, Height };
    SDL_RenderCopy(Renderer,texture, NULL, &renderQuad );
    return Height;
}

//---------Initializations------------------------


bool initConfig() {
    bool success = true;
    std::ifstream cfg("..//SDL2//config.ini");
    if (!cfg.good()) {
        printf("Can't open config.ini.\n");
        success=false;
    }
    std::map<std::string,std::string> config; //Stores the config options in a map
    std::string store1, store2;
    while(!cfg.eof()) {
        cfg >> store1;
        if(store1[0]!='[') {
            cfg >> store2;
            config.insert(std::pair<std::string,std::string>(store1,store2));
        }
    }
    if(config.find("SCREEN_WIDTH")!=config.end()) { //Checks for SCREEN_WIDTH
        SCREEN_WIDTH=std::atoi(config.find("SCREEN_WIDTH")->second.c_str());
    }
    if(config.find("SCREEN_HEIGHT")!=config.end()) { //Checks for SCREEN_HEIGHT
        SCREEN_HEIGHT=std::atoi(config.find("SCREEN_HEIGHT")->second.c_str());
    }
    return success;
}

bool initSDL() {
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags)&imgFlags)) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        success = false;
    }
    else {
        //Get window surface
        ScreenSurface = SDL_GetWindowSurface(window);
    }
    return success;
}

bool initWindow() {
    bool success = true;
    window = SDL_CreateWindow("SDL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if( window == NULL ) {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        //Create renderer for window
        Renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
        if(Renderer == NULL){
            printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }

        else {
            //Initialize renderer color
            SDL_SetRenderDrawColor( Renderer, 0xFF, 0xFF, 0xFF, 0xFF );

            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }

            if( TTF_Init() == -1 ){
                printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                success = false;
            }
        }
    }
    return success;
}

bool initTextures(Terrain_Resources &Terrain_Resource, std::vector<Texture> &minimap_textures) {
    bool success=true;
    std::ifstream textures("..//SDL2//textures//textures.txt");
    if (!textures.good()) {
        printf("Can't open tiles.txt.\n");
        success=false;
    }
    std::string name, value, width, height, location;
    textures >> width;
    textures >> height;
    Terrain_Resource.width=std::atoi(width.c_str());
    Terrain_Resource.height=std::atoi(height.c_str());
    while(!textures.eof()) {
        textures >> name;
        textures >> value;
        textures >> location;
        Terrain_Resource.terrain_textures.push_back(Texture(Renderer, "..//SDL2//textures//"+location));
    }
    std::ifstream minimap("..//SDL2//textures//minimap.txt");
    if (!minimap.good()) {
        printf("Can't open tiles.txt.\n");
        success=false;
    }
    std::string waste;
    for(int i=0;i<5;i++) {
        minimap >> waste;
    }
    while(!minimap.eof()) {
        minimap >> name;
        minimap >> value;
        minimap >> width;
        minimap >> height;
        minimap >> location;
        minimap_textures.push_back(Texture(Renderer, "..//SDL2//textures//"+location));
    }
    return success;
}

bool initTiles(std::vector<std::pair<std::string,std::vector<std::string>>> &tiles) {
    bool success=true;
    std::ifstream f_tiles("..//SDL2//textures//tiles.txt");
    if (!f_tiles.good()) {
        printf("Can't open tiles.txt.\n");
        success=false;
    }
    std::string name;
    std::vector<std::string> resources;
    while(!f_tiles.eof()) {
        f_tiles >> name;
        if(name=="C") {
            f_tiles >> name;
            resources.push_back(name);
        }
        else {
            f_tiles >> name;
            tiles.push_back(std::make_pair(name,resources));
            resources.clear();
        }
    }
    return true;
}

//---------Map_Functions------------------------

bool map_parse(std::vector<std::pair<std::string,std::vector<std::string>>> tiles, std::vector<Tile> &map_info, std::vector<std::pair<int,std::pair<int,int>>> &tiles1, std::string location) {
    bool success=true;
    std::ifstream map(location);
    if (!map.good()) {
        printf("Can't open map.txt.\n");
        success=false;
    }
    int x=0;int y=0;
    int x_m=0; int y_m=0;
    std::string value;
    while(!map.eof()) {
        map >> value;
        if(value!="/") {
            int value_=std::atoi(value.c_str());
            map_info.push_back(Tile(tiles[value_].first,value_,x,y));
            if(value_!=1) {
                tiles1.push_back(std::make_pair(0,std::make_pair(x_m,y_m)));
            }
            else {
                tiles1.push_back(std::make_pair(1,std::make_pair(x_m,y_m)));
            }
            x+=100;
            x_m+=9;
        }
        else {
            if(x%100==0) {
                x=50;
                x_m=5;
            }
            else {
                x=0;
                x_m=0;
            }
            y+=56;
            y_m+=6;
        }
    }
    int width=30;
    int height=30;
    int co_x,co_y;
    std::vector<int> beach;
    for(int i=0; i<map_info.size(); i++) {
        beach.clear();
        if(map_info[i].returnName()=="ocean") {
            co_x=i%width;
            co_y=i/width;
            if(co_y%2==1) {
                if((co_y>0 && map_info[co_x+(co_y-1)*width].returnName()!="ocean") && (co_y>0 && co_x<30 && map_info[co_x+1+(co_y-1)*width].returnName()!="ocean")) {
                    beach.push_back(10);
                    beach.push_back(11);
                }
                else if(co_y>0 && map_info[co_x+(co_y-1)*width].returnName()!="ocean") {
                    beach.push_back(16);
                }
                else if(co_y>0 && co_x<30 && map_info[co_x+1+(co_y-1)*width].returnName()!="ocean") {
                    beach.push_back(17);
                }
                if(co_y<30 && co_x<30 && map_info[co_x+1+(co_y+1)*width].returnName()!="ocean" && co_y<30 && map_info[co_x+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(13);
                    beach.push_back(14);
                }
                else if(co_y<30 && co_x<30 && map_info[co_x+1+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(18);
                }
                else if(co_y<30 && map_info[co_x+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(19);
                }
                if(co_x>0 && map_info[co_x-1+co_y*width].returnName()!="ocean") {
                    beach.push_back(15);
                }
                if(co_x<30 && map_info[co_x+1+co_y*width].returnName()!="ocean") {
                    beach.push_back(12);
                }
            }
            else {
                if((co_y>0 && co_x>0 && map_info[co_x-1+(co_y-1)*width].returnName()!="ocean") && (co_y>0 && map_info[co_x+(co_y-1)*width].returnName()!="ocean")) {
                    beach.push_back(10);
                    beach.push_back(11);
                }
                else if(co_y>0 && co_x>0 && map_info[co_x-1+(co_y-1)*width].returnName()!="ocean") {
                    beach.push_back(16);
                }
                else if(co_y>0 && map_info[co_x+(co_y-1)*width].returnName()!="ocean") {
                    beach.push_back(17);
                }
                if(co_y<30 && map_info[co_x+(co_y+1)*width].returnName()!="ocean" && co_y<30 && co_x>0 && map_info[co_x-1+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(13);
                    beach.push_back(14);
                }
                else if(co_y<30 && map_info[co_x+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(18);
                }
                else if(map_info[co_x-1+(co_y+1)*width].returnName()!="ocean") {
                    beach.push_back(19);
                }
                if(co_x>0 && map_info[co_x-1+co_y*width].returnName()!="ocean") {
                    beach.push_back(15);
                }
                if(co_x<30 && map_info[co_x+1+co_y*width].returnName()!="ocean") {
                    beach.push_back(12);
                }
            }
            map_info[i].setBeaches(beach);
        }
    }
}

//---------Camera_Functions------------------------

void getMouseLocation(Mouse_Resources &Mouse_Resource, int width, int height) {
    int x_half=width*.5;
    int y_rect=height*.7;
    int y_tri=height-y_rect;
    int x=Mouse_Resource.x-Mouse_Resource.x_modifier; int y=Mouse_Resource.y-30-Mouse_Resource.y_modifier;
    int tmp_y=y%y_rect;
    Mouse_Resource.tile_location_y=y/(y_rect);
    if(tmp_y<=y_tri) {
        if(Mouse_Resource.tile_location_y%2==0) {
            if(x%width<=x_half) {
                float slope=(x%width)*.48;
                //left
                if(slope<(y_tri-tmp_y)) {
                    Mouse_Resource.tile_location_y-=1;
                }
            }
            else {
                float slope=((width-x%width)*.48);
                //right
                if(slope<(y_tri-tmp_y)) {
                    Mouse_Resource.tile_location_y-=1;
                }

            }
        }
        else if(Mouse_Resource.tile_location_y%2==1) {
            if((x-x_half)%width<=x_half) {
                //left
                float slope=((x-x_half)%width)*.48;
                if(slope<(y_tri-tmp_y)) {
                    Mouse_Resource.tile_location_y-=1;
                }
            }
            else {
                //right
                float slope=((width-(x-x_half)%width)*.48);
                if(slope<(y_tri-tmp_y)) {
                    Mouse_Resource.tile_location_y-=1;
                }
            }
        }
    }
    if(Mouse_Resource.tile_location_y%2==0) {
        Mouse_Resource.tile_location_x=x/width*width;
    }
    else {
        Mouse_Resource.tile_location_x=((x-x_half)/width*width)+x_half;
    }
    Mouse_Resource.tile_location_y=Mouse_Resource.tile_location_y*y_rect;
}

void close() {
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    Renderer=NULL;
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

void move_camera(Mouse_Resources &Mouse_Resource) { //this function moves the camera when the user hovers over the edge of the map
    if(Mouse_Resource.x<10){//Left Stronger
        Mouse_Resource.x_modifier+=2;
    }
    else if(Mouse_Resource.x<20){//Left
        Mouse_Resource.x_modifier+=1;
    }
    if(Mouse_Resource.x>SCREEN_WIDTH-10) {//Right Stronger
        Mouse_Resource.x_modifier-=2;
    }
    else if(Mouse_Resource.x>SCREEN_WIDTH-20) {//Right
        Mouse_Resource.x_modifier-=1;
    }
    if(Mouse_Resource.y<40 && Mouse_Resource.y>30) {//Top Stronger
        Mouse_Resource.y_modifier+=2;
    }
    else if(Mouse_Resource.y<50 && Mouse_Resource.y>30) {//Top
        Mouse_Resource.y_modifier+=1;
    }
    if(Mouse_Resource.y>SCREEN_HEIGHT-10) {//Bottom Stronger
        Mouse_Resource.y_modifier-=2;
    }
    else if(Mouse_Resource.y>SCREEN_HEIGHT-20) {//Bottom
        Mouse_Resource.y_modifier-=1;
    }
    if(Mouse_Resource.y_modifier>0) { //Prevents the user from leaving the map
        Mouse_Resource.y_modifier=0;
    }
    if(Mouse_Resource.x_modifier>0) { //Prevents the user from leaving the map
        Mouse_Resource.x_modifier=0;
    }
}

void change_zoom(std::vector<Texture> &textures,std::vector<Tile> &map_info, int &width, int &height) {
    int new_w, old_w, new_h, old_h;
    old_w=width;
    old_h=height;
    int old_y=old_h*.7;
    new_w=100*SCROLL_DEPTH/100;
    int new_y=new_w*.7*4./5.;
    int old_remainder=old_w/2;
    int new_remainder=new_w/2;
    width=new_w;
    height=new_w*4./5.;
    for(int i=0;i<map_info.size();i++) {
        if(map_info[i].returnX()%old_w==0) {
            map_info[i].setX((map_info[i].returnX()/old_w)*new_w);
            map_info[i].setY((map_info[i].returnY()/old_y)*new_y);
        }
        else {
            map_info[i].setX((map_info[i].returnX()-old_remainder)/old_w*new_w+new_remainder);
            map_info[i].setY((map_info[i].returnY()/old_y)*new_y);
        }
    }
}

int main(int argc, char* args[]) {
    Mouse_Resources Mouse_Resource;
    Terrain_Resources Terrain_Resource;
    Minimap_Resources Minimap_Resource;
    if(!initConfig()) {//Loads basic settings
        printf( "Failed to initialize config!\n" );
    }
    else {
        if(!initSDL()) {//Initializes SDL and related libraries
            printf("Failed to initialize SDL!\n");
        }
        else {
            if(!initWindow()) { //Creates a window
                printf("Failed to initialize window!\n");
            }
            else {
                if(!initTextures(Terrain_Resource, Minimap_Resource.minimap_textures)) { //Loads all textures
                    printf( "Failed to load textures!\n" );
                }
                else {
                    if(!initTiles(Terrain_Resource.terrain_type_information)) {
                        printf("Failed to load tiles!\n");
                    }
                    else {
                        //Main Initialization area

                        //Init Main System Triggers
                        bool quit = false; //initiates end of event loop
                        SDL_Event e; //event value

                        //Overlap Bool
                        bool overlap;

                        //Viewport Initialization
                        SDL_Rect screen; //the main screen viewport
                        screen.x=0;
                        screen.y=0;
                        screen.w=SCREEN_WIDTH;
                        screen.h=SCREEN_HEIGHT;

                        SDL_Rect header; //the header menu viewport
                        header.x=0;
                        header.y=0;
                        header.w=SCREEN_WIDTH;
                        header.h=30;

                        SDL_Rect header_highlight; //the header highlights viewport
                        header_highlight.x=0;
                        header_highlight.y=0;
                        header_highlight.w=SCREEN_WIDTH;
                        header_highlight.h=29;

                        SDL_Rect map; //viewport for map area
                        map.x=0;
                        map.y=30;
                        map.w=SCREEN_WIDTH;
                        map.h=SCREEN_HEIGHT-30;

                        //Window Initializations
                        Window Terrain_Info(Renderer,100,100,100,100);
                        Window Map(Renderer, 400, 200, 200, 230);

                        //Button Initializations
                        Checkbox test(Renderer,"button",0,0);
                        test.setHeight(26);
                        test.setWidth(26);

                        //TTF Initializations
                        TTF_Font* Font=NULL;
                        Font=TTF_OpenFont("../SDL2/ttf/default.ttf", 12);
                        if(Font == NULL ) {
                            printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
                        }
                        SDL_Color textColor = {0, 0, 0};

                        //Map Initialization
                        map_parse(Terrain_Resource.terrain_type_information, Terrain_Resource.terrain_individual_information,Minimap_Resource.minimap_information,"..//SDL2//map.txt");
                        SDL_SetRenderDrawBlendMode(Renderer,SDL_BLENDMODE_BLEND);

                        std::vector<std::string> resources;

                        std::string name;

                        bool resize=false;
                        bool up=false;
                        bool down=false;


                        while(!quit) {
                            name=" ";
                            overlap=false;
                            down=false;

                            //Check collisions
                            SDL_GetMouseState(&Mouse_Resource.x,&Mouse_Resource.y);
                            getMouseLocation(Mouse_Resource,Terrain_Resource.width, Terrain_Resource.height);
                            move_camera(Mouse_Resource);
                            overlap=Terrain_Info.returnInside();

                            while( SDL_PollEvent( &e ) != 0 ) {
                                const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
                                Terrain_Info.handleEvent(&e);
                                Map.handleEvent(&e);
                                test.handleEvent(&e);
                                if( e.type == SDL_QUIT ) {
                                    quit = true;
                                }
                                if(e.type==SDL_MOUSEBUTTONDOWN) {
                                    up=true;
                                }
                                if(e.type==SDL_MOUSEBUTTONUP) {
                                    up=false;
                                }
                                while(currentKeyStates[SDL_SCANCODE_UP] && currentKeyStates[SDL_SCANCODE_LSHIFT]) {
                                    resize=true;
                                    SCROLL_DEPTH+=2;
                                    break;
                                }
                                while(currentKeyStates[SDL_SCANCODE_DOWN] && currentKeyStates[SDL_SCANCODE_LSHIFT]) {
                                    resize=true;
                                    SCROLL_DEPTH-=2;
                                    break;
                                }
                                if(SCROLL_DEPTH<50) {
                                    SCROLL_DEPTH=50;
                                }
                                if(SCROLL_DEPTH>100) {
                                    SCROLL_DEPTH=100;
                                }
                                if(resize) {
                                    change_zoom(Terrain_Resource.terrain_textures,Terrain_Resource.terrain_individual_information,Terrain_Resource.width,Terrain_Resource.height);
                                }
                                resize=false;
                            }
                            //Clear screen
                            SDL_RenderClear(Renderer);
                            SDL_SetRenderDrawColor(Renderer, 70, 70, 70, 255);
                            SDL_RenderSetViewport(Renderer,&screen);
                            SDL_RenderFillRect( Renderer,&screen);

                            //Header Viewport
                            SDL_RenderSetViewport(Renderer,&header);
                            SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect( Renderer,&header);
                            SDL_SetRenderDrawColor(Renderer,70,70,70,255);
                            SDL_RenderFillRect(Renderer,&header_highlight);
                            test.render(Renderer);
                            SDL_SetRenderDrawColor(Renderer, 255,255, 255, 255);

                            //Map Viewport
                            SDL_RenderSetViewport(Renderer,&map);
                            for(int i=0;i<Terrain_Resource.terrain_individual_information.size();i++) {
                                if(Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier>=-(Terrain_Resource.size) && Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier<SCREEN_WIDTH && Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier>=(-Terrain_Resource.size) && Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier<SCREEN_HEIGHT) {
                                    Terrain_Resource.terrain_textures[Terrain_Resource.terrain_individual_information[i].returnIndex()].render(Renderer, Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier, Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier, Terrain_Resource.width, Terrain_Resource.height);
                                    if(Terrain_Resource.terrain_individual_information[i].returnName()=="ocean") {
                                        std::vector<int> beach=Terrain_Resource.terrain_individual_information[i].returnBeaches();
                                        for(int j=0; j<beach.size();j++) {
                                            Terrain_Resource.terrain_textures[beach[j]].render(Renderer, Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier, Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier, Terrain_Resource.width, Terrain_Resource.height);
                                        }
                                    }
                                    if(test.getActivate()) {
                                        Terrain_Resource.terrain_textures[9].render(Renderer, Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier, Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier, Terrain_Resource.width, Terrain_Resource.height);
                                    }
                                    if(!overlap) {
                                        if(Terrain_Resource.terrain_individual_information[i].returnY()==Mouse_Resource.tile_location_y && Terrain_Resource.terrain_individual_information[i].returnX()==Mouse_Resource.tile_location_x) {
                                            Terrain_Resource.terrain_textures[8].render(Renderer, Terrain_Resource.terrain_individual_information[i].returnX()+Mouse_Resource.x_modifier, Terrain_Resource.terrain_individual_information[i].returnY()+Mouse_Resource.y_modifier, Terrain_Resource.width, Terrain_Resource.height);
                                            name=Terrain_Resource.terrain_individual_information[i].returnName();
                                            resources=Terrain_Resource.terrain_individual_information[i].returnCommodities();
                                        }
                                    }
                                }
                            }
                            //Screen Viewport
                            SDL_RenderSetViewport(Renderer, &screen);
                            Terrain_Info.render(Renderer);
                            Map.render(Renderer);
                            SDL_Rect window0_rect=Terrain_Info.returnUsuableViewport();
                            SDL_RenderSetViewport(Renderer,&window0_rect);
                            int counter = loadFromRenderedText(Renderer,name,Font,textColor,0,0);
                            if(resources.size()>0) {
                                for(int i=0; i<resources.size();i++) {
                                    counter+=loadFromRenderedText(Renderer,resources[i],Font,textColor,0,counter);
                                }
                            }
                            resources.clear();
                            window0_rect=Map.returnUsuableViewport();
                            SDL_RenderSetViewport(Renderer,&window0_rect);
                            for(int i=0;i<Minimap_Resource.minimap_information.size();i++) {
                                Minimap_Resource.minimap_textures[Minimap_Resource.minimap_information[i].first].render(Renderer,Minimap_Resource.minimap_information[i].second.first,Minimap_Resource.minimap_information[i].second.second,Minimap_Resource.size,Minimap_Resource.size);
                            }
                            SDL_Rect place={-Mouse_Resource.x_modifier/float(SCROLL_DEPTH)*10,-Mouse_Resource.y_modifier/float(SCROLL_DEPTH)*10*79/100,map.w/float(SCROLL_DEPTH)*10*92/100.,float(map.h)/float(SCROLL_DEPTH)*10*82/100};
                            SDL_RenderDrawRect(Renderer,&place);
                            if(up) {
                                if(Mouse_Resource.x>Map.returnUsuableViewport().x && Mouse_Resource.x<Map.returnUsuableViewport().x+Map.returnWidth() && Mouse_Resource.y<Map.returnUsuableViewport().y+Map.returnHeight() && Mouse_Resource.y>Map.returnUsuableViewport().y) {
                                    Mouse_Resource.x_modifier=-(Mouse_Resource.x-Map.returnX())*float(SCROLL_DEPTH)/10;
                                    Mouse_Resource.y_modifier=-(Mouse_Resource.y-Map.returnY())*float(SCROLL_DEPTH)/10/79*100;
                                }
                            }
                            SDL_RenderPresent(Renderer);
                        }
                    }
                }
            }
        }
    //Free resources and close SDL
    close();
    return 0;
    }
}
