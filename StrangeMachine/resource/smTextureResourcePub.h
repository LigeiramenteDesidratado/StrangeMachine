#ifndef SM_TEXTURE_RESOURCE_PUBLIC_H
#define SM_TEXTURE_RESOURCE_PUBLIC_H

#include "renderer/smDevicePub.h"
#include "smpch.h"

typedef struct {
  uint32_t handle;

} texture_handler_s;

texture_handler_s texture_res_new(const char *file);
void texture_res_dtor(texture_handler_s handler);

const char *texture_res_get_name(texture_handler_s handler);
uint32_t texture_res_get_channels(texture_handler_s handler);
uint32_t texture_res_get_height(texture_handler_s handler);
uint32_t texture_res_get_width(texture_handler_s handler);

void texture_res_unload_cpu_data(texture_handler_s handler);
void texture_res_unload_gpu_data(texture_handler_s handler);
bool texture_res_has_cpu_data(texture_handler_s handler);
bool texture_res_has_gpu_data(texture_handler_s handler);
bool texture_res_load_gpu_data(texture_handler_s handler);
void texture_res_load_cpu_data(texture_handler_s handler);
void texture_res_unload_data(texture_handler_s handler);

void texture_res_bind(texture_handler_s handler, uint32_t tex_index);
void texture_res_unbind(texture_handler_s handler, uint32_t tex_index);

#endif /* SM_TEXTURE_RESOURCE_PUBLIC_H */
