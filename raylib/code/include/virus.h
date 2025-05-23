#ifndef VIRUS_H
#define VIRUS_H

#include "UI.h"
#include "maze.h"

struct Virus {
  Vector3 position;
  int type;
};

void UpdateVirusMovement(std::vector<Virus> &viruses, const Maze &maze);
#endif
