#ifndef VEC2_H
#define VEC2_H

typedef struct {

  union {
    struct {
      float x;
      float y;
    };

    float v[2];
  };

} vec2;


#define vec2_print(V)                                                          \
  printf("%s\n\t%f, %f\n", #V, V.x, V.y);

#define vec2_new(X, Y) ((vec2){.x = X, .y = Y})

typedef struct {
  union {
    struct {
      int x;
      int y;
    };

    int v[2];
  };

} ivec2;

typedef struct {
  union {
    struct {
      unsigned int x;
      unsigned int y;
    };

    unsigned int v[2];
  };

} uivec2;

#endif // VEC2_H
