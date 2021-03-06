#ifndef PLAYER_H
#define PLAYER_H

#include "math/smMath.h"
#include "stdbool.h"

struct player_s;

struct player_s *player_new(void);

// Constructor
bool player_ctor(struct player_s *player);

// Destructor
void player_dtor(struct player_s *player);

// Forward declarations
void player_do(struct player_s *player, float dt);
void player_draw(struct player_s *player);
void player_get_transformation_mat4(struct player_s *player, mat4 out);
void player_get_position(struct player_s *player, vec3 out);
void player_draw_debug(struct player_s *player);

#endif // PLAYER_H
