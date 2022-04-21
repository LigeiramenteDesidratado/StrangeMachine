#ifndef SM_DESCRIPTOR_H
#define SM_DESCRIPTOR_H

#include "smpch.h"

typedef struct {

  bool dynamic;
  size_t buffer_size;
  void *data;

} buffer_desc_s;

typedef struct {

  uint32_t index;
  int32_t size;
  uint32_t type;
  size_t stride;
  const void *pointer;

} attribute_desc_s;

typedef struct {

  const char *name;
  uint32_t location;

} attribute_loc_desc_s;

typedef struct {

  const char *vertex_shader;
  const char *fragment_shader;

} shader_desc_s;

#endif /* SM_DESCRIPTOR_H */
