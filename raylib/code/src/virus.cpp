
#include "virus.h"

void UpdateVirusMovement(std::vector<Virus> &viruses, const Maze &maze) {
  for (auto &virus : viruses) {
    int dx = 0, dz = 0;
    int dir = GetRandomValue(0, 3);
    if (dir == 0)
      dx = -1; 
    else if (dir == 1)
      dx = 1;
    else if (dir == 2)
      dz = -1;
    else
      dz = 1;
    int newX = (int)virus.position.x + dx;
    int newZ = (int)virus.position.z + dz;
    if (newX >= 0 && newX < maze.cols && newZ >= 0 && newZ < maze.rows &&
        maze.grid[newZ][newX] != 5) {
      virus.position.x = (float)newX;
      virus.position.z = (float)newZ;
    }
  }
}


