#ifndef SM_SCENE_COMPONENTS_H
#define SM_SCENE_COMPONENTS_H

#include "math/smMath.h"
#include "smpch.h"

typedef uint32_t component_s;

extern const component_s SM_POSITION_COMP;
typedef vec3 position_s;

extern const component_s SM_VELOCITY_COMP;
typedef vec3 velocity_s;

extern const component_s SM_FORCE_COMP;
typedef vec3 force_s;

typedef struct {

  /* component name */
  char *name;
  /* size in bytes of the component */
  size_t size;

  uint32_t id;

} component_desc_s;

void component_init(void);
void component_teardown(void);

void component_register_component(component_desc_s *desc);
const component_desc_s *component_get_desc(component_s id);

#endif /* SM_SCENE_COMPONENTS_H */
