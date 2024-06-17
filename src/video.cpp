#include "video.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>

void Camera::setPixel(int16_t pix, int32_t col, SDL_PixelFormat *format) {
  *(int32_t *)(this->row_buff.data() + pix * format->BytesPerPixel) = col;
}

void Camera::project(MeshObject *obj) {
  Types::Matrix rotm = (this->rot * obj->rot).toRot();

  std::vector<Types::Pos2> pverts(obj->verts.size());
  for (int i = 0; i < obj->verts.size(); i++) {
    Types::Pos3 vert = rotm.rotate(obj->verts.at(i)) + obj->pos - this->pos;
    Types::Pos2 pvert;
    int32_t pval = ((int32_t)this->foc << 16) / (vert.z - (int32_t)this->foc);
    pvert.x = (vert.x * pval >> 16) + (WINDOW_WIDTH >> 1);
    pvert.y = (vert.y * pval >> 16) + (WINDOW_HEIGHT >> 1);
    pverts.at(i) = pvert;
  }

  for (int16_t ipair : obj->indicies) {
    Types::Pos2 a = pverts.at(ipair >> 8);
    Types::Pos2 b = pverts.at(ipair & 0x00ff);
    ProjPart part;

    if (a.y == b.y) {
      part.k = 0x40000000;
      if (a.x < b.x) {
        part.a = a;
        part.b = b;
      } else {
        part.a = b;
        part.b = a;
      }
    } else {
      if (a.y < b.y) {
        part.a = a;
        part.b = b;
      } else {
        part.a = b;
        part.b = a;
      }
      if (a.x < b.x)
        part.k =
            (uint32_t)(((int32_t)(b.x - a.x) << 16) / (int32_t)(b.y - a.y)) >>
            1;
      else
        part.k =
            (uint32_t)(((int32_t)(a.x - b.x) << 16) / (int32_t)(a.y - b.y)) >>
            1;
    }
    this->composite.push_back(part);
  }
}

// renders screen by lines and removes all drawn elements from composite
void Camera::draw(SDL_Surface *surface) {
  std::sort(this->composite.begin(), this->composite.end(),
            [](ProjPart a, ProjPart b) { return a.a.y < b.a.y; });
  // simulation code
  for (int16_t y = 0; y < surface->h; y++) {
    for (int16_t i = 0; i < this->composite.size(); i++) {
      ProjPart part = this->composite.at(i);
      if (part.k & 0x80000000 || part.a.y > y)
        continue;

      if (part.k == 0x40000000) {
        for (int x = part.a.x; x <= part.b.x; x++)
          this->setPixel(x, 0x00ffffff, surface->format);
      } else {
        int32_t dx = (y - part.a.y) * (part.k << 1);
        int16_t x = (int16_t)(dx >> 16) + part.a.x;
        this->setPixel(x, 0x00ffffff, surface->format);
        // for contineus lines
        int16_t xn = part.b.x;
        if (part.b.y != y)
          xn = (int16_t)((dx + (part.k << 1)) >> 16) + part.a.x;
        if (part.k & 0x40000000) {
          for (int16_t xf = xn; xf < x; xf++)
            this->setPixel(xf, 0x00ffffff, surface->format);
        } else {
          for (int16_t xf = x; xf < xn; xf++)
            this->setPixel(xf, 0x00ffffff, surface->format);
        }
      }

      if (part.b.y <= y)
        this->composite.at(i).k |= 0x80000000;
    }
    // example code to change pixel at x, y to r, g, b
    // for (int32_t x = 0; x < surface->w; x++) {
    //  buffer assign
    //  int32_t *target = (int32_t *)(this->row_buff->data() +
    //                               x * surface->format->BytesPerPixel);
    //*target = r << surface->format->Rshift | g << surface->format->Gshift |
    //           b << surface->format->Bshift;
    //}
    memcpy((int8_t *)surface->pixels + y * surface->pitch,
           this->row_buff.data(), this->row_buff.size());
    memset(this->row_buff.data(), 0x00, this->row_buff.size());
  }
  this->composite.clear();

  // old code to draw only verts
  //
  // for (ProjObject *pobj : *this->composite) {
  //   for (Types::Pos2 vert : *pobj->verts) {
  //     if (vert.x < WINDOW_WIDTH && vert.x > 0 && vert.y < WINDOW_HEIGHT &&
  //         vert.y > 0) {
  //       // this sets the pixel at x,y to desired data
  //       int32_t *target =
  //           (int32_t *)((int8_t *)surface->pixels + vert.y * surface->pitch +
  //                       vert.x * surface->format->BytesPerPixel);
  //       *target = 0x00ffffff;
  //     }
  //   }
  // }
}
