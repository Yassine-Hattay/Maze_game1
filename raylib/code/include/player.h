#ifndef PLAYER_H
#define PLAYER_H

#include "maze.h"
#include <cmath>     // For sin(), cos(), round()
#include "raymath.h"

struct Player {
  Vector3 position;
  float immunity;
  bool disinfectantUsed;
  float rotationX; // For pitch control (up/down)
  float rotationY; // For yaw control (left/right)
};

extern Camera3D camera;

void HandlePlayerMovement(Player &player, const Maze &maze);

void UseDisinfectant(Player &player, float &countdown, float &cooldown) ;

void UpdateCamera(Player &player, float delta);
#endif