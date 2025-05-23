#include "UI.h"

void InputCustomSettings(int &rows, int &cols, int &virusCount, float &customCountdown, int &choice, bool &inCustomMode, int &difficulty) {
    std::string rowInput = std::to_string(rows), colInput = std::to_string(cols), countdownInput = std::to_string(customCountdown);
    bool isEditingRows = false, isEditingCols = false, isEditingCountdown = false;
    bool showError = false;

    // Difficulty buttons
    int selectedDifficulty = difficulty; // 1 = Easy, 2 = Medium, 3 = Hard
    int selectedMazeType = choice; // 1 = Custom Maze, 2 = Random Maze

    while (!WindowShouldClose()) {
        BeginDrawing(); 
        ClearBackground(RAYWHITE);

        DrawText("Enter custom settings:", 120, 360, 30, BLACK);

        // Draw input fields
        DrawText("Rows: ", 120, 400, 20, BLACK);
        DrawRectangle(170, 395, 100, 30, LIGHTGRAY);
        DrawText(rowInput.c_str(), 175, 400, 20, BLACK);

        DrawText("Columns: ", 120, 440, 20, BLACK);
        DrawRectangle(170, 435, 100, 30, LIGHTGRAY);
        DrawText(colInput.c_str(), 175, 440, 20, BLACK);

        DrawText("Countdown: ", 120, 480, 20, BLACK);
        DrawRectangle(170, 475, 100, 30, LIGHTGRAY);
        DrawText(countdownInput.c_str(), 175, 480, 20, BLACK);

        // Draw Difficulty selection
        DrawText("Difficulty:", 300, 400, 20, BLACK);
        DrawRectangle(400, 395, 80, 30, selectedDifficulty == 1 ? DARKGREEN : LIGHTGRAY);
        DrawText("Easy", 410, 400, 20, selectedDifficulty == 1 ? WHITE : BLACK);
        DrawRectangle(490, 395, 80, 30, selectedDifficulty == 2 ? ORANGE : LIGHTGRAY);
        DrawText("Medium", 495, 400, 20, selectedDifficulty == 2 ? WHITE : BLACK);
        DrawRectangle(580, 395, 80, 30, selectedDifficulty == 3 ? RED : LIGHTGRAY);
        DrawText("Hard", 595, 400, 20, selectedDifficulty == 3 ? WHITE : BLACK);

        // Draw Maze Type selection
        DrawText("Maze Type:", 300, 440, 20, BLACK);
        DrawRectangle(400, 435, 120, 30, selectedMazeType == 1 ? SKYBLUE : LIGHTGRAY);
        DrawText("Custom", 420, 440, 20, selectedMazeType == 1 ? WHITE : BLACK);
        DrawRectangle(530, 435, 120, 30, selectedMazeType == 2 ? SKYBLUE : LIGHTGRAY);
        DrawText("Random", 550, 440, 20, selectedMazeType == 2 ? WHITE : BLACK);

        // Handle keyboard input
        for (int key = 32; key <= 126; key++) {
            if (IsKeyPressed(key)) {
                if (isEditingRows) rowInput += (char)key;
                else if (isEditingCols) colInput += (char)key;
                else if (isEditingCountdown) countdownInput += (char)key;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (isEditingRows && !rowInput.empty()) rowInput.pop_back();
            else if (isEditingCols && !colInput.empty()) colInput.pop_back();
            else if (isEditingCountdown && !countdownInput.empty()) countdownInput.pop_back();
        }

        // Handle clicks
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Input boxes
            isEditingRows = CheckCollisionPointRec(mousePos, {170, 395, 100, 30});
            isEditingCols = CheckCollisionPointRec(mousePos, {170, 435, 100, 30});
            isEditingCountdown = CheckCollisionPointRec(mousePos, {170, 475, 100, 30});

            // Difficulty selection
            if (CheckCollisionPointRec(mousePos, {400, 395, 80, 30})) selectedDifficulty = 1;
            if (CheckCollisionPointRec(mousePos, {490, 395, 80, 30})) selectedDifficulty = 2;
            if (CheckCollisionPointRec(mousePos, {580, 395, 80, 30})) selectedDifficulty = 3;

            // Maze type selection
            if (CheckCollisionPointRec(mousePos, {400, 435, 120, 30})) selectedMazeType = 1;
            if (CheckCollisionPointRec(mousePos, {530, 435, 120, 30})) selectedMazeType = 2;

            // Confirm
            if (CheckCollisionPointRec(mousePos, {120, 520, 150, 40})) {
                try {
                    rows = std::stoi(rowInput);
                    cols = std::stoi(colInput);
                    customCountdown = std::stof(countdownInput);
                    difficulty = selectedDifficulty;
                    choice = selectedMazeType;
                    virusCount = (difficulty == 1) ? 5 : (difficulty == 2) ? 10 : 15;
                    inCustomMode = false;
                    return;
                } catch (...) {
                    showError = true;
                }
            }

            // Delete
            if (CheckCollisionPointRec(mousePos, {280, 520, 150, 40})) {
                rowInput.clear();
                colInput.clear();
                countdownInput.clear();
                showError = false;
            }
        }

        // Buttons
        DrawRectangle(120, 520, 150, 40, DARKGREEN);
        DrawText("Confirm", 145, 530, 20, WHITE);
        DrawRectangle(280, 520, 150, 40, RED);
        DrawText("Delete", 320, 530, 20, WHITE);

        if (showError)
            DrawText("Invalid input, please check your values.", 120, 570, 20, RED);

        EndDrawing();
    }

    inCustomMode = false;
}

