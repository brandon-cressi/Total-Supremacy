//Standard C++ Libraries
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>

//SDL2 C++ Libraries

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//boost C++ Libraries

#include <boost/filesystem.hpp>

//Custom Interface Classes
#include "framework/interface/texture.h"
#include "framework/interface/button.h"
#include "framework/interface/window.h"
#include "framework/interface/tile.h"
#include "framework/interface/checkbox.h"

//Screen dimension constants (will default to 640x480 if none are defined in config.ini
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int SCROLL_DEPTH=100;

//Global Variables
SDL_Renderer* Renderer = NULL;
SDL_Window* window = NULL;
SDL_Surface* ScreenSurface = NULL;

struct Mouse_Resources {
    int x,y; //current x and y coordinates
    int tile_location_x, tile_location_y; //current tile coordinates
    int x_modifier=0; //x scroll modifier
    int y_modifier=0; //y scroll modifier
};

struct Terrain_Resources {
    int size=100;
    int width=50;
    int height=40;
    std::vector<Tile> terrain_individual_information;
    std::vector<std::pair<std::string,std::vector<std::string>>> terrain_type_information;
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
    std::ifstream cfg("..//SDL2//config.ini");
    if (!cfg.good()) {
        printf("Can't open config.ini.\n");
        return false;
    }
    std::map<std::string,std::string> config; //Stores the config options in a map
    std::string parameter, value;
    while(!cfg.eof()) {
        cfg >> parameter;
        if(parameter[0]!='[') {
            cfg >> value;
            config.insert(std::pair<std::string,std::string>(parameter,value));
        }
    }
    if(config.find("SCREEN_WIDTH")!=config.end()) { //Checks for SCREEN_WIDTH
        SCREEN_WIDTH=std::atoi(config.find("SCREEN_WIDTH")->second.c_str());
    }
    if(config.find("SCREEN_HEIGHT")!=config.end()) { //Checks for SCREEN_HEIGHT
        SCREEN_HEIGHT=std::atoi(config.find("SCREEN_HEIGHT")->second.c_str());
    }
    return true;
}

bool initSDL() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags)&imgFlags)) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    else {
        //Get window surface
        ScreenSurface = SDL_GetWindowSurface(window);
    }
    return true;
}

bool initWindow() {
    window = SDL_CreateWindow("SDL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if( window == NULL ) {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    else {
        //Create renderer for window
        Renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
        if(Renderer == NULL){
            printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
            return false;
        }

        else {
            //Initialize renderer color
            SDL_SetRenderDrawColor( Renderer, 0xFF, 0xFF, 0xFF, 0xFF );

            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                return false;
            }

            if( TTF_Init() == -1 ){
                printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                return false;
            }
        }
    }
    return true;
}

bool initTextures(std::map<std::string,std::vector<Texture>> &textures) {
    boost::filesystem::path path("..//SDL2_project//assets//textures");
    boost::filesystem::directory_iterator b(path), e;
    std::vector<boost::filesystem::path> directory(b, e);
    std::map<std::string,std::vector<Texture>> alltextures;
    std::vector<Texture> tmp;
    for(int i=0; i<directory.size();i++) {
        b=boost::filesystem::directory_iterator(directory[i]), e;
        std::vector<boost::filesystem::path> subtexture(b, e);
        std::string s=directory[i].string();
        s.replace(s.begin(),s.begin()+path.size()+1,"");
        std::sort(subtexture.begin(),subtexture.end());
        for(int j=0;j<subtexture.size();j++) {
            if(subtexture[j].extension().string()==".png") {
                tmp.push_back(Texture(Renderer,subtexture[j].string()));
            }
        }
        alltextures.insert(std::pair<std::string,std::vector<Texture>>(s,tmp));
        tmp.clear();
    }
    textures=alltextures;
    return true;
}

bool initTiles(std::map<std::string,Tile> &alltiles) {
    std::ifstream f_tiles("../SDL2_project/assets/tilesnew.txt");
    if (!f_tiles.good()) {
        printf("Can't open tiles.txt.\n");
        return false;
    }
    std::string buffer, name;
    while(!f_tiles.eof()) {
        f_tiles>>buffer; 
        if(buffer[0]=='<') {
            name=buffer;
            name.replace(name.begin(),name.begin()+1,"");
            alltiles.insert(std::pair<std::string,Tile>(name,Tile(name)));
        }
        else if(buffer[0]=='>') {
            buffer.replace(buffer.begin(),buffer.begin()+1,"");
            if(buffer=="capacity") {
                f_tiles>>buffer;
                alltiles.find(name)->second.setCapacity(std::atoi(buffer.c_str()));
            }
            else if(buffer=="mobility") {
                f_tiles>>buffer;
                alltiles.find(name)->second.setMobility(std::atoi(buffer.c_str()));
            }
            else if(buffer=="level") {
                f_tiles>>buffer;
                alltiles.find(name)->second.setLevel(std::atoi(buffer.c_str()));
            }
            else if(buffer=="below") {
                f_tiles>>buffer;
                alltiles.find(name)->second.setBelow(buffer);
            }
        }
    }
    return true;
}

//---------Map_Functions------------------------

bool map_parse(std::map<std::string,Tile> alltiles, std::vector<Tile> &map_info, std::string location, std::map<std::string,std::vector<Texture>> &textures1) {
    int w, h;
    std::ifstream map(location);
    if (!map.good()) {
        printf("Can't open map.txt.\n");
        return false;
    }
    int x=0;int y=0;
    std::map<int,std::string> maps;
    std::string value,name;
    map >> w;
    map >> h;
    while(!map.eof()) {
        map >> value;
        if(value[0]=='>') {
            value.replace(value.begin(),value.begin()+1,"");
            name=value;
            map>>value;
            maps.insert(std::pair<int,std::string>(std::atoi(name.c_str()),value));
        }
        else if(value!="/") {
            int value_=std::atoi(value.c_str());
            std::string value1= maps.find(value_)->second;
            int random=rand()%(textures1.find(value1)->second.size());
            Tile t(alltiles.find(value1)->second,random,x,y);
            map_info.push_back(t);
            x+=50;
        }
        else {
            if(x%100==0) {
                x=25;
            }
            else {
                x=0;
            }
            y+=28;
        }
    }
    return true;
}


//---------Camera_Functions------------------------

//This function returns the tile location that the mouse is currently hovering over. Each hexagon is split into rectangles
//to find the general mouse position. After that, each triangle of the hexagon is checked and the general mouse position is
//fixed.

void getMouseLocation(Mouse_Resources &Mouse_Resource, int width, int height) {
    int x_half=width*.5; //half the tile width
    int y_rect=height*.7; //70% of the tile height
    int y_tri=height-y_rect; //top 30% of the tile
    int x=Mouse_Resource.x-Mouse_Resource.x_modifier; int y=Mouse_Resource.y-30-Mouse_Resource.y_modifier; //get mouse x,y
    int tmp_y=y%y_rect; //count how much remain after 70%s go into the y coordinate of mouse
    Mouse_Resource.tile_location_y=y/(y_rect); //set y tile location to remainder of 70%s
    //these if statements deal with the triangle portion of the hexagons
    if(Mouse_Resource.tile_location_y%2==0) {  //even row =(when row is further left)
        if(x%width<=x_half) { //left half
            float line=(x%width)*.48;//slope of hex triangle is .48, used to calculate line
            if(line<(y_tri-tmp_y)) {//if mouse position is above the line, the tile position is corrected
                Mouse_Resource.tile_location_y-=1;
            }
        }
        else { //right half
            float slope=((width-x%width)*.48); //slope of hex triangle is .48, used to calculate line
            if(slope<(y_tri-tmp_y)) {//if mouse position is above the line, the tile position is corrected
                Mouse_Resource.tile_location_y-=1;
            }

        }
    }
    else if(Mouse_Resource.tile_location_y%2==1) { //odd row =(when row is further right)
        if((x-x_half)%width<=x_half) { //left half
            float slope=((x-x_half)%width)*.48; //slope of hex triangle is .48, used to calculate line
            if(slope<(y_tri-tmp_y)) {//if mouse position is above the line, the tile position is corrected
                Mouse_Resource.tile_location_y-=1;
            }
        }
        else { //right half
            float slope=((width-(x-x_half)%width)*.48); //slope of hex triangle is .48, used to calculate line
            if(slope<(y_tri-tmp_y)) {//if mouse position is above the line, the tile position is corrected
                Mouse_Resource.tile_location_y-=1;
            }
        }
    }
    if(Mouse_Resource.tile_location_y%2==0) { //set width for even rows
        Mouse_Resource.tile_location_x=x/width*width;
    }
    else { //set width for odd rows
        Mouse_Resource.tile_location_x=((x-x_half)/width*width)+x_half;
    }
    Mouse_Resource.tile_location_y=Mouse_Resource.tile_location_y*y_rect;//set height
}

//closes and frees sdl assets

void close() {
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    Renderer=NULL;
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

//this function moves the camera when the user hovers over the edge of the map

bool move_camera(Mouse_Resources &Mouse_Resource) {
    bool moved=0;
    if(Mouse_Resource.x<10){//Moves Left based on proximity to edge
        Mouse_Resource.x_modifier+=2;
        moved=1;
    }
    else if(Mouse_Resource.x<20){//Moves Left based on proximity to edge
        Mouse_Resource.x_modifier+=1;
        moved=1;
    }
    if(Mouse_Resource.x>SCREEN_WIDTH-10) {//Moves Right based on proximity to edge
        Mouse_Resource.x_modifier-=2;
        moved=1;
    }
    else if(Mouse_Resource.x>SCREEN_WIDTH-20) {//Moves Right based on proximity to edge
        Mouse_Resource.x_modifier-=1;
        moved=1;
    }
    if(Mouse_Resource.y<40 && Mouse_Resource.y>30) {//Moves Up based on proximity to edge
        Mouse_Resource.y_modifier+=2;
        moved=1;
    }
    else if(Mouse_Resource.y<50 && Mouse_Resource.y>30) {//Moves Up based on proximity to edge
        Mouse_Resource.y_modifier+=1;
        moved=1;
    }
    if(Mouse_Resource.y>SCREEN_HEIGHT-10) {//Moves Down based on proximity to edge
        Mouse_Resource.y_modifier-=2;
        moved=1;
    }
    else if(Mouse_Resource.y>SCREEN_HEIGHT-20) {//Moves Down based on proximity to edge
        Mouse_Resource.y_modifier-=1;
        moved=1;
    }
    if(Mouse_Resource.y_modifier>0) { //Prevents the user from leaving the map
        Mouse_Resource.y_modifier=0;
        moved=1;
    }
    if(Mouse_Resource.x_modifier>0) { //Prevents the user from leaving the map
        Mouse_Resource.x_modifier=0;
        moved=1;
    }
    if(Mouse_Resource.x_modifier<-50*50-25+SCREEN_WIDTH) {
        Mouse_Resource.x_modifier=-50*50-25+SCREEN_WIDTH;
    }
    if(Mouse_Resource.y_modifier<-50*28-42+SCREEN_HEIGHT) {
        Mouse_Resource.y_modifier=-50*28-42+SCREEN_HEIGHT;
    }
}

void change_zoom(Terrain_Resources terrain_resources, std::vector<Tile> &map_info, int &width, int &height) {
    int new_w, old_w, new_h, old_h; //storage for new and old

    old_w=width;
    old_h=height;

    int old_y=old_h*.7;

    new_w=terrain_resources.width*SCROLL_DEPTH/100;
    new_h=terrain_resources.height*SCROLL_DEPTH/100;

    int new_y=new_h*.7;

    int old_remainder=old_w/2;
    int new_remainder=new_w/2;

    width=new_w;
    height=new_h;

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

std::string getLower(std::map<std::string,Tile> tiles, Tile tile, int j) {
    while(tiles.find(tile.returnName())->second.returnLevel()>j) {
        tile=tiles.find(tile.returnBelow())->second;
    }
    return tile.returnName();
}

void create_map_layers(Texture &layers, Texture &minimap, Terrain_Resources &Terrain_Resource, Mouse_Resources &Mouse_Resource,std::map<std::string,std::vector<Texture>> textures,std::map<std::string,Tile> tiles) {
    SDL_SetRenderDrawColor(Renderer,0,0,0,255);
    minimap.createBlank(Renderer,(50*50+25)/5,(50*28+12)/5,SDL_TEXTUREACCESS_TARGET);
    layers.createBlank(Renderer,(50*50+25),(50*28+12),SDL_TEXTUREACCESS_TARGET);
    std::string name; Tile tile; int placex, placey;
    SDL_Rect l = {0,0,50*50+25,50*28+12};
    layers.setAsRenderTarget(Renderer);
    SDL_RenderFillRect(Renderer,&l);
    l.w=l.w/5;
    l.h=l.h/5;
    for(int i=0;i<Terrain_Resource.terrain_individual_information.size();i++) {
        name=Terrain_Resource.terrain_individual_information[i].returnName();
        tile=Terrain_Resource.terrain_individual_information[i];
        placex=tile.returnX()+Mouse_Resource.x_modifier;
        placey=tile.returnY()+Mouse_Resource.y_modifier+Terrain_Resource.height-textures.find(name)->second[tile.returnIndex()].getHeight();
        textures.find(name)->second[tile.returnIndex()].render(Renderer,placex,placey);
    }
    minimap.setAsRenderTarget(Renderer);
    SDL_RenderFillRect(Renderer,&l);
    SDL_Rect rect = {0,0,(50*50+25)/5,(50*28+12)/5};
    layers.renderRect(Renderer,&rect,NULL);
    SDL_SetRenderTarget(Renderer,NULL);
    SDL_SetRenderDrawColor(Renderer,255,255,255,255);
}

int main(int argc, char* args[]) {
    Mouse_Resources Mouse_Resource;
    Terrain_Resources Terrain_Resource;
    std::map<std::string,std::vector<Texture>> textures;
    std::map<std::string,Tile> tiles;
    Texture minimap;
    Texture layers;
    srand(time(NULL));
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
                if(!initTextures(textures)) { //Loads all textures
                    printf( "Failed to load textures!\n" );
                }
                else {
                    if(!initTiles(tiles)) {
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

                        SDL_Rect minimap_selector;
                        SDL_Rect map22;
                        SDL_Rect map33;

                        //Window Initializations
                        Window Terrain_Info(Renderer,100,100,100,100);
                        Window Map(Renderer, 400, 200, 200, 230);

                        //Button Initializations
                        Checkbox test(Renderer,"button",0,0);
                        test.setHeight(26);
                        test.setWidth(26);
                        std::string name1;

                        //TTF Initializations
                        //TTF_Font* Font=NULL;
                        //Font=TTF_OpenFont("../SDL2_project/assets/ttf/default.ttf", 12);
                        //if(Font == NULL ) {
                            //printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
                        //}
                        SDL_Color textColor = {0, 0, 0};

                        //Map Initialization
                        map_parse(tiles, Terrain_Resource.terrain_individual_information,"..//SDL2_project//map.map",textures);
                        create_map_layers(layers, minimap,Terrain_Resource,Mouse_Resource,textures,tiles);

                        std::vector<std::string> resources;

                        std::string name;

                        bool resize=false;
                        bool up=false;
                        bool down=false;


                        Tile tile;

                        int numFrames = 0;
                        int placex,placey;
                        bool moved;
                        Uint32 startTime;
                        Uint32 endTime;
                        const Uint8* currentKeyStates;
                        SDL_Rect window0_rect;

                        while(!quit) {
                            startTime = SDL_GetTicks();
                            name=" ";
                            overlap=false;
                            down=false;

                            //Check collisions
                            SDL_GetMouseState(&Mouse_Resource.x,&Mouse_Resource.y);
                            getMouseLocation(Mouse_Resource,Terrain_Resource.width, Terrain_Resource.height);
                            moved=move_camera(Mouse_Resource);
                            overlap=Terrain_Info.returnInside();


                            while( SDL_PollEvent( &e ) != 0 ) {
                                currentKeyStates = SDL_GetKeyboardState( NULL );
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
                                    change_zoom(Terrain_Resource, Terrain_Resource.terrain_individual_information,Terrain_Resource.width,Terrain_Resource.height);
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
                            /*for(int i=0;i<Terrain_Resource.terrain_individual_information.size();i++) {
                                name=Terrain_Resource.terrain_individual_information[i].returnName();
                                tile=Terrain_Resource.terrain_individual_information[i];
                                placex=tile.returnX()+Mouse_Resource.x_modifier;
                                placey=tile.returnY()+Mouse_Resource.y_modifier+Terrain_Resource.height-textures.find(name)->second[tile.returnIndex()].getHeight();
                                if(placex+50>0 && placey+50>0 && placex<map.w && placey<map.h) {
                                        textures.find(name)->second[tile.returnIndex()].render(Renderer,placex,placey);
                                    if(test.getActivate()) {
                                        textures.find("random")->second[0].render(Renderer, placex, placey, Terrain_Resource.width, Terrain_Resource.height);                               }
                                    if(!overlap) {
                                        if(Terrain_Resource.terrain_individual_information[i].returnY()==Mouse_Resource.tile_location_y && Terrain_Resource.terrain_individual_information[i].returnX()==Mouse_Resource.tile_location_x) {
                                            textures.find("ui")->second[1].render(Renderer, placex, placey);
                                            name=Terrain_Resource.terrain_individual_information[i].returnName();
                                            resources=Terrain_Resource.terrain_individual_information[i].returnCommodities();
                                        }
                                    }
                                }
                            }*/
                            map22.x-=Mouse_Resource.x_modifier;
                            map22.y-=Mouse_Resource.y_modifier;
                            map22.w=map.w;
                            map22.h=map.h;
                            map33= map;
                            map33.y=0;

                            layers.renderRect(Renderer,&map33,&map22);
                            //Screen Viewport
                            SDL_RenderSetViewport(Renderer, &screen);
                            Terrain_Info.render(Renderer);
                            Map.render(Renderer);
                            window0_rect=Terrain_Info.returnUsuableViewport();
                            SDL_RenderSetViewport(Renderer,&window0_rect);
                            //int counter = loadFromRenderedText(Renderer,name,Font,textColor,0,0);
                            resources.clear();
                            window0_rect=Map.returnUsuableViewport();
                            SDL_RenderSetViewport(Renderer,&window0_rect);
                            map22={0,0,std::min(window0_rect.w,(50*50+25)/5),std::min(window0_rect.h,(50*28+12)/5)};
                            minimap_selector={-Mouse_Resource.x_modifier/5,-Mouse_Resource.y_modifier/5,map.w/5,map.h/5};
                            minimap.renderRect(Renderer,&map22,&map22);
                            SDL_RenderDrawRect(Renderer,&minimap_selector);
                            SDL_RenderPresent(Renderer);
                            numFrames++;
                            endTime=SDL_GetTicks();
                            if(endTime-startTime>0) {
                                std::cout << 1000/(endTime-startTime) << " ";
                            }
                        }
                    }
                }
            }
        }
    //Free resources and close SDL2
    close();
    return 0;
    }
}
