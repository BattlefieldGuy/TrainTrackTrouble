/* command to run
g++ Main.cpp -o train.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/

#include "raylib.h"

int tileSize = 32;

int screenWidth = 1800;
int screenHeight = 800;

int main()
{
    InitWindow(screenWidth, screenHeight, "Train Track Trouble");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
#pragma region - grid vriables -
        int gridRows = screenWidth / tileSize;
        int gridCols = screenHeight / tileSize;

#pragma endregion

        BeginDrawing();
        ClearBackground(RAYWHITE);

#pragma region - draw grid -
        for (int x = 0; x < gridRows; x++)
        {
            for (int y = 0; y < gridCols; y++)
            {
                int cellx = x * tileSize;
                int celly = y * tileSize;

                DrawRectangleLines(cellx, celly, tileSize, tileSize, BLUE);
            }
        }
#pragma endregion

#pragma region - grid highlight -

        Vector2 PositionOnGrid = GetMousePosition();

        int gridX = PositionOnGrid.x / tileSize;
        int gridY = PositionOnGrid.y / tileSize;

        int tileX = gridX * tileSize;
        int tileY = gridY * tileSize;

        DrawRectangleLines(tileX, tileY, tileSize, tileSize, GREEN);

#pragma endregion

        // set rail on grid cells

        EndDrawing();
    }

    CloseWindow();
    return 0;
}