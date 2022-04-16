#include "scene/smSceneDecl.h"
#include "smpch.h"

#include "core/smCore.h"

#include "core/data/smArray.h"

#include "scene/smComponents.h"



const component_s SM_POSITION_COMP = 1 << 0;
const component_s SM_VELOCITY_COMP = 1 << 1;
const component_s SM_FORCE_COMP = 1 << 2;


component_desc_s *COMPONENTS = NULL;

void component_init(void) {

  SM_ASSERT(COMPONENTS == NULL && "Component system already initialized");

  component_desc_s pos_desc = {.name = "position", .size = sizeof(position_s), .id = SM_POSITION_COMP};
  component_desc_s vel_desc = {.name = "velocity", .size = sizeof(velocity_s), .id = SM_VELOCITY_COMP};
  component_desc_s force_desc = {.name = "force", .size = sizeof(force_s), .id = SM_FORCE_COMP};
  SM_ARRAY_PUSH(COMPONENTS, pos_desc);
  SM_ARRAY_PUSH(COMPONENTS, vel_desc);
  SM_ARRAY_PUSH(COMPONENTS, force_desc);
}

void component_register_component(component_desc_s *desc) {

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");
  SM_ASSERT(desc != NULL);
  SM_ASSERT(desc->id != 0);

#ifdef SM_DEBUG
  for (size_t i = 0; i < SM_ARRAY_SIZE(COMPONENTS); i++) {
    SM_ASSERT(COMPONENTS[i].id != desc->id && "Component already registered");
  }
#endif

  SM_ARRAY_PUSH(COMPONENTS, *desc);
}

const component_desc_s *component_get_desc(component_s id) {

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");

  for (size_t i = 0; i < SM_ARRAY_SIZE(COMPONENTS); i++) {
    if (COMPONENTS[i].id == id) {
      return &COMPONENTS[i];
    }
  }

  return NULL;
}

void component_teardown(void) {

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");

  SM_ARRAY_DTOR(COMPONENTS);
}
