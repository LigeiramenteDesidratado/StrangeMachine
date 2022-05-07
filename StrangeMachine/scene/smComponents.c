#include "smpch.h"

#include "core/smCore.h"

#include "scene/smComponents.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "COMPONENTS"

const sm_component_t SM_TRANSFORM_COMP = 1ULL << 0;
const sm_component_t SM_VELOCITY_COMP = 1ULL << 1;
const sm_component_t SM_FORCE_COMP = 1ULL << 2;
const sm_component_t SM_SPEED_COMP = 1ULL << 3;

sm_component_desc_s *COMPONENTS = NULL;

void component_init(void) {

  SM_ASSERT(COMPONENTS == NULL && "Component system already initialized");

  sm_component_desc_s transform_desc = {
      .name = "transform", .size = sizeof(sm_transform_s), .alligned = true, .id = SM_TRANSFORM_COMP};
  sm_component_desc_s vel_desc = {
      .name = "velocity", .size = sizeof(velocity_s), .alligned = false, .id = SM_VELOCITY_COMP};
  sm_component_desc_s force_desc = {.name = "force", .size = sizeof(force_s), .alligned = false, .id = SM_FORCE_COMP};
  sm_component_desc_s speed_desc = {.name = "speed", .size = sizeof(speed_s), .alligned = false, .id = SM_SPEED_COMP};

  SM_ARRAY_PUSH(COMPONENTS, transform_desc);
  SM_ARRAY_PUSH(COMPONENTS, vel_desc);
  SM_ARRAY_PUSH(COMPONENTS, force_desc);
  SM_ARRAY_PUSH(COMPONENTS, speed_desc);
}

void component_register_component(sm_component_desc_s *desc) {

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

const sm_component_desc_s *component_get_desc(sm_component_t id) {

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

#undef SM_MODULE_NAME
