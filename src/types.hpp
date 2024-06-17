#include <cmath>
#include <cstdint>
#include <cstdio>

namespace Types {
class Vector2;
class Vector3;
class Quat;
class Matrix;

// int16_t -> f2_14
// S|N|D|D D->
class f2_14 {
public:
  int16_t val;

  f2_14(int16_t val = 0) : val{val} {}
  ~f2_14() {}

  f2_14 operator>>(int8_t b) { return this->val >> b; }
  f2_14 operator<<(int8_t b) { return this->val << b; }
  // multiplication by power of 2
  f2_14 mul2(int8_t b) {
    // if ((this->val & 0xc000) == 0x8000 || (this->val & 0xc000) == 0x4000)
    //  printf("WARN %04x\n", (uint16_t)this->val);
    // return this->val & 0x8000 | (this->val & 0x7fff) << b;
    return this->val << b;
  }

  f2_14 operator+(f2_14 b) { return this->val + b.val; }
  f2_14 operator-(f2_14 b) { return this->val - b.val; }

  f2_14 operator*(f2_14 b) {
    return (int16_t)((int32_t)this->val * (int32_t)b.val >> 14);
  }

  int32_t operator*(int32_t b) { return (int32_t)this->val * b >> 14; }
  // debugging
  float_t toFloat() { return (float_t)(this->val) / 0x4000; }
};

// projected grapics
class Pos2 {
public:
  int16_t x;
  int16_t y;

  Pos2(int16_t x = 0, int16_t y = 0) : x{x}, y{y} {}
  ~Pos2() {}
};
// used for vert pos
class Pos3 {
public:
  int32_t x;
  int32_t y;
  int32_t z;

  Pos3(int32_t x = 0, int32_t y = 0, int32_t z = 0) : x{x}, y{y}, z{z} {}
  ~Pos3() {}

  int32_t dot(Pos3 o);
  Pos3 operator+(Pos3 o);
  Pos3 operator-(Pos3 o);
};
// rotationary quat should always be of len 1
// the values are 8 24 fixed point
class Quat {
public:
  f2_14 r; //    cos t/2
  f2_14 i; // ux sin t/2
  f2_14 j; // uy sin t/2
  f2_14 k; // uz sin t/2

  Quat(f2_14 r = f2_14(0x4000), f2_14 i = f2_14(), f2_14 j = f2_14(),
       f2_14 k = f2_14())
      : r{r}, i{i}, j{j}, k{k} {}
  ~Quat() {}

  void rescale();

  Matrix toRot();
  Quat operator*(Quat o);
};
// matrix
class Matrix {
public:
  class Row {
  public:
    f2_14 x;
    f2_14 y;
    f2_14 z;

    Row(f2_14 x = f2_14(), f2_14 y = f2_14(), f2_14 z = f2_14())
        : x{x}, y{y}, z{z} {}
    ~Row() {}

    int32_t dot(Pos3 b);
  };
  Row a;
  Row b;
  Row c;

  Matrix(Row a = Row(), Row b = Row(), Row c = Row()) : a{a}, b{b}, c{c} {}
  ~Matrix() {}

  Pos3 rotate(Pos3 b);
};
} // namespace Types
