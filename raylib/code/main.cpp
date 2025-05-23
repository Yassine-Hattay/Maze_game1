#include "UI.h"    // Handles input UI for settings
#include "maze.h"  // Handles maze structure and generation
#include "virus.h" // Handles virus structure and movement

int main() {
  while (1) // Infinite loop to allow restarting the game after a round
  {
    // ----------- Default Game Settings -----------

    int rows = 20, cols = 20, virusCount = 5,
        difficulty = 1; // Maze dimensions ,  Number of viruses , Virus movement
                        // behavior or speed
    float customCountdown =
        50.0f; // Time allowed to win the game after being infected

    int choice = 2; // 1 = Manual maze, 2 = Random maze

    bool inCustomMode = false; // True if player is customizing settings

    // ----------- Menu Window Initialization -----------
    InitWindow(800, 600, "Virus Maze Game"); // Open menu window
    SetTargetFPS(60);                        // Smooth 60 FPS menu

    // ----------- Main Menu Loop -----------

    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(RAYWHITE);

      DrawText("Virus Maze Game", 250, 100, 40, BLACK); // Title

      // --- Set text hover and normal colors ---

      Color hoverColor = BLUE;
      Color normalColor = BLACK;

      // --- Get mouse position for hover detection ---

      Vector2 mousePos = GetMousePosition();

      // --- Default Mode Option ---
      if (mousePos.x >= 100 && mousePos.x <= 700 && mousePos.y >= 200 &&
          mousePos.y <= 230) {
        DrawText("1. Default Mode (20x20, Easy, 50s, Random Maze)", 100, 200,
                 20, hoverColor);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          // Apply default settings

          rows = 20;
          cols = 20;
          difficulty = 1;
          virusCount = 5;
          customCountdown = 50.0f;
          choice = 2; // Random
          break;      // Exit menu and start game
        }
      } else {
        DrawText("1. Default Mode (20x20, Easy, 50s, Random Maze)", 100, 200,
                 20, normalColor);
      }

      // --- Custom Mode Option ---
      if (mousePos.x >= 100 && mousePos.x <= 700 && mousePos.y >= 250 &&
          mousePos.y <= 280) {
        DrawText("2. Custom Mode", 100, 250, 20, hoverColor);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          inCustomMode = true;
          // Launch user input UI for custom settings
          InputCustomSettings(rows, cols, virusCount, customCountdown, choice,
                              inCustomMode, difficulty);
          if (!inCustomMode)
            break; // If settings input is complete, exit menu
        }
      } else {
        DrawText("2. Custom Mode", 100, 250, 20, normalColor);
      }

      DrawText("Choose a mode to start", 100, 300, 20, BLACK);

      EndDrawing();
    }

    // --- Fake loading screen before real game ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Game Starting...", 300, 300, 30, DARKGREEN);
    EndDrawing();
    WaitTime(2);

    CloseWindow(); // Close menu window

    // ----------- Maze Setup -----------

    Maze maze;
    maze.rows = rows;
    maze.cols = cols;

    if (choice == 1)
      FillMazeManual(maze); // Manual input
    else
      FillMazeRandom(maze, virusCount, difficulty); // Random generation

    // ----------- Fullscreen Game Window Setup -----------

    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Virus Maze Game");
    ToggleFullscreen();
    SetTargetFPS(60);
    InitAudioDevice();

    // ----------- Load 3D Models and Textures -----------

    Model virusModel = LoadModel("models/new/corona.obj");

    Model weakVirusModel = LoadModel("models/weak_virus/model.obj");

    Model green_potion_model =
        LoadModel("models/green_potion/spherepotion.obj");

    Model gel_model = LoadModel("models/gel/sphere_potion_gel.obj");

    Model flag_model = LoadModel("models/flag/Mineways2Skfb.obj");

    // Load textures and apply to generated cube models

    Texture2D cubeTexture0 = LoadTexture("models/dirt/dirt.png");
    if (cubeTexture0.id == 0)
      printf("Error loading texture\n");
    Mesh cubeMesh_dirt = GenMeshCube(1.0f, 1.0f, 1.0f); // Cube with size 1.0f
    Model dirt_model = LoadModelFromMesh(cubeMesh_dirt);
    dirt_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cubeTexture0;

    Texture2D cubeTexture =
        LoadTexture("models/wall/CinderblockSloppy0005_1_S.png");
    if (cubeTexture.id == 0)
      printf("Error loading texture\n");
    Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f); // Cube with size 1.0f
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cubeTexture;

    // ----------- Player and Camera Setup -----------

    Player player = {{1.0f, 0.5f, 1.0f}, 5.0f, false, 0.0f, 0.0f};

    camera.position = {player.position.x, player.position.y + 1.6f,
                       player.position.z};
    camera.target = player.position;
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 40.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // ----------- Initialize Viruses from Maze Grid -----------

    std::vector<Virus> viruses;
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j)
        if (maze.grid[i][j] == 1 || maze.grid[i][j] == 2)
          viruses.push_back({{(float)j, 0.5f, (float)i}, maze.grid[i][j]});
    }

    // ----------- Gameplay Variables -----------

    float damageCooldown = 0.0f;
    float countdown = 5.0f;
    float disinfectantCooldown = 0.0f;
    int virusMoveCounter = 0;
    bool won = false;

    // ----------- Main Game Loop -----------

    while (!WindowShouldClose()) {
      float delta = GetFrameTime(); // Delta time between frames

      // --- Game Logic Updates ---

      HandlePlayerMovement(player, maze);
      UpdateCamera(player, delta);
      HandleMazeItemInteraction(player, maze, won, countdown, customCountdown);
      UseDisinfectant(player, countdown, disinfectantCooldown);
      // --- Virus movement every N frames ---
      if (++virusMoveCounter > 30) {
        UpdateVirusMovement(viruses, maze);
        virusMoveCounter = 0;
      }
      // --- Damage Handling ---

      damageCooldown -= delta;
      if (damageCooldown <= 0.0f) {
        for (auto &virus : viruses) {
          if ((int)virus.position.x == (int)player.position.x &&
              (int)virus.position.z == (int)player.position.z) {
            player.immunity -= (virus.type == 1 ? 1.0f : 3.0f);
            sart_countdown = (virus.type != 1); // Timer starts for strong virus
            damageCooldown = 1.0f;
            break;
          }
        }
      }

      disinfectantCooldown -= delta;
      // --- Game Over Conditions ---

      if (player.immunity <= 0 || countdown <= 0.0f) {
        DrawText("Game Over! You ran out of time or immunity.", 250, 400, 20,
                 RED);
        break;
      }

      if (sart_countdown)
        countdown -= delta;
      // --- Start Drawing Frame ---

      BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode3D(camera); // Enable 3D rendering
                           // --- Render Maze Grid ---

      for (int z = 0; z < rows; ++z) {
        for (int x = 0; x < cols; ++x) {
          Color color = LIGHTGRAY;
          Vector3 pos = {(float)x, -0.6f, (float)z};
          Vector3 rotationAxis = {0.0f, 1.0f, 0.0f};
          float rotationAngle = 0.0f;
          Vector3 scale = {1.0f, 2.2f, 1.0f};
          Color tint = WHITE;

          switch (maze.grid[z][x]) {
          case 1: { // Weak virus
            color = RED;
            float scale = 1.0f + sin(GetTime()) * 0.2f;
            float scale1 = 0.2f * scale;
            Vector3 scaledSize = {scale1, scale1, scale1};
            float rotationAngle = 270.0f;

            // Increase lift based on virus type
            pos.y += 1.8f;

            DrawModelEx(weakVirusModel, pos, {1.0f, 0.0f, 0.0f}, rotationAngle,
                        scaledSize, WHITE);
            break;
          }

          case 2: { // Strong virus
            color = BLUE;

            float scale = 1.0f + sin(GetTime()) * 0.2f;
            float scale1 = 0.1f * scale;
            Vector3 scaledSize = {scale1, scale1, scale1};
            float rotationAngle = 270.0f;

            // Increase lift based on virus type
            pos.y += 1.5f;

            // Choose model based on virus type

            DrawModelEx(virusModel, pos, {1.0f, 0.0f, 0.0f}, rotationAngle,
                        scaledSize, WHITE);

            break;
          }
          case 3: { // Green potion
            color = GREEN;
            Vector3 potionPos = pos;
            potionPos.x -= 0.3f; // Move slightly to the left on the X-axis
            potionPos.y = 0.6f;  // Float higher above the ground

            Vector3 potionScale = {0.1f, 0.1f, 0.1f}; // Smaller size

            Vector3 rotationAxis = {1.0f, 0.0f, 0.0f}; // Rotate around Y-axis
            float rotationAngle = -90.0f;              // 90 degrees rotation

            DrawModelEx(green_potion_model, potionPos, rotationAxis,
                        rotationAngle, potionScale, tint);
            break;
          }

          case 4: { // Gel
            color = PURPLE;
            Vector3 potionPos = pos;
            potionPos.x -= 0.3f; // Move slightly to the left on the X-axis
            potionPos.y = 0.6f;  // Float higher above the ground

            Vector3 potionScale = {0.1f, 0.1f, 0.1f}; // Smaller size

            Vector3 rotationAxis = {1.0f, 0.0f, 0.0f}; // Rotate around Y-axis
            float rotationAngle = -90.0f;              // 90 degrees rotation

            DrawModelEx(gel_model, potionPos, rotationAxis, rotationAngle,
                        potionScale, tint);
            break;
          }
          case 5: {
            color = BLACK;
            pos.y = 1.0f; // Lift the wall
            DrawModelEx(cubeModel, pos, rotationAxis, rotationAngle, scale,
                        tint);
            continue;
          }
          case 6: {
            color = GOLD;
            Vector3 potionPos = pos;
            potionPos.x -= 0.0f; // Move slightly to the left on the X-axis
            potionPos.y = 0.5f;  // Float higher above the ground
            float scale_local = .5f;
            Vector3 potionScale = {scale_local, scale_local,
                                   scale_local}; // Smaller size

            Vector3 rotationAxis = {1.0f, 0.0f, 0.0f}; // Rotate around Y-axis
            float rotationAngle = 0.0f;                // 90 degrees rotation

            // Draw the potion model at the specified position, rotation, and
            // scale

            DrawModelEx(flag_model, potionPos, rotationAxis, rotationAngle,
                        potionScale, tint);
            break;
          }
          }
          // Draw dirt model below any objects, including potion, tiles, etc.

          pos = {(float)x, -0.6f, (float)z};
          DrawModelEx(dirt_model, pos, rotationAxis, rotationAngle, scale,
                      tint);
        }
      }
      // Animate and draw all viruses in the scene

      for (auto &v : viruses) {
        float scale =
            1.0f + sin(GetTime()) *
                       0.2f; // Pulsating scale effect for breathing animation

        float scale1 = (v.type == 1) ? 0.2f * scale : 0.1f * scale;
        Vector3 scaledSize = {scale1, scale1, scale1};
        float rotationAngle = 270.0f;
        Vector3 drawPosition = v.position;

        // Adjust virus elevation based on type
        drawPosition.y += (v.type == 1) ? 0.8f : 0.5f;

        // Select appropriate model for virus type
        Model modelToDraw = (v.type == 1) ? weakVirusModel : virusModel;
        // Draw the virus with pulsing scale and fixed rotation

        DrawModelEx(modelToDraw, drawPosition, {1.0f, 0.0f, 0.0f},
                    rotationAngle, scaledSize, WHITE);
      }

      EndMode3D(); // End 3D rendering mode
                   // Draw HUD elements (2D)

      DrawText(TextFormat("Immunity: %.0f", player.immunity), 10, 10, 20,
               MAROON);
      if (sart_countdown)
        DrawText(TextFormat("Countdown: %.0f", countdown), 10, 40, 20, BLUE);

      EndDrawing(); // Finalize the drawing for this frame

      if (won)
        break; // Exit game loop if player has won
    }
    // Cleanup resources after game loop exits

    CloseAudioDevice();
    CloseWindow();
  }
  return 0;
}
