#include "video.hpp"
#include <SDL2/SDL_video.h>
#include <chrono>
#include <thread>
#include <vector>

// rpi2040 chip has a 32 bit single-cycle multiplier

int main(int argc, char *args[]) {
  getSDL(window, windowSurface, "s-melt");

  Camera cam(windowSurface, 640);
  MeshObject obj;
  obj.pos = Types::Pos3(0, 0, -150);
  obj.verts.push_back(Types::Pos3(100, 100, 100));
  obj.verts.push_back(Types::Pos3(-100, 100, 100));
  obj.verts.push_back(Types::Pos3(100, -100, 100));
  obj.verts.push_back(Types::Pos3(100, 100, -100));
  obj.verts.push_back(Types::Pos3(100, -100, -100));
  obj.verts.push_back(Types::Pos3(-100, 100, -100));
  obj.verts.push_back(Types::Pos3(-100, -100, 100));
  obj.verts.push_back(Types::Pos3(-100, -100, -100));
  obj.indicies.push_back(0x0001);
  obj.indicies.push_back(0x0002);
  obj.indicies.push_back(0x0003);
  obj.indicies.push_back(0x0704);
  obj.indicies.push_back(0x0705);
  obj.indicies.push_back(0x0706);

  Types::Quat rotquat =
      Types::Quat(0x3fff, 0x0000, 0x0047, 0x0000); // angle of 1° over 0 1 0

  std::chrono::time_point<std::chrono::system_clock> frame_start;
  std::chrono::time_point<std::chrono::system_clock> frame_end;

  SDL_Event e;
  bool quit = false;
  while (!quit) {
//    std::chrono::time_point<std::chrono::system_clock> frame_start = frame_end;

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
    }

    obj.rot = obj.rot * rotquat;
    obj.rot.rescale();

    cam.project(&obj);
    cam.draw(windowSurface);

    SDL_UpdateWindowSurface(window);

//    frame_end = std::chrono::system_clock::now();
//    std::this_thread::sleep_for(
//        std::chrono::milliseconds(20) -
//        std::chrono::duration_cast<std::chrono::milliseconds>(frame_end -
//                                                              frame_start));
  }
  SDL_DestroyWindow(window);
}
