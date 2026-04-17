#include "map.h"
#include <cmath>

void UpdateMap(float& heroX, float& heroY, bool& triggerBattle,
               float slimeX[], float slimeY[], bool slimeAlive[],
               int slimeCount, int& encounteredSlimeIndex)
{
    float speed = 2.5f;

    if (IsKeyDown(KEY_W)) heroY -= speed;
    if (IsKeyDown(KEY_S)) heroY += speed;
    if (IsKeyDown(KEY_A)) heroX -= speed;
    if (IsKeyDown(KEY_D)) heroX += speed;

    // Keep Hiro inside dungeon bounds
    if (heroX < 40) heroX = 40;
    if (heroX > 760) heroX = 760;
    if (heroY < 40) heroY = 40;
    if (heroY > 560) heroY = 560;

    // Check collision with each alive slime
    for (int i = 0; i < slimeCount; i++)
    {
        if (!slimeAlive[i])
        {
            continue;
        }

        float dx = heroX - slimeX[i];
        float dy = heroY - slimeY[i];
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 25)
        {
            triggerBattle = true;
            encounteredSlimeIndex = i;
            break;
        }
    }
}

void DrawMap(float heroX, float heroY,
             float slimeX[], float slimeY[], bool slimeAlive[],
             int slimeCount, int slimeKills,
             Texture2D hiroMapTexture, Texture2D slimeMapTexture)
{
    ClearBackground({18, 18, 22, 255});

    // Floor tiles
    int tileSize = 40;
    for (int y = 0; y < 600; y += tileSize)
    {
        for (int x = 0; x < 800; x += tileSize)
        {
            Color tileColor;

            if (((x / tileSize) + (y / tileSize)) % 2 == 0)
            {
                tileColor = {52, 52, 58, 255};
            }
            else
            {
                tileColor = {44, 44, 50, 255};
            }

            DrawRectangle(x, y, tileSize, tileSize, tileColor);
            DrawRectangleLines(x, y, tileSize, tileSize, {28, 28, 32, 255});
        }
    }

    // Outer walls
    DrawRectangle(0, 0, 800, 30, {70, 70, 78, 255});
    DrawRectangle(0, 570, 800, 30, {70, 70, 78, 255});
    DrawRectangle(0, 0, 30, 600, {70, 70, 78, 255});
    DrawRectangle(770, 0, 30, 600, {70, 70, 78, 255});

    DrawLine(0, 30, 800, 30, {110, 110, 118, 255});
    DrawLine(30, 0, 30, 600, {110, 110, 118, 255});

    // Inner walls
    DrawRectangle(160, 120, 480, 25, {78, 78, 86, 255});
    DrawRectangle(160, 455, 480, 25, {78, 78, 86, 255});

    DrawRectangle(160, 120, 25, 120, {78, 78, 86, 255});
    DrawRectangle(615, 360, 25, 120, {78, 78, 86, 255});

    // Pillars
    DrawRectangle(220, 220, 35, 35, {90, 90, 98, 255});
    DrawRectangle(545, 220, 35, 35, {90, 90, 98, 255});
    DrawRectangle(220, 325, 35, 35, {90, 90, 98, 255});
    DrawRectangle(545, 325, 35, 35, {90, 90, 98, 255});

    DrawRectangle(220, 220, 35, 6, {120, 120, 130, 255});
    DrawRectangle(545, 220, 35, 6, {120, 120, 130, 255});
    DrawRectangle(220, 325, 35, 6, {120, 120, 130, 255});
    DrawRectangle(545, 325, 35, 6, {120, 120, 130, 255});

    // Torches
    DrawCircleGradient(110, 110, 70, {255, 150, 40, 90}, BLANK);
    DrawCircleGradient(690, 110, 70, {255, 150, 40, 90}, BLANK);

    DrawRectangle(102, 110, 8, 28, BROWN);
    DrawCircle(106, 104, 7, ORANGE);
    DrawCircle(106, 100, 4, YELLOW);

    DrawRectangle(690, 110, 8, 28, BROWN);
    DrawCircle(694, 104, 7, ORANGE);
    DrawCircle(694, 100, 4, YELLOW);

    // Stairs
    for (int i = 0; i < 5; i++)
    {
        DrawRectangle(360 - i * 8, 70 + i * 8, 80 + i * 16, 6, {95, 95, 105, 255});
    }

    // Bottom shadow
    for (int i = 0; i < 120; i++)
    {
        DrawRectangle(0, 480 + i, 800, 1, {0, 0, 0, (unsigned char)(i)});
    }

    // Draw all alive slimes
    for (int i = 0; i < slimeCount; i++)
    {
        if (!slimeAlive[i])
        {
            continue;
        }

        DrawTexturePro(
            slimeMapTexture,
            {0, 0, (float)slimeMapTexture.width, (float)slimeMapTexture.height},
            {slimeX[i] - 16, slimeY[i] - 16, 32, 32},
            {0, 0},
            0.0f,
            WHITE
        );
    }

    // Draw Hiro
    DrawTexturePro(
        hiroMapTexture,
        {0, 0, (float)hiroMapTexture.width, (float)hiroMapTexture.height},
        {heroX - 18, heroY - 18, 36, 36},
        {0, 0},
        0.0f,
        WHITE
    );

    // Title and kill counter
    DrawText("Dungeon Depths", 300, 20, 24, LIGHTGRAY);
    DrawText("Find the Elixir of Life", 260, 45, 18, GRAY);
    DrawText(TextFormat("Slimes Defeated: %i", slimeKills), 560, 20, 20, GREEN);
}