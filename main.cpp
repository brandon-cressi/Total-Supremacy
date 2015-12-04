#include <iostream>
#include <vector>
#include <noise/noise.h>
#include "noiseutils.h"
#include <fstream>
#include <random>


enum {plain=0,ocean=1,desert=2,forest=3,jungle=4,marsh=5};

using namespace noise;

int main (int argc, char** argv) {
  srand (time(NULL));
  std::ofstream map("..//SDL2//map.txt");
  float x= 30.;
  float y= 30.;
  float value;
  module::Perlin myModule;
  myModule.SetSeed(rand()%12456754325413);
  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule (myModule);
  heightMapBuilder.SetDestNoiseMap (heightMap);
  heightMapBuilder.SetDestSize (x,y);
  heightMapBuilder.SetBounds (0, 1, 0, 3);
  heightMapBuilder.Build ();
  utils::RendererImage renderer;
  utils::Image image;
  renderer.SetSourceNoiseMap (heightMap);
  renderer.SetDestImage (image);
  renderer.Render ();

  utils::WriterBMP writer;
    writer.SetSourceImage (image);
    writer.SetDestFilename ("..//Noise//tutorial.bmp");
    writer.WriteDestFile ();
  std::vector<float> p;
  int count=0;
  for(float i=0;i<y;i++) {
      for(float j=0;j<x;j++) {
          value = myModule.GetValue(j/x,i/y,4);
          p.push_back(value);
      }
  }
  for(int i=0;i<x*y;i++) {
      std::cout << p[i] << " ";
  }

  for(int i=0;i<x*y;i++) {
      if(p[i]<-.0) {
          p[i]=ocean;
      }
      else if(p[i]<.05) {
          p[i]=desert;
      }
      else if(p[i]<.25) {
          p[i]=plain;
      }
      else if(p[i]<.5) {
          p[i]=forest;
      }
      else if(p[i]<1) {
          p[i]=jungle;
      }
  }
  for(int i=0;i<y;i++) {
      for(int j=0;j<x;j++) {
              map<<p[j+i*x]<<" ";
      }
      map<<"/\n";
  }

  return 0;
}
