#include "types.hpp"
#include <cstdint>

int32_t Types::Pos3::dot(Pos3 o) {
  return this->x * o.x + this->y * o.y + this->z * o.z;
}

Types::Pos3 Types::Pos3::operator+(Pos3 o) {
  Pos3 out;
  out.x = this->x + o.x;
  out.y = this->y + o.y;
  out.z = this->z + o.z;
  return out;
}

Types::Pos3 Types::Pos3::operator-(Pos3 o) {
  Pos3 out;
  out.x = this->x - o.x;
  out.y = this->y - o.y;
  out.z = this->z - o.z;
  return out;
}

void Types::Quat::rescale() {
  f2_14 rr = this->r * this->r;
  f2_14 ii = this->i * this->i;
  f2_14 jj = this->j * this->j;
  f2_14 kk = this->k * this->k;
  // 1.5 - len^2 / 2
  f2_14 cor = (f2_14)0x6000 - (rr + ii + jj + kk) * (f2_14)0x2000;

  this->r = this->r * cor;
  this->i = this->i * cor;
  this->j = this->j * cor;
  this->k = this->k * cor;

  // printf("cor:%f len:%f\n", cor.toFloat(), (rr + ii + jj + kk).toFloat());
}

Types::Matrix Types::Quat::toRot() {
  f2_14 rr = this->r * this->r;
  f2_14 ri = this->r * this->i;
  f2_14 rj = this->r * this->j;
  f2_14 rk = this->r * this->k;

  f2_14 ii = this->i * this->i;
  f2_14 ij = this->i * this->j;
  f2_14 ik = this->i * this->k;

  f2_14 jj = this->j * this->j;
  f2_14 jk = this->j * this->k;

  f2_14 kk = this->k * this->k;

  Matrix out = Matrix();
  // if wierd stuff happens then this is the problem
  
  out.a.x = (f2_14)0x4000 - (jj + kk).mul2(1);
  out.a.y = (ij - rk).mul2(1);
  out.a.z = (ik + rj).mul2(1);

  out.b.x = (ij + rk).mul2(1);
  out.b.y = (f2_14)0x4000 - (ii + kk).mul2(1);
  out.b.z = (jk - ri).mul2(1);

  out.c.x = (ik - rj).mul2(1);
  out.c.y = (jk + ri).mul2(1);
  out.c.z = (f2_14)0x4000 - (ii + jj).mul2(1);

  return out;
}

Types::Quat Types::Quat::operator*(Quat o) {
  Quat out;
  out.r = this->r * o.r - this->i * o.i - this->j * o.j - this->k * o.k;
  out.i = this->r * o.i + this->i * o.r + this->j * o.k - this->k * o.j;
  out.j = this->r * o.j - this->i * o.k + this->j * o.r + this->k * o.i;
  out.k = this->r * o.k + this->i * o.j - this->j * o.i + this->k * o.r;
  return out;
}

int32_t Types::Matrix::Row::dot(Pos3 b) {
  return this->x * b.x + this->y * b.y + this->z * b.z;
}

Types::Pos3 Types::Matrix::rotate(Pos3 b) {
  Pos3 out;
  out.x = this->a.dot(b);
  out.y = this->b.dot(b);
  out.z = this->c.dot(b);
  return out;
}
