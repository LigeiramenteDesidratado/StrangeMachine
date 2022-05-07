#ifndef SM_SCENE_COMPONENTS_H
#define SM_SCENE_COMPONENTS_H

#include "smpch.h"

#include "math/smMath.h"

typedef uint64_t sm_component_t;

extern const sm_component_t SM_TRANSFORM_COMP;
/* typedef struct sm__transform_s sm_transform_s; */

extern const sm_component_t SM_VELOCITY_COMP;
typedef vec3 velocity_s;

extern const sm_component_t SM_FORCE_COMP;
typedef vec3 force_s;

extern const sm_component_t SM_SPEED_COMP;
typedef struct {
  float speed;
} speed_s;

typedef struct sm__component_desc_s {

  /* component name */
  char *name;
  /* size in bytes of the component */
  size_t size;
  size_t offset;
  bool alligned;

  sm_component_t id;

} sm_component_desc_s;

void component_init(void);
void component_teardown(void);

void component_register_component(sm_component_desc_s *desc);
const sm_component_desc_s *component_get_desc(sm_component_t id);

#endif /* SM_SCENE_COMPONENTS_H */
