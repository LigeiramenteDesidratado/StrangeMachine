#ifndef VEC4_H
#define VEC4_H

typedef struct {
  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };

    float v[4];
  };

} vec4;

#define vec4_new(X, Y, Z, W) ((vec4){.x = X, .y = Y, .z = Z, .w = W})

typedef struct {
  union {
    struct {
      int x;
      int y;
      int z;
      int w;
    };

    int v[4];
  };

} ivec4;

#define ivec4_new(X, Y, Z, W) ((ivec4){.x = X, .y = Y, .z = Z, .w = W})

#endif // VEC4_H
