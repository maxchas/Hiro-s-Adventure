#include "start_screen.h"

void DrawStartScreen(Texture2D hiroBattleTexture, Texture2D slimeBattleTexture)
{
    // Background
    ClearBackground({15, 15, 20, 255});

    // Simple title glow bands
    DrawRectangle(0, 0, 800, 120, {25, 25, 35, 255});
    DrawRectangle(0, 480, 800, 120, {20, 20, 28, 255});

    // Title
    DrawText("HIRO'S ADVENTURES", 210, 70, 42, GOLD);
    DrawText("Dungeon of the Elixir", 240, 125, 28, LIGHTGRAY);

    // Subtitle / prompt
    DrawText("Press ENTER to begin", 275, 470, 24, WHITE);
    DrawText("Press F to toggle fullscreen", 250, 510, 20, GRAY);

    // Decorative sprites
    DrawTexturePro(
        hiroBattleTexture,
        {0, 0, (float)hiroBattleTexture.width, (float)hiroBattleTexture.height},
        {130, 210, 180, 180},
        {0, 0},
        0.0f,
        WHITE
    );

    DrawTexturePro(
        slimeBattleTexture,
        {0, 0, (float)slimeBattleTexture.width, (float)slimeBattleTexture.height},
        {510, 240, 140, 140},
        {0, 0},
        0.0f,
        WHITE
    );

    // Flavor text
    DrawText("A young spellblade descends into the depths", 180, 390, 22, LIGHTGRAY);
    DrawText("in search of the Elixir of Life.", 255, 420, 22, LIGHTGRAY);
}