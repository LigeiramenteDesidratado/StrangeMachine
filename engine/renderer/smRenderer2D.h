#ifndef SM_RENDERER_2D_H
#define SM_RENDERER_2D_H

#include "smpch.h"

#include "renderer/smDeviceDefs.h"

struct renderer2D_s;

struct renderer2D_s *renderer2D_new(void);

bool renderer2D_ctor(struct renderer2D_s *renderer, device_api_e device);
void renderer2D_dtor(struct renderer2D_s *renderer);
void renderer2D_draw(struct renderer2D_s *renderer);

#endif /* SM_RENDERER_2D_H */
