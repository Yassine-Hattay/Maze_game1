#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <raylib.h>
#include <vector>

struct Maze {
  std::vector<std::vector<int>> grid;
  int rows;
  int cols;
};

#include "player.h"

struct Player;

extern bool sart_countdown;

void FillMazeManual(Maze &maze);

void FillMazeRandom(Maze &maze, int virusCount, int difficulty);

void HandleMazeItemInteraction(Player &player, Maze &maze, bool &won,
                               float &countdown, float customCountdown);
#endif
