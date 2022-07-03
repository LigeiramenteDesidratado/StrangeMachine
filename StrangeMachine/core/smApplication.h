#ifndef SM_APPLICATION_H
#define SM_APPLICATION_H

#include "smpch.h"

#include "core/smLayer.h"

struct application_s;

/* Allocate memorty for application */
struct application_s *application_new(void);

/* Constructor */
b8 application_ctor(struct application_s *app, const char *name);

/* Destructor */
void application_dtor(struct application_s *app);

/* Application main loop */
void application_do(struct application_s *app);

void application_push_layer(struct application_s *app, struct layer_s *layer);
void application_push_overlay(struct application_s *app, struct layer_s *layer);
double aplication_get_fps(struct application_s *app);

#endif /* SM_APPLICATION_H */
