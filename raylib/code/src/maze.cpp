
#include "maze.h"

bool sart_countdown = false;

void InputMazeSize(int &rows, int &cols) {
  std::cout << "Enter maze size:\nRows: ";
  std::cin >> rows;
  std::cout << "Cols: ";
  std::cin >> cols;
}

void FillMazeManual(Maze &maze) {
  maze.grid.resize(maze.rows, std::vector<int>(maze.cols, 0));
  std::cout << "\nManual Maze Fill:\n";
  std::cout << "Enter values (0=empty, 1=weak virus, 2=covid, 3=potion, 4=gel, "
               "5=wall, 6=finish):\n";

  for (int i = 0; i < maze.rows; ++i) {
    for (int j = 0; j < maze.cols; ++j) {
      int val;
      std::cout << "[" << i << "," << j << "]: ";
      std::cin >> val;
      maze.grid[i][j] = val;
    }
  }
}




void FillMazeRandom(Maze &maze, int virusCount, int difficulty) {
  maze.grid.resize(maze.rows, std::vector<int>(maze.cols, 0));

  // Modify wall density based on difficulty
  int wallThreshold = (difficulty == 1)   ? 5
                      : (difficulty == 2) ? 20
                                          : 30; // less walls in easy mode

  // Fill the grid with random elements based on the difficulty
  for (int i = 0; i < maze.rows; ++i) {
    for (int j = 0; j < maze.cols; ++j) {
      int r = GetRandomValue(0, 100);
      if (i == 0 || j == 0 || i == maze.rows - 1 || j == maze.cols - 1)
        maze.grid[i][j] = 5; // Border walls
      else if (r < wallThreshold)
        maze.grid[i][j] = 5; // Wall, lower percentage in easy mode
      else if (r < wallThreshold + 10)
        maze.grid[i][j] = 3; // Immunity potion
      else if (r < wallThreshold + 15)
        maze.grid[i][j] = 4; // Gel
      else
        maze.grid[i][j] = 0; // Empty space
    }
  }

  maze.grid[1][1] = 0;                         // player start
  maze.grid[maze.rows - 2][maze.cols - 2] = 6; // finish

  // Add viruses based on virusCount (difficulty)
  int virusesAdded = 0;
  while (virusesAdded < virusCount) {
    int r = GetRandomValue(1, maze.rows - 2);
    int c = GetRandomValue(1, maze.cols - 2);

    // Ensure that the spot is empty and not too close to other viruses or walls
    if (maze.grid[r][c] == 0) {
      bool tooClose = false;
      for (int i = r - 1; i <= r + 1 && !tooClose; ++i) {
        for (int j = c - 1; j <= c + 1 && !tooClose; ++j) {
          if (maze.grid[i][j] == 1 ||
              maze.grid[i][j] == 2) { // Check adjacent spots for viruses
            tooClose = true;
          }
        }
      }
      if (!tooClose) {
        int virusType = GetRandomValue(
            1, 2); // Randomly choose between weak and COVID virus
        maze.grid[r][c] = virusType; // Place the virus
        ++virusesAdded;              // Increment the counter for added viruses
      }
    }
  }
}
  


void HandleMazeItemInteraction(Player &player, Maze &maze, bool &won,
                               float &countdown, float customCountdown) {
  int x = (int)player.position.x;
  int z = (int)player.position.z;
  int cell = maze.grid[z][x];

  switch (cell) {
  case 1: // Weak virus
    player.immunity -= 1.0f;
    break;
  case 2: // COVID virus
    if (player.immunity > 0)
      player.immunity -= 1.0f; // Reduce immunity gradually
    else
      player.immunity = 0.0f; // Prevent going below zero immunity
    countdown =
        customCountdown; // start countdown for 1 min when touching COVID
    sart_countdown = true;
    break;
  case 3: // Immunity potion
    player.immunity =
        std::min(player.immunity + 1.0f, 5.0f); // max immunity cap
    break;
  case 4:                        // Gel
    countdown = customCountdown; // Reset the countdown when touching the gel
    sart_countdown = false;
    break;
  case 6: // Finish point
    won = true;
    std::cout << "You win!\n";
    break;
  }
  if (cell >= 1 && cell <= 4)
    maze.grid[z][x] = 0; // Clear used item
}
