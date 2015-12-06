
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <string>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "texture.h"
#include "button.h"
#include "slider.h"

#include <noise/noise.h>
#include "noiseutils.h"

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int X_MODIFIER=0;
int Y_MODIFIER=0;

SDL_Renderer* Renderer = NULL;
SDL_Window* window = NULL;
SDL_Surface* ScreenSurface = NULL;

enum {deep_ocean=0,shallow_ocean=1,beach=2,plain=3,forest=4,hill=5,mountain=6,jungle=7,marsh=8};

struct Map_Info {
    std::vector<float> heights = {-.5,-.3,-.2,.3,.4,.6};
    module::Perlin myModule;
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    float x;
    float y;
    std::vector<float> p;
    std::vector<float> c;
    std::vector<std::pair<int,int>> locations;
};

struct Texture_Info {
    std::vector<Texture> terrain_textures;
    int x; int y;
};

using namespace noise;

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

bool initTextures(Texture_Info &textures) {
    bool success=true;
    std::ifstream file("..//Noise//textures//textures");
    if(!file.good()) {
        printf("Cannot open textures!\n");
        success=false;
    }
    std::string x, y,name;
    int index=0;
    file >> x;
    file >> y;
    textures.x=std::atoi(x.c_str());
    textures.y=std::atoi(y.c_str());
    while(!file.eof()) {
        file >> name;
        textures.terrain_textures.push_back(Texture());
        textures.terrain_textures[index].loadFromFile(Renderer, "..//Noise//textures//"+name+".png");
        index++;
    }
    return true;
}

bool generate_rand_map(Map_Info &storage) {
    storage.myModule.SetSeed(rand()%12456754325413);
    storage.heightMapBuilder.SetSourceModule (storage.myModule);
    storage.heightMapBuilder.SetDestNoiseMap (storage.heightMap);
    storage.heightMapBuilder.SetDestSize (storage.x,storage.y);
    storage.heightMapBuilder.SetBounds (0, 1, 0, 3);
    storage.heightMapBuilder.Build ();
}

bool render_to_image(std::string name, Map_Info &storage) {
    bool success=true;
    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap (storage.heightMap);
    renderer.SetDestImage (image);
    renderer.Render ();

    utils::WriterBMP writer;
    writer.SetSourceImage (image);
    writer.SetDestFilename ("..//Noise//"+name+".bmp");
    writer.WriteDestFile ();
    return success;
}

bool take_data(Map_Info &storage) {
    float value;
    for(float i=0;i<storage.y;i++) {
        for(float j=0;j<storage.x;j++) {
            value = storage.myModule.GetValue(j/storage.x,i/storage.y,4);
            storage.p.push_back(value);
        }
    }
    storage.c=storage.p;
}

bool store_in_file(std::string file, Map_Info &storage) {
    std::ofstream map("..//SDL2//"+file+".txt");
    for(int i=0;i<storage.y;i++) {
        for(int j=0;j<storage.x;j++) {
                map<<storage.p[j+i*storage.x]<<" ";
        }
        map<<"/\n";
    }
}

bool assign_coordinates(Map_Info &storage) {
       int y=0;
       int x=0;
       for(int i=0; i<storage.y; i++) {
           for(int j=0; j<storage.x; j++) {
                storage.locations.push_back(std::make_pair(x,y));
                x+=50;
           }
           if(x%50==0) {
               x=25;
           }
           else {
               x=0;
           }
           y+=37;
       }
}

bool assign_terrain(Map_Info &storage) {
    for(int i=0; i<storage.p.size(); i++) {
        if(storage.c[i]<storage.heights[0]) {
            storage.p[i]=0;
        }
        else if(storage.c[i]<storage.heights[1]) {
            storage.p[i]=1;
        }
        else if(storage.c[i]<storage.heights[2]) {

            storage.p[i]=2;
        }
        else if(storage.c[i]<storage.heights[3]) {
            storage.p[i]=3;
        }
        else if(storage.c[i]<storage.heights[4]) {
            storage.p[i]=4;
        }
        else if(storage.c[i]<storage.heights[5]) {
            storage.p[i]=5;
        }
        else {
            storage.p[i]=6;
        }
    }
}
bool handle_map_generation(Map_Info &storage) {
    storage.myModule.SetOctaveCount (6);
    storage.myModule.SetFrequency (4.0);
    generate_rand_map(storage);
    take_data(storage);
    assign_coordinates(storage);
    assign_terrain(storage);
    store_in_file("map",storage);
}

bool check(std::vector<float> &heights) {
    if(heights[heights.size()-1]>1) {
        heights[heights.size()-1]=1;
    }
    for(int i=0; i<heights.size()-1;i++) {
        if(heights[i]>heights[i+1]) {
            heights[i]=heights[i+1];
        }
    }
}

int main (int argc, char** argv) {
  srand (time(NULL));

  float value;
  Map_Info storage;
  Texture_Info textures;

  if(!initSDL()) {//Initializes SDL and related libraries
      printf("Failed to initialize SDL!\n");
  }
  else {
      if(!initWindow()) { //Creates a window
          printf("Failed to initialize window!\n");
      }
      else {

          if(!initTextures(textures)) { //Creates a window
              printf("Failed to initialize textures!\n");
          }
          else {
              bool quit=false;
              SDL_Event e;

              storage.x=30;
              storage.y=30;

              handle_map_generation(storage);

              SDL_Rect map;
              map.x=0; map.y=0; map.w=.8*SCREEN_WIDTH; map.h=SCREEN_HEIGHT;
              SDL_Rect menu;
              menu.x=.8*SCREEN_WIDTH; menu.y=0; menu.w=.2*SCREEN_WIDTH; menu.h=SCREEN_HEIGHT;
              SDL_Rect screen;
              screen.x=0; screen.y=0; screen.w=SCREEN_WIDTH; screen.h=SCREEN_HEIGHT;


              Button button(Renderer,"..//Noise//textures//button",0,0);
              Texture menugraphic(Renderer,"..//Noise//textures//menu.png");
              Texture menugraphic1(Renderer,"..//Noise//textures//menu2.png");
              SDL_Rect indicator;
              indicator.x=menu.x+menu.w/2-menugraphic1.getWidth()/2+1;indicator.y=menu.h/10+1; indicator.w=menugraphic1.getWidth()-2; indicator.h=menugraphic1.getHeight()-2;
              button.setX(map.w+menu.w/2-button.getWidth()/2);

              SDL_Rect test=indicator;

              SDL_Rect test1=test;
              test1.h=((storage.heights[5]+1)/2)*indicator.h;
              test1.y=indicator.y+indicator.h-test1.h;

              SDL_Rect test2=test;
              test2.h=((storage.heights[4]+1)/2)*indicator.h;
              test2.y=indicator.y+indicator.h-test2.h;

              SDL_Rect test3=test;
              test3.h=((storage.heights[3]+1)/2)*indicator.h;
              test3.y=indicator.y+indicator.h-test3.h;

              SDL_Rect test4=test;
              test4.h=((storage.heights[2]+1)/2)*indicator.h;
              test4.y=indicator.y+indicator.h-test4.h;

              SDL_Rect test5=test;
              test5.h=((storage.heights[1]+1)/2)*indicator.h;
              test5.y=indicator.y+indicator.h-test5.h;

              SDL_Rect test6=test;
              test6.h=((storage.heights[0]+1)/2)*indicator.h;
              test6.y=indicator.y+indicator.h-test6.h;


              Slider slide1(Renderer,"..//Noise//textures//slider.png",test1.x,test1.y,2);


              Slider slide2(Renderer,"..//Noise//textures//slider.png",test2.x,test2.y,2);


              Slider slide3(Renderer,"..//Noise//textures//slider.png",test3.x,test3.y,2);


              Slider slide4(Renderer,"..//Noise//textures//slider.png",test4.x,test4.y,2);


              Slider slide5(Renderer,"..//Noise//textures//slider.png",test5.x,test5.y,2);


              Slider slide6(Renderer,"..//Noise//textures//slider.png",test6.x,test6.y,2);


              int mouse_x, mouse_y;

              bool change;
              bool ready=false;
              bool yes=false;
              while(!quit) {
                  slide1.setMinY(indicator.y+1);
                  slide2.setMinY(slide1.getY()+3);
                  slide3.setMinY(slide2.getY()+3);
                  slide4.setMinY(slide3.getY()+3);
                  slide5.setMinY(slide4.getY()+3);
                  slide6.setMinY(slide5.getY()+3);

                  slide1.setMaxY(slide2.getY()-3);
                  slide2.setMaxY(slide3.getY()-3);
                  slide3.setMaxY(slide4.getY()-3);
                  slide4.setMaxY(slide5.getY()-3);
                  slide5.setMaxY(slide6.getY()-3);
                  slide6.setMaxY(indicator.y+indicator.h-3);

                  bool change=false;
                  check(storage.heights);
                  while( SDL_PollEvent( &e ) != 0 ) {
                      button.handleEvent(&e);
                      slide1.handleEvent(&e);
                      slide2.handleEvent(&e);
                      slide3.handleEvent(&e);
                      slide4.handleEvent(&e);
                      slide5.handleEvent(&e);
                      slide6.handleEvent(&e);
                      if(button.getActivate()) {
                          handle_map_generation(storage);
                          X_MODIFIER=0;
                          Y_MODIFIER=0;
                      }
                      if(e.type==SDL_MOUSEBUTTONDOWN) {
                          ready=true;
                      }
                      if(e.type==SDL_MOUSEBUTTONUP) {
                          ready=false;
                      }
                      if( e.type == SDL_QUIT ) {
                          quit = true;
                      }
                  }
                  if(change) {
                    assign_terrain(storage);
                  }
                  SDL_GetMouseState(&mouse_x,&mouse_y);
                  if(mouse_x<12) {
                      X_MODIFIER--;
                  }
                  if(mouse_y<12 && mouse_x<=map.w) {
                      Y_MODIFIER--;
                  }
                  if(mouse_y>SCREEN_HEIGHT-12 && mouse_x<=map.w
                          ) {
                      Y_MODIFIER++;
                  }
                  if(mouse_x>map.w-12 && mouse_x<=map.w) {
                      X_MODIFIER++;
                  }
                  if(X_MODIFIER<0) {
                      X_MODIFIER=0;
                  }
                  if(Y_MODIFIER<0) {
                      Y_MODIFIER=0;
                  }
                  SDL_RenderClear(Renderer);

                  SDL_RenderSetViewport(Renderer,&map);

                  for(int i=0; i<storage.p.size(); i++) {
                        textures.terrain_textures[storage.p[i]].render(Renderer,storage.locations[i].first-X_MODIFIER,storage.locations[i].second-Y_MODIFIER);
                  }

                  SDL_RenderSetViewport(Renderer,&menu);

                  SDL_RenderSetViewport(Renderer,&screen);
                  menugraphic.render(Renderer,menu.x,menu.y);
                  menugraphic1.render(Renderer,menu.x+menu.w/2-menugraphic1.getWidth()/2,menu.h/10);
                  button.render(Renderer);

                  SDL_SetRenderDrawColor( Renderer, 128, 128, 128, 0xFF );//grey
                  SDL_RenderFillRect(Renderer, &test);

                  SDL_SetRenderDrawColor( Renderer, 127, 51, 0, 0xFF );//brown
                  test1.h=((storage.heights[5]+1)/2)*indicator.h;
                  test1.y=indicator.y+indicator.h-test1.h;
                  SDL_RenderFillRect(Renderer, &test1);

                  SDL_SetRenderDrawColor( Renderer, 0, 127,70, 0xFF );//dark green
                  test2.h=((storage.heights[4]+1)/2)*indicator.h;
                  test2.y=indicator.y+indicator.h-test2.h;
                  SDL_RenderFillRect(Renderer, &test2);

                  SDL_SetRenderDrawColor( Renderer, 38, 127,0, 0xFF );//green
                  test3.h=((storage.heights[3]+1)/2)*indicator.h;
                  test3.y=indicator.y+indicator.h-test3.h;
                  SDL_RenderFillRect(Renderer, &test3);

                  SDL_SetRenderDrawColor( Renderer, 241, 197,168, 0xFF );//beach
                  test4.h=((storage.heights[2]+1)/2)*indicator.h;
                  test4.y=indicator.y+indicator.h-test4.h;
                  SDL_RenderFillRect(Renderer, &test4);

                  SDL_SetRenderDrawColor( Renderer, 0, 74,127, 0xFF );//shallow
                  test5.h=((storage.heights[1]+1)/2)*indicator.h;
                  test5.y=indicator.y+indicator.h-test5.h;
                  SDL_RenderFillRect(Renderer, &test5);

                  SDL_SetRenderDrawColor( Renderer, 33, 0,127, 0xFF );//deep
                  test6.h=((storage.heights[0]+1)/2)*indicator.h;
                  test6.y=indicator.y+indicator.h-test6.h;
                  SDL_RenderFillRect(Renderer, &test6);

                  storage.heights[0]=float(indicator.y+indicator.h-slide6.getY())/indicator.h*2-1;
                  storage.heights[1]=float(indicator.y+indicator.h-slide5.getY())/indicator.h*2-1;
                  storage.heights[2]=float(indicator.y+indicator.h-slide4.getY())/indicator.h*2-1;
                  storage.heights[3]=float(indicator.y+indicator.h-slide3.getY())/indicator.h*2-1;
                  storage.heights[4]=float(indicator.y+indicator.h-slide2.getY())/indicator.h*2-1;
                  storage.heights[5]=float(indicator.y+indicator.h-slide1.getY())/indicator.h*2-1;

                  assign_terrain(storage);

                  SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 0xFF );
                  slide1.render(Renderer);
                  slide2.render(Renderer);
                  slide3.render(Renderer);
                  slide4.render(Renderer);
                  slide5.render(Renderer);
                  slide6.render(Renderer);


                  SDL_SetRenderDrawColor( Renderer, 255, 255, 255, 0xFF );
                  SDL_RenderPresent(Renderer);

              }
          }
      }
  }
  return 0;
}
