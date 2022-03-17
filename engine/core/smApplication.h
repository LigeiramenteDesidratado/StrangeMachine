#ifndef SM_APPLICATION_H
#define SM_APPLICATION_H

#include "core/smLayer.h"
#include <stdbool.h>

struct application_s;

/* Allocate memorty for application */
struct application_s *application_new(void);

/* Constructor */
bool application_ctor(struct application_s *app, const char *name);

/* Destructor */
void application_dtor(struct application_s *app);

/* Application main loop */
void application_do(struct application_s *app);


void application_push_layer(struct application_s *app, layer_s *layer);
void application_push_overlay(struct application_s *app, layer_s *layer);

#endif /* SM_APPLICATION_H */
