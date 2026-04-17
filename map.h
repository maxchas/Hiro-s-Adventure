#pragma once
#include "raylib.h"

// ------------------------------------------------------------
// Number of slimes that exist on the dungeon map at one time
// ------------------------------------------------------------
const int MAP_SLIME_COUNT = 3;

// ------------------------------------------------------------
// Updates Hiro's movement on the map and checks if he touches
// a slime, which triggers a battle
// ------------------------------------------------------------
void UpdateMap(float& heroX, float& heroY, bool& triggerBattle,
               float slimeX[], float slimeY[], bool slimeAlive[],
               int slimeCount, int& encounteredSlimeIndex);

// ------------------------------------------------------------
// Draws the dungeon map, Hiro, all alive slimes, and the
// slime kill counter
// ------------------------------------------------------------
void DrawMap(float heroX, float heroY,
             float slimeX[], float slimeY[], bool slimeAlive[],
             int slimeCount, int slimeKills,
             Texture2D hiroMapTexture, Texture2D slimeMapTexture);