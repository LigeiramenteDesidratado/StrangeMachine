#ifndef SM_DESCRIPTOR_H
#define SM_DESCRIPTOR_H

#include "smpch.h"

#include "renderer/api/smTypes.h"

typedef struct {

  b8 dynamic;
  size_t buffer_size;
  void *data;

} buffer_desc_s;

typedef struct {

  u32 index;
  i32 size;
  types_e type;
  size_t stride;
  const void *pointer;

} attribute_desc_s;

typedef struct {

  const char *name;
  u32 location;

} attribute_loc_desc_s;

typedef struct {

  const char *vertex_shader;
  const char *fragment_shader;

} shader_desc_s;

#endif /* SM_DESCRIPTOR_H */
