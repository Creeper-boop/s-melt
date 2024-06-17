#include "types.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <vector>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define getSDL(window, windowSurface, windowName)                              \
  SDL_Window *window = NULL;                                                   \
  SDL_Surface *windowSurface = NULL;                                           \
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {                                          \
    printf("[SDL] failed to init video: %s\n", SDL_GetError());                \
    return 0;                                                                  \
  }                                                                            \
  window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED,               \
                            SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,             \
                            WINDOW_HEIGHT, SDL_WINDOW_SHOWN);                  \
  if (window == NULL) {                                                        \
    printf("[SDL] failed to create window: %s\n", SDL_GetError());             \
    return 0;                                                                  \
  }                                                                            \
  printf("[SDL] created window %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);         \
  windowSurface = SDL_GetWindowSurface(window);                                \
  SDL_FillRect(windowSurface, NULL,                                            \
               SDL_MapRGB(windowSurface->format, 0x00, 0x00, 0x00));           \
  SDL_UpdateWindowSurface(window);

// standard mesh
class MeshObject {
public:
  Types::Pos3 pos; // object center and point of rotation
  Types::Quat rot; // object rotation
  int32_t size;    // distance between object centre and the furthest vert
  std::vector<Types::Pos3> verts; // max 256 elements
  std::vector<int16_t> indicies;

  MeshObject(Types::Pos3 pos = Types::Pos3(), Types::Quat rot = Types::Quat())
      : pos{pos}, rot{rot}, verts(), indicies() {}

  ~MeshObject() {}
};
// projected object
class ProjPart {
public:
  // mark sign main fixed
  //   1 |  1 | 15 | 15
  int32_t k;
  Types::Pos2 a;
  Types::Pos2 b;

  ProjPart() {}
  ~ProjPart() {}
};
// camera
class Camera {
private:
  // this has to change for pico but for now is what is used
  std::vector<int8_t> row_buff;
  void setPixel(int16_t pix, int32_t col, SDL_PixelFormat *format);

public:
  int16_t foc;
  Types::Quat rot;
  Types::Pos3 pos;

  std::vector<ProjPart> composite;

  Camera(SDL_Surface *surface, int16_t foc, Types::Pos3 pos = Types::Pos3(),
         Types::Quat rot = Types::Quat())
      : foc{foc}, pos{pos}, rot{rot},
        row_buff(WINDOW_WIDTH * surface->format->BytesPerPixel, 0),
        composite() {}
  ~Camera() {}

  void project(MeshObject *obj);
  void draw(SDL_Surface *surface);
};
