#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

// Function to draw a player
void drawPlayer(Vector2 position, int size, Color color)
{
    Vector2 v1 = {position.x, position.y - size};
    Vector2 v2 = {position.x - size, position.y + size};
    Vector2 v3 = {position.x + size, position.y + size};
    DrawTriangleLines(v1, v2, v3, color);
}

int enemysKilled = 0;

// Constants
const int enemyRows = 5;
const int enemyCols = 10;
const int enemySize = 25;
const int enemySpeed = 3;

const int playerSize = 10;
const float playerSpeed = 4.5;

const int fps = 60;
const int screenWidth = 1024;
const int screenHeight = 768;

const int projWidth = 2;
const int projHeight = 10;
const int maxProjectiles = 100;
const int maxLifetime = 5;
const int PROJ_SPEED = -10;

const int shieldSize = 4;
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

void spawnProjectiles(struct proj projectiles[], Vector2 position, Vector2 speed)
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (!projectiles[i].active)
        {
            projectiles[i].active = true;
            projectiles[i].position = position;
            projectiles[i].speed = speed;
            projectiles[i].lifeTime = 0;
            break;
        }
    }
}

void updateProjectiles(struct proj projectiles[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (projectiles[i].active)
        {
            projectiles[i].position = Vector2Add(projectiles[i].position, projectiles[i].speed);
            projectiles[i].lifeTime += 1.0/fps;
            if (projectiles[i].position.y < 0 || projectiles[i].lifeTime > maxLifetime)
            {
                projectiles[i].active = false;
            }
        }
    }
}

void drawProjectiles(struct proj projectiles[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (projectiles[i].active)
        {
            DrawRectangle(projectiles[i].position.x, projectiles[i].position.y, projWidth, projHeight, GREEN);
        }
    }
}

bool isMaxOnScreen(struct proj projectiles[])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (!projectiles[i].active)
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
    
    if (enemies[enemyCols-1][enemyRows-1].position.x + enemySize > screenWidth || enemies[0][0].position.x < 0)
    {
        shiftEnemiesDown(enemies);
        return;
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

void checkEnemyCollision(struct proj projectiles[], struct Enemy enemies[enemyCols][enemyRows])
{
    for (int i = 0; i < maxProjectiles; i++)
    {
        for (int j = 0; j < enemyCols; j++)
        {
            for (int k = 0; k < enemyRows; k++)
            {
                if (enemies[j][k].active && projectiles[i].active && CheckCollisionRecs((Rectangle){projectiles[i].position.x, projectiles[i].position.y, projWidth, projHeight},
                            (Rectangle){enemies[j][k].position.x, enemies[j][k].position.y, enemySize, enemySize}))
                {
                    enemies[j][k].active = false;
                    projectiles[i].active = false;
                    enemysKilled++;
                }
            }
        }
    }
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

void checkShieldCollision(struct proj projectiles[], struct shieldBlock blocks[shieldCols][shieldRows])
{
    for(int i = 0; i < maxProjectiles; i++)
    {
        for(int j = 0; j < shieldCols; j++)
        {
            for(int k = 0; k < shieldRows; k++)
            {
                if( blocks[j][k].active && CheckCollisionRecs((Rectangle){projectiles[i].position.x,projectiles[i].position.y,projWidth,projHeight}, 
                            (Rectangle) {blocks[j][k].position.x,blocks[j][k].position.y,shieldSize,shieldSize}))
                {
                    blocks[j][k].active = false;
                    projectiles[i].active = false;
                }
            }
        }
    }
}
int main()
{
    //NOTE: These should be in an array of structs, but I don't know how to make that work propperly.
    //      who cares its just 3 calls instead of a for loop xdd.

    struct shieldBlock blocks[shieldCols][shieldRows];
    struct shieldBlock blocks2[shieldCols][shieldRows];
    struct shieldBlock blocks3[shieldCols][shieldRows];

    spawnShield(blocks,230,650);
    spawnShield(blocks2,450,650);
    spawnShield(blocks3,670,650);

    struct Enemy enemies[enemyCols][enemyRows];
    spawnEnemies(enemies);

    InitAudioDevice();
    struct proj projectiles[maxProjectiles];

    Sound shoot = LoadSound("shoot.wav");
    Music music = LoadMusicStream("music.ogg");
    SetMusicVolume(music, 0.5);
    PlayMusicStream(music);
    SetSoundVolume(shoot, 0.2);

    InitWindow(screenWidth, screenHeight, "Space game");
    SetTargetFPS(fps);

    struct Player player = {{screenWidth/2, screenHeight - playerSize*2},playerSize,GREEN};
    while(!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (IsKeyDown(KEY_A)) {player.position.x -= playerSpeed;}
        if (IsKeyDown(KEY_D)) {player.position.x += playerSpeed;}
        if (IsKeyPressed(KEY_W)) {
            if (!isMaxOnScreen(projectiles))
            {
                PlaySound(shoot);
                spawnProjectiles(projectiles, player.position, (Vector2){0, PROJ_SPEED});
            }
        }

        if (player.position.x < 0) {player.position.x = screenWidth;}
        if (player.position.x > screenWidth) {player.position.x = 0;}
        updateProjectiles(projectiles);
        updateEnemies(enemies);
        checkShieldCollision(projectiles, blocks);
        checkShieldCollision(projectiles, blocks2);
        checkShieldCollision(projectiles, blocks3);
        checkEnemyCollision(projectiles, enemies);
        BeginDrawing();
        {
            // Awlays start by clearing the screen.
            ClearBackground(BLACK);
            drawProjectiles(projectiles);
            drawPlayer(player.position, player.size, player.color);
            drawShield(blocks);
            drawShield(blocks2);
            drawShield(blocks3);
            drawEnemy(enemies);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
