#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <string.h>


int enemiesKilled = 0;
// Constants
// Listen, I know these should be all capital with underscores, I forgot.
#define enemyRows 5
#define enemyCols 10
#define enemySize 25
#define enemySpeed 2
#define enemyProjSpeed 5
#define maxEnemyProj 5

#define playerSize 10
#define playerSpeed 4.5

#define fps 60
#define screenWidth 1024
#define screenHeight 768

#define projWidth 2
#define projHeight 10
#define maxProjectiles 100
#define maxLifetime 5
#define PROJ_SPEED -10

#define shieldSize 4
#define shieldCols 20
#define shieldRows 3

struct Enemy
{
    Vector2 position;
    int speed;
    int size;
    Color color;
    bool active;
};

struct Player
{
    Vector2 position;
    int size;
    Color color;
};


struct proj
{
    float lifeTime;
    Vector2 position;
    Vector2 speed;
    bool active;
};

struct shieldBlock
{
    Vector2 position;
    int size;
    Color color;
    bool active;
};

struct enemyProj
{
    Vector2 position;
    Vector2 speed;
    bool active;
    float lifeTime;
};

void drawPlayer(Vector2 position, int size, Color color)
{
    Vector2 v1 = {position.x, position.y - size};
    Vector2 v2 = {position.x - size, position.y + size};
    Vector2 v3 = {position.x + size, position.y + size};
    DrawTriangleLines(v1, v2, v3, color);
}

void spawnProjectiles(struct proj playerProj[], Vector2 position, Vector2 speed)
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (!playerProj[i].active)
        {
            playerProj[i].active = true;
            playerProj[i].position = position;
            playerProj[i].speed = speed;
            playerProj[i].lifeTime = 0;
            break;
        }
    }
}

void updateProjectiles(struct proj playerProj[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (playerProj[i].active)
        {
            playerProj[i].position = Vector2Add(playerProj[i].position, playerProj[i].speed);
            playerProj[i].lifeTime += 1.0/fps;
            if (playerProj[i].position.y < 0 || playerProj[i].lifeTime > maxLifetime)
            {
                playerProj[i].active = false;
            }
        }
    }
}

void drawProjectiles(struct proj playerProj[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (playerProj[i].active)
        {
            DrawRectangle(playerProj[i].position.x, playerProj[i].position.y, projWidth, projHeight, GREEN);
        }
    }
}

bool isMaxOnScreen(struct proj playerProj[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (!playerProj[i].active)
        {
            return false;
        }
    }
    return true;
}

void drawEnemy(struct Enemy enemies[enemyCols][enemyRows])
{
    for (int i = 0; i < enemyCols; i++)
    {
        for (int j = 0; j < enemyRows; j++)
        {
            if (enemies[i][j].active)
            {
                DrawRectangle(enemies[i][j].position.x, enemies[i][j].position.y, enemySize, enemySize, enemies[i][j].color);
            }
        }
    }
}

void shiftEnemiesDown(struct Enemy enemies[enemyCols][enemyRows])
{
    for (int i = 0; i < enemyCols; i++)
    {
        for (int j = 0; j < enemyRows; j++)
        {
            enemies[i][j].position.y += enemySize;
            enemies[i][j].speed *= -1;
        }
    }
}

void updateEnemies(struct Enemy enemies[enemyCols][enemyRows])
{

    for (int i = 0; i < enemyCols; i++)
    {
        for (int j = 0; j < enemyRows; j++)
        {
            if (enemies[i][j].active)
            {
                enemies[i][j].position.x += enemies[i][j].speed;
            }
        }
    }
    for (int i = 0; i < enemyCols; i++)
    {
        for (int j = 0; j < enemyRows; j++)
        {
            if(enemies[i][j].active && enemies[i][j].position.x + enemySize > screenWidth || 
                    enemies[i][j].position.x < 0) 
            {
                shiftEnemiesDown(enemies);
                break;
            }
        }
    }
}

void spawnEnemies(struct Enemy enemies[enemyCols][enemyRows])
{
    for (int i = 0; i < enemyCols; i++)
    {
        for (int j = 0; j < enemyRows; j++)
        {
            enemies[i][j].position.x = i * enemySize * 2 + 50;
            enemies[i][j].position.y = j * enemySize * 2 + 50;
            enemies[i][j].size = enemySize;
            enemies[i][j].active = true;
            enemies[i][j].color = RED;
            enemies[i][j].speed = enemySpeed;
        }
    }
}

bool checkEnemyCollision(struct proj playerProj[], struct Enemy enemies[enemyCols][enemyRows])
{
    bool killed = false;

    for (int i = 0; i < maxProjectiles; i++)
    {
        for (int j = 0; j < enemyCols; j++)
        {
            for (int k = 0; k < enemyRows; k++)
            {
                if (enemies[j][k].active && playerProj[i].active && (CheckCollisionRecs((Rectangle){playerProj[i].position.x, playerProj[i].position.y, projWidth, projHeight},
                            (Rectangle){enemies[j][k].position.x, enemies[j][k].position.y, enemySize, enemySize})))
                {
                    enemies[j][k].active = false;
                    playerProj[i].active = false;
                    enemiesKilled++;
                    killed = true;
                }
            }
        }
    }
    return killed;
}

//NOTE: Fixed the bug, caused by not spacing blocks propperly by thier size and overlapping. 
//To get propper spacing add (size * (col/row+1)) to give them a propper offset so they don't overlap.

void spawnShield(struct shieldBlock blocks[shieldCols][shieldRows],int x,int y)
{
    for(int i = 0; i < shieldCols; i++)
    {
        for(int j = 0; j < shieldRows; j++)
        {
            blocks[i][j].position.x = (shieldSize *(i+1)) + x;
            blocks[i][j].position.y = (shieldSize * (j+1)) + y;
            blocks[i][j].active = true;
            blocks[i][j].color = GREEN;
        }
    }
}

void drawShield(struct shieldBlock blocks[shieldCols][shieldRows])
{
    for(int i = 0; i < shieldCols; i++)
    {
        for(int j = 0; j < shieldRows; j++)
        {
            if(blocks[i][j].active)
            {
                DrawRectangle(blocks[i][j].position.x, blocks[i][j].position.y, shieldSize, shieldSize, blocks[i][j].color);
            }
        }
    }
}

void checkShieldCollision(struct proj playerProj[], struct shieldBlock blocks[shieldCols][shieldRows])
{
    for(int i = 0; i < maxProjectiles; i++)
    {
        for(int j = 0; j < shieldCols; j++)
        {
            for(int k = 0; k < shieldRows; k++)
            {
                if( blocks[j][k].active && CheckCollisionRecs((Rectangle){playerProj[i].position.x,playerProj[i].position.y,projWidth,projHeight}, 
                            (Rectangle) {blocks[j][k].position.x,blocks[j][k].position.y,shieldSize,shieldSize}))
                {
                    blocks[j][k].active = false;
                    playerProj[i].active = false;
                }
            }
        }
    }
}

int main()
{
    //NOTE: These should be in an array of structs, but I don't know how to make that work propperly.
    //      Who cares its just 3 calls instead of a for loop xdd.
    struct shieldBlock blocks[shieldCols][shieldRows];
    struct shieldBlock blocks2[shieldCols][shieldRows];
    struct shieldBlock blocks3[shieldCols][shieldRows];

    spawnShield(blocks,230,650);
    spawnShield(blocks2,450,650);
    spawnShield(blocks3,670,650);

    struct Enemy enemies[enemyCols][enemyRows];
    spawnEnemies(enemies);

    InitAudioDevice();

    struct proj playerProj[maxProjectiles];

    struct enemyProj enemyProj[maxEnemyProj];

    for (int i = 0; i < maxEnemyProj; i++)
    {
        enemyProj[i].active = false;
    }

    //NOTE: Fixes the bug where the first few playerProj are drawn at 0,0.
    for (int i = 0; i < maxProjectiles; i++)
    {
        playerProj[i].active = false;
    }

    char hudBuffer[1024];


    Music music = LoadMusicStream("music.ogg");
    SetMusicVolume(music, 0.5);
    PlayMusicStream(music);

    Sound shoot = LoadSound("shoot.wav");
    SetSoundVolume(shoot, 0.2);

    Sound killed = LoadSound("invaderkilled.wav");
    SetSoundVolume(killed,0.2);

    InitWindow(screenWidth, screenHeight, "Space game");
    SetTargetFPS(fps);

    struct Player player = {{screenWidth/2, screenHeight - playerSize*2},playerSize,GREEN};

    while(!WindowShouldClose())
    {

        UpdateMusicStream(music);

        if (IsKeyDown(KEY_A)) {player.position.x -= playerSpeed;}
        if (IsKeyDown(KEY_D)) {player.position.x += playerSpeed;}
        if (IsKeyPressed(KEY_W)) {
            if (!isMaxOnScreen(playerProj))
            {
                PlaySound(shoot);
                spawnProjectiles(playerProj, player.position, (Vector2){0, PROJ_SPEED});
            }
        }

        if (player.position.x < 0) {player.position.x = 0;}
        if (player.position.x > screenWidth) {player.position.x = screenWidth;}
        
        updateProjectiles(playerProj);
        updateEnemies(enemies);

        checkShieldCollision(playerProj, blocks);
        checkShieldCollision(playerProj, blocks2);
        checkShieldCollision(playerProj, blocks3);

        if(checkEnemyCollision(playerProj, enemies)) {PlaySound(killed);}

        BeginDrawing();
        {
            // Awlays start by clearing the screen.
            ClearBackground(BLACK);

            drawPlayer(player.position, player.size, player.color);
            
            drawProjectiles(playerProj);
            
            drawShield(blocks);
            drawShield(blocks2);
            drawShield(blocks3);

            drawEnemy(enemies);

            // Write into the hudBuffer with snprintf, then draw the hudBuffer on screen.
            snprintf(hudBuffer, sizeof(hudBuffer), "Score: %i", enemiesKilled*10);
            DrawText(hudBuffer, 10, 10, 20, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
