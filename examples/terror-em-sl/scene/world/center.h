#ifndef CENTER_H
#define CENTER_H

#include "util/common.h"
#include "scene/scene.h"

struct center_s;

// Allocate memory
struct center_s *center_new(void);

// Constructor
int center_ctor(struct center_s *center, SCENE_EX6 id);

// Destructor
void center_dtor(struct center_s *lvl_one);


#endif // CENTER_H

