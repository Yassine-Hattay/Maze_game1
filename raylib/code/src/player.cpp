#include "player.h"

Camera3D camera = {0};

void HandlePlayerMovement(Player &player, const Maze &maze) {
  static bool keyWPressed = false; // Track W key state
  static bool keySPressed = false; // Track S key state

  // Handle forward and backward movement with W and S keys
  int newX = (int)player.position.x;
  int newZ = (int)player.position.z;
 
  if (IsKeyPressed(KEY_S) &&
      !keyWPressed) { // Trigger forward movement only once per press
    newX += (int)round(sin(DEG2RAD * player.rotationY));
    newZ += (int)round(cos(DEG2RAD * player.rotationY));
    keyWPressed = true; // Set the flag to indicate the key is pressed
  }
  if (IsKeyPressed(KEY_W) &&
      !keySPressed) { // Trigger backward movement only once per press
    newX -= (int)round(sin(DEG2RAD * player.rotationY));
    newZ -= (int)round(cos(DEG2RAD * player.rotationY));
    keySPressed = true; // Set the flag to indicate the key is pressed
  }

  // Ensure the player stays within the maze and doesn't go through walls
  if (newX >= 0 && newX < maze.cols && newZ >= 0 && newZ < maze.rows &&
      maze.grid[newZ][newX] != 5) {
    player.position.x = (float)newX;
    player.position.z = (float)newZ;
  }

  // Handle camera rotation with AZERTY (A -> left, D -> right)
  const float targetRotationStep = 90.0f; // Target rotation in degrees
  const float rotationSpeed =
      3.0f; // Smooth transition speed (higher value = faster transition)
  static float targetRotation = player.rotationY; // Store the target rotation
  static bool rotatingLeft = false;  // Track if the player is rotating left
  static bool rotatingRight = false; // Track if the player is rotating right

  static float lastRotationTime = 0.0f; // Time of the last rotation
  const float rotationCooldown =
      0.25f; // Minimum time (in seconds) between rotations

  if (IsKeyDown(KEY_A)) {
    float currentTime = GetTime(); // Get the current time
    if (!rotatingLeft && (currentTime - lastRotationTime > rotationCooldown)) {
      targetRotation -= targetRotationStep; // Rotate 90 degrees to the left
      rotatingLeft = true; // Set the flag to indicate the rotation has started
      lastRotationTime = currentTime; // Update the last rotation time
    }
  } else {
    rotatingLeft = false; // Reset the flag when the key is released
  }

  // Rotate right (D key)
  if (IsKeyDown(KEY_D)) {
    float currentTime = GetTime(); // Get the current time
    if (!rotatingRight && (currentTime - lastRotationTime > rotationCooldown)) {
      targetRotation += targetRotationStep; // Rotate 90 degrees to the right
      rotatingRight = true; // Set the flag to indicate the rotation has started
      lastRotationTime = currentTime; // Update the last rotation time
    }
  } else {
    rotatingRight = false; // Reset the flag when the key is released
  }

  // Smoothly rotate towards the target rotation
  if (player.rotationY < targetRotation) {
    player.rotationY += rotationSpeed;
    if (player.rotationY > targetRotation)
      player.rotationY = targetRotation; // Snap to target if overshot
  } else if (player.rotationY > targetRotation) {
    player.rotationY -= rotationSpeed;
    if (player.rotationY < targetRotation)
      player.rotationY = targetRotation; // Snap to target if overshot
  }

  // Reset the key press states and rotation flags when keys are released
  if (!IsKeyDown(KEY_W))
    keyWPressed = false;
  if (!IsKeyDown(KEY_S))
    keySPressed = false;

  // Reset rotation flags when keys are released
  if (!IsKeyDown(KEY_A))
    rotatingLeft = false;
  if (!IsKeyDown(KEY_D))
    rotatingRight = false;
}




void UpdateCamera(Player &player, float delta) {
  // Update the player's rotation based on D and Q keys (steering)
  if (IsKeyPressed(KEY_D))
    player.rotationY += 5.0f; // Turn right
  if (IsKeyPressed(KEY_Q))
    player.rotationY -= 5.0f; // Turn left

  // Make sure the rotation angle stays within 0 to 360 degrees
  if (player.rotationY > 360.0f)
    player.rotationY -= 360.0f;
  if (player.rotationY < 0.0f)
    player.rotationY += 360.0f;

  // Update camera position based on player's position and rotation
  player.rotationX =
      Clamp(player.rotationX, -89.0f, 89.0f); // Prevent camera flipping
  // Move the camera slightly forward
  const float moveForwardDistance = -4.5f; // Small distance to move forward
  camera.position = {
      player.position.x + sin(DEG2RAD * player.rotationY) * 5.0f +
          sin(DEG2RAD * player.rotationY) * moveForwardDistance,
      player.position.y + 1.5f,
      player.position.z + cos(DEG2RAD * player.rotationY) * 5.0f +
          cos(DEG2RAD * player.rotationY) * moveForwardDistance};
  camera.target = {player.position.x, player.position.y + 1.4f,
                   player.position.z};
}



void UseDisinfectant(Player &player, float &countdown, float &cooldown) {
  if (IsKeyPressed(KEY_E) && player.immunity > 0 &&
      cooldown <= 0) { // Press E to use disinfectant
    player.immunity = std::min(player.immunity + 2.0f,
                               5.0f); // Use disinfectant to recover immunity
    cooldown = 5.0f;                  // Set cooldown for disinfectant
  }
}
