#include "raylib.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include "map.h"
#include "start_screen.h"

using namespace std;

// ------------------------------------------------------------
// Character struct
// Stores the data for Hiro and the slime used in battle
// ------------------------------------------------------------
struct Character
{
    string name;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int x;
    int y;
};

// ------------------------------------------------------------
// Battle states
// Controls what is happening during battle
// ------------------------------------------------------------
enum GameState
{
    PLAYER_TURN,
    PLAYER_MAGIC_ANIMATION,
    ENEMY_TURN,
    ENEMY_MAGIC_ANIMATION,
    WIN,
    LOSE
};

// ------------------------------------------------------------
// DrawDungeonBackground()
// Draws the battle background for the dungeon
// ------------------------------------------------------------
void DrawDungeonBackground()
{
    ClearBackground({28, 28, 32, 255});

    int brickWidth = 80;
    int brickHeight = 40;

    for (int y = 0; y < 600; y += brickHeight)
    {
        for (int x = 0; x < 800; x += brickWidth)
        {
            int offset = ((y / brickHeight) % 2) * (brickWidth / 2);

            Color brickColor = {
                (unsigned char)(48 + rand() % 10),
                (unsigned char)(48 + rand() % 10),
                (unsigned char)(54 + rand() % 10),
                255
            };

            DrawRectangle(x - offset, y, brickWidth, brickHeight, brickColor);
            DrawRectangleLines(x - offset, y, brickWidth, brickHeight, {18, 18, 22, 255});
        }
    }

    DrawRectangle(0, 430, 800, 170, {18, 18, 20, 255});

    for (int i = 0; i < 80; i++)
    {
        DrawRectangle(0, 350 + i, 800, 1, {0, 0, 0, (unsigned char)(i * 2)});
    }

    DrawCircleGradient(85, 165, 90, {255, 140, 40, 90}, BLANK);
    DrawCircleGradient(715, 165, 90, {255, 140, 40, 90}, BLANK);

    DrawRectangle(80, 170, 10, 35, BROWN);
    DrawCircle(85, 160, 9, ORANGE);
    DrawCircle(85, 155, 5, YELLOW);

    DrawRectangle(710, 170, 10, 35, BROWN);
    DrawCircle(715, 160, 9, ORANGE);
    DrawCircle(715, 155, 5, YELLOW);
}

// ------------------------------------------------------------
// DrawCharacterPanel()
// Draws the HP and MP panel for a character
// ------------------------------------------------------------
void DrawCharacterPanel(const Character& character, int x, int y, Color panelColor)
{
    DrawRectangle(x, y, 280, 160, panelColor);
    DrawText(character.name.c_str(), x + 20, y + 15, 26, WHITE);

    DrawText(TextFormat("HP: %i / %i", character.hp, character.maxHp), x + 20, y + 50, 20, WHITE);
    DrawRectangle(x + 20, y + 80, 200, 20, DARKGRAY);
    DrawRectangle(x + 20, y + 80, (character.hp * 200) / character.maxHp, 20, GREEN);

    DrawText(TextFormat("MP: %i / %i", character.mp, character.maxMp), x + 20, y + 110, 20, WHITE);
    DrawRectangle(x + 20, y + 140, 200, 20, DARKGRAY);
    DrawRectangle(x + 20, y + 140, (character.mp * 200) / character.maxMp, 20, SKYBLUE);
}

// ------------------------------------------------------------
// ResetBattle()
// Resets HP, MP, menu selection, and message for battle
// ------------------------------------------------------------
void ResetBattle(Character& hero, Character& slime, int& selectedOption, string& message, GameState& gameState)
{
    hero.hp = hero.maxHp;
    hero.mp = hero.maxMp;
    slime.hp = slime.maxHp;
    slime.mp = slime.maxMp;

    selectedOption = 0;
    message = "Choose an action:";
    gameState = PLAYER_TURN;
}

// ------------------------------------------------------------
// UpdatePlayerMagicAnimation()
// Moves Hiro's fireball and applies damage when it reaches
// the slime
// ------------------------------------------------------------
void UpdatePlayerMagicAnimation(
    float& fireballX,
    bool& playerCastingFireball,
    Character& slime,
    GameState& gameState,
    string& message,
    double& enemyTurnStartTime,
    bool& slimeHit,
    double& slimeHitStartTime)
{
    fireballX += 8.0f;

    if (fireballX >= slime.x - 20)
    {
        playerCastingFireball = false;

        slime.hp -= 25;
        if (slime.hp < 0)
        {
            slime.hp = 0;
        }

        slimeHit = true;
        slimeHitStartTime = GetTime();

        if (slime.hp <= 0)
        {
            gameState = WIN;
            message = "You burned the slime down!";
        }
        else
        {
            message = "Fireball hit for 25 damage!";
            gameState = ENEMY_TURN;
            enemyTurnStartTime = GetTime();
        }
    }
}

// ------------------------------------------------------------
// UpdateEnemyMagicAnimation()
// Moves the slime's spell and applies damage when it reaches
// Hiro
// ------------------------------------------------------------
void UpdateEnemyMagicAnimation(
    float& gooBlastX,
    bool& enemyCastingMagic,
    Character& hero,
    GameState& gameState,
    string& message,
    bool& heroHit,
    double& heroHitStartTime)
{
    gooBlastX -= 8.0f;

    if (gooBlastX <= hero.x + 20)
    {
        enemyCastingMagic = false;

        hero.hp -= 18;
        if (hero.hp < 0)
        {
            hero.hp = 0;
        }

        heroHit = true;
        heroHitStartTime = GetTime();

        if (hero.hp <= 0)
        {
            gameState = LOSE;
            message = "The slime's Goo Blast defeated you!";
        }
        else
        {
            message = "Goo Blast hit for 18 damage!";
            gameState = PLAYER_TURN;
        }
    }
}

int main()
{
    // --------------------------------------------------------
    // Virtual game resolution
    // Keep all gameplay and drawing based on this size
    // --------------------------------------------------------
    const int gameWidth = 800;
    const int gameHeight = 600;

    // --------------------------------------------------------
    // Actual window size
    // --------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Hiro's Adventure");
    SetTargetFPS(60);
    // Set music
    InitAudioDevice();
    //
    // --------------------------------------------------------
    // Render texture for clean scaling
    // --------------------------------------------------------
    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);

    // --------------------------------------------------------
    // Load textures for map and battle sprites
    // --------------------------------------------------------
    Texture2D hiroMapTexture = LoadTexture("assets/hiro_map.png");
    Texture2D hiroBattleTexture = LoadTexture("assets/hiro_battle.png");
    Texture2D slimeMapTexture = LoadTexture("assets/slime_map.png");
    Texture2D slimeBattleTexture = LoadTexture("assets/slime_battle.png");
    // Load music and set volume
    Music menuMusic = LoadMusicStream("assets/final_ascent.wav");
    Music battleMusic = LoadMusicStream("assets/es_red phoenix.wav");
    SetMusicVolume(menuMusic, 0.5f);
    SetMusicVolume(battleMusic, 0.5f);

     // --------------------------------------------------------
    // Music states
    enum MusicState
{
    TITLE_MUSIC,
    BATTLE_MUSIC,
};
    MusicState currentMusicState = TITLE_MUSIC;
    PlayMusicStream(menuMusic);


    // --------------------------------------------------------
    // ---------------------------------------------------------
    srand(time(NULL));

    // --------------------------------------------------------
    // Battle characters
    // --------------------------------------------------------
    Character hero = {"Hero", 100, 100, 50, 50, 240, 235};
    Character slime = {"Slime", 90, 90, 30, 30, 560, 220};

    // Scene states
    // --------------------------------------------------------
    enum Scene
{
    START_SCENE,
    MAP_SCENE,
    BATTLE_SCENE,
    END_SCENE
};

    Scene currentScene = START_SCENE;

    // --------------------------------------------------------
    // Hiro map position
    // --------------------------------------------------------
    float heroMapX = 400.0f;
    float heroMapY = 300.0f;

    // --------------------------------------------------------
    // Slime map positions
    // --------------------------------------------------------
    float slimeMapX[MAP_SLIME_COUNT] = {200.0f, 600.0f, 400.0f};
    float slimeMapY[MAP_SLIME_COUNT] = {200.0f, 200.0f, 500.0f};
    bool slimeAlive[MAP_SLIME_COUNT] = {true, true, true};

    int encounteredSlimeIndex = -1;
    int slimeKills = 0;
    bool triggerBattle = false;

    // --------------------------------------------------------
    // Battle menu data
    // --------------------------------------------------------
    int selectedOption = 0;
    const int totalOptions = 3;

    string message = "Choose an action:";
    GameState gameState = PLAYER_TURN;

    // --------------------------------------------------------
    // Enemy timing
    // --------------------------------------------------------
    double enemyTurnStartTime = 0.0;
    double enemyDelay = 1.0;

    // --------------------------------------------------------
    // Spell animation flags
    // --------------------------------------------------------
    bool playerCastingFireball = false;
    bool enemyCastingMagic = false;

    // --------------------------------------------------------
    // Spell positions
    // --------------------------------------------------------
    float fireballX = 0.0f;
    float fireballY = 0.0f;

    float gooBlastX = 0.0f;
    float gooBlastY = 0.0f;

    // --------------------------------------------------------
    // Hit flash effect data
    // --------------------------------------------------------
    bool heroHit = false;
    bool slimeHit = false;
    double heroHitStartTime = 0.0;
    double slimeHitStartTime = 0.0;
    double hitFlashDuration = 0.30;

    while (!WindowShouldClose())
    {
        // ----------------------------------------------------
        // Fullscreen toggle
        // ----------------------------------------------------
        if (IsKeyPressed(KEY_F))
        {
            ToggleFullscreen();
        }

        // Music update
        if (currentMusicState == TITLE_MUSIC)
        {
            UpdateMusicStream(menuMusic);
        }
        else if (currentMusicState == BATTLE_MUSIC)
        {
            UpdateMusicStream(battleMusic);
        }
        // ====================================================

        // ====================================================
        // UPDATE LOGIC
        // ====================================================
        
        
        if (currentScene == START_SCENE)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScene = MAP_SCENE;
            }
        }   
        else if (currentScene == MAP_SCENE)
        {
            UpdateMap(heroMapX, heroMapY, triggerBattle,
                      slimeMapX, slimeMapY, slimeAlive,
                      MAP_SLIME_COUNT, encounteredSlimeIndex);

            if (triggerBattle)
            {
                triggerBattle = false;
                currentScene = BATTLE_SCENE;
                
                if (currentMusicState != BATTLE_MUSIC)
                {
                    StopMusicStream(menuMusic);
                    PlayMusicStream(battleMusic);
                    currentMusicState = BATTLE_MUSIC;
                }

                slime.hp = slime.maxHp;
                slime.mp = slime.maxMp;

                selectedOption = 0;
                message = "A slime appears!";
                gameState = PLAYER_TURN;

                playerCastingFireball = false;
                enemyCastingMagic = false;
                heroHit = false;
                slimeHit = false;
            }
        }
        else if (currentScene == BATTLE_SCENE)
        {
            if (heroHit && GetTime() - heroHitStartTime >= hitFlashDuration)
            {
                heroHit = false;
            }

            if (slimeHit && GetTime() - slimeHitStartTime >= hitFlashDuration)
            {
                slimeHit = false;
            }

            if (gameState == PLAYER_TURN)
            {
                if (IsKeyPressed(KEY_UP))
                {
                    selectedOption--;
                    if (selectedOption < 0)
                    {
                        selectedOption = totalOptions - 1;
                    }
                }

                if (IsKeyPressed(KEY_DOWN))
                {
                    selectedOption++;
                    if (selectedOption >= totalOptions)
                    {
                        selectedOption = 0;
                    }
                }

                if (IsKeyPressed(KEY_ENTER))
                {
                    if (selectedOption == 0)
                    {
                        slime.hp -= 15;
                        if (slime.hp < 0)
                        {
                            slime.hp = 0;
                        }

                        slimeHit = true;
                        slimeHitStartTime = GetTime();

                        message = hero.name + " attacked the slime!";

                        if (slime.hp <= 0)
                        {
                            gameState = WIN;
                            message = "You defeated the slime!";
                        }
                        else
                        {
                            gameState = ENEMY_TURN;
                            enemyTurnStartTime = GetTime();
                        }
                    }
                    else if (selectedOption == 1)
                    {
                        if (hero.mp >= 5)
                        {
                            hero.mp -= 5;
                            hero.hp += 12;

                            if (hero.hp > hero.maxHp)
                            {
                                hero.hp = hero.maxHp;
                            }

                            message = hero.name + " healed 12 HP!";
                            gameState = ENEMY_TURN;
                            enemyTurnStartTime = GetTime();
                        }
                        else
                        {
                            message = "Not enough MP to Heal!";
                        }
                    }
                    else if (selectedOption == 2)
                    {
                        if (hero.mp >= 10)
                        {
                            hero.mp -= 10;
                            message = hero.name + " casts Fireball!";

                            fireballX = 270.0f;
                            fireballY = 270.0f;
                            playerCastingFireball = true;
                            gameState = PLAYER_MAGIC_ANIMATION;
                        }
                        else
                        {
                            message = "Not enough MP for Fireball!";
                        }
                    }
                }
            }
            else if (gameState == PLAYER_MAGIC_ANIMATION)
            {
                UpdatePlayerMagicAnimation(
                    fireballX,
                    playerCastingFireball,
                    slime,
                    gameState,
                    message,
                    enemyTurnStartTime,
                    slimeHit,
                    slimeHitStartTime);
            }
            else if (gameState == ENEMY_TURN)
            {
                if (GetTime() - enemyTurnStartTime >= enemyDelay)
                {
                    int enemyChoice = rand() % 2;

                    if (enemyChoice == 1 && slime.mp >= 8)
                    {
                        slime.mp -= 8;
                        message = "The slime casts Goo Blast!";

                        gooBlastX = 520.0f;
                        gooBlastY = 280.0f;
                        enemyCastingMagic = true;
                        gameState = ENEMY_MAGIC_ANIMATION;
                    }
                    else
                    {
                        hero.hp -= 10;
                        if (hero.hp < 0)
                        {
                            hero.hp = 0;
                        }

                        heroHit = true;
                        heroHitStartTime = GetTime();

                        if (hero.hp <= 0)
                        {
                            gameState = LOSE;
                            message = "The slime defeated you!";
                        }
                        else
                        {
                            message = "The slime attacked you!";
                            gameState = PLAYER_TURN;
                        }
                    }
                }
            }
            else if (gameState == ENEMY_MAGIC_ANIMATION)
            {
                UpdateEnemyMagicAnimation(
                    gooBlastX,
                    enemyCastingMagic,
                    hero,
                    gameState,
                    message,
                    heroHit,
                    heroHitStartTime);
            }
            else if (gameState == WIN)
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScene = MAP_SCENE;

                    // SWITCH BACK TO MAP/TITLE MUSIC HERE
                    if (currentMusicState != TITLE_MUSIC)
                    {
                        StopMusicStream(battleMusic);
                        PlayMusicStream(menuMusic);
                        currentMusicState = TITLE_MUSIC;
                    }

                    if (encounteredSlimeIndex >= 0 && encounteredSlimeIndex < MAP_SLIME_COUNT)
                    {
                        slimeAlive[encounteredSlimeIndex] = false;
                        slimeKills++;
                    }

                    if (slimeKills >= 3)
                    {
                        currentScene = END_SCENE;
                    }
                }
            }
            else if (gameState == LOSE)
            {
                if (IsKeyPressed(KEY_R))
                {
                    hero.hp = hero.maxHp;
                    hero.mp = hero.maxMp;

                    currentScene = MAP_SCENE;

                    heroMapX = 400.0f;
                    heroMapY = 300.0f;

                    selectedOption = 0;
                    message = "Choose an action:";
                    gameState = PLAYER_TURN;

                    playerCastingFireball = false;
                    enemyCastingMagic = false;
                    heroHit = false;
                    slimeHit = false;
                }
            }
        }

        // ====================================================
        // DRAW TO RENDER TEXTURE
        // ====================================================
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (currentScene == START_SCENE)
        {
            DrawStartScreen(hiroBattleTexture, slimeBattleTexture);
        }
        else if (currentScene == MAP_SCENE)
        {
            DrawMap(heroMapX, heroMapY,
                slimeMapX, slimeMapY, slimeAlive,
                MAP_SLIME_COUNT, slimeKills,
                hiroMapTexture, slimeMapTexture);
        }   
        else if (currentScene == BATTLE_SCENE)
        {
            DrawDungeonBackground();

            DrawCharacterPanel(hero, 60, 20, BLUE);
            DrawCharacterPanel(slime, 460, 20, RED);

            Color heroTint = heroHit ? RED : WHITE;
            Color slimeTint = slimeHit ? RED : WHITE;

            DrawTexturePro(
                hiroBattleTexture,
                {0, 0, (float)hiroBattleTexture.width, (float)hiroBattleTexture.height},
                {150, 180, 140, 140},
                {0, 0},
                0.0f,
                heroTint
            );

            DrawTexturePro(
                slimeBattleTexture,
                {0, 0, (float)slimeBattleTexture.width, (float)slimeBattleTexture.height},
                {500, 210, 120, 120},
                {0, 0},
                0.0f,
                slimeTint
            );

            if (playerCastingFireball)
            {
                DrawCircle((int)fireballX, (int)fireballY, 18, ORANGE);
                DrawCircle((int)fireballX, (int)fireballY, 10, YELLOW);
            }

            if (enemyCastingMagic)
            {
                DrawCircle((int)gooBlastX, (int)gooBlastY, 18, LIME);
                DrawCircle((int)gooBlastX, (int)gooBlastY, 10, GREEN);
            }

            DrawRectangle(100, 360, 600, 80, GRAY);
            DrawText(message.c_str(), 120, 390, 24, WHITE);

            DrawRectangle(100, 460, 350, 120, BLACK);

            if (gameState == PLAYER_TURN)
            {
                DrawText(selectedOption == 0 ? "> Attack" : "  Attack", 120, 475, 24, WHITE);
                DrawText(selectedOption == 1 ? "> Heal" : "  Heal", 120, 510, 24, WHITE);
                DrawText(selectedOption == 2 ? "> Fireball" : "  Fireball", 120, 545, 24, WHITE);
            }
            else if (gameState == ENEMY_TURN)
            {
                DrawText("Enemy is thinking...", 120, 510, 24, WHITE);
            }
            else if (gameState == PLAYER_MAGIC_ANIMATION)
            {
                DrawText("Fireball is flying...", 120, 510, 24, ORANGE);
            }
            else if (gameState == ENEMY_MAGIC_ANIMATION)
            {
                DrawText("Goo Blast is flying...", 120, 510, 24, LIME);
            }
            else if (gameState == WIN)
            {
                DrawText("You win! Press ENTER to return to map.", 120, 510, 24, GREEN);
            }
            else if (gameState == LOSE)
            {
                DrawText("You lose! Press R to return to map.", 120, 510, 24, RED);
            }
        }
        else if (currentScene == END_SCENE)
        {
            ClearBackground(BLACK);

            const char* line1 = "You'll see more of Hiro's adventure soon...";
            const char* line2 = "Thanks for playing!";

            int line1Width = MeasureText(line1, 30);
            int line2Width = MeasureText(line2, 40);

            DrawText(line1, (gameWidth - line1Width) / 2, 220, 30, LIGHTGRAY);
            DrawText(line2, (gameWidth - line2Width) / 2, 300, 40, GREEN);
            DrawText("Press ESC to close the game", 250, 380, 20, GRAY);
        }

        EndTextureMode();

        // ====================================================
        // DRAW SCALED TO WINDOW
        // ====================================================
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            target.texture,
            {0, 0, (float)target.texture.width, -(float)target.texture.height},
            {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            {0, 0},
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    // --------------------------------------------------------
    // Unload resources before closing
    // --------------------------------------------------------
    UnloadRenderTexture(target);

    UnloadTexture(hiroMapTexture);
    UnloadTexture(hiroBattleTexture);
    UnloadTexture(slimeMapTexture);
    UnloadTexture(slimeBattleTexture);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(battleMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}