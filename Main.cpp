/* command to run
g++ Main.cpp -o train.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/

#include "raylib.h"
#include <vector>

int tileSize = 32;

int screenWidth = 1800;
int screenHeight = 800;

int main()
{
        InitWindow(screenWidth, screenHeight, "Train Track Trouble");
        SetTargetFPS(60);

#pragma region - grid vriables -

        enum class TileType
        {
                Empty,
                Track,
                Station,
                Obstacle
                // room for more .....
        };

        std::vector<std::vector<TileType>> grid;

        int gridRows = screenWidth / tileSize;
        int gridCols = screenHeight / tileSize;

#pragma endregion

#pragma region - grid initializer -
        grid.resize(gridRows);

        for (int i = 0; i < gridRows; i++)
        {
                grid[i].resize(gridCols, TileType::Empty);
        }
#pragma endregion

        // painter
        TileType activeTile = TileType::Track;

        while (!WindowShouldClose())
        {

                BeginDrawing();
                ClearBackground(RAYWHITE);

#pragma region - draw grid -
                for (int x = 0; x < gridRows; x++)
                {
                        for (int y = 0; y < gridCols; y++)
                        {
                                int cellx = x * tileSize;
                                int celly = y * tileSize;

                                switch (grid[y][x])
                                {
                                case TileType::Track:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, GRAY);
                                        break;
                                case TileType::Station:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, GREEN);
                                        break;
                                case TileType::Obstacle:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, DARKGRAY);
                                        break;

                                default:
                                        break;
                                }

                                //always draw outlines
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

                DrawRectangle(tileX, tileY, tileSize, tileSize, GREEN);

#pragma endregion

#pragma region - tile drawing -

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                        if (gridY >= 0 && gridY < grid.size() &&
                            gridX >= 0 && gridX < grid[0].size())
                                grid[gridY][gridX] = activeTile;
                }

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                        grid[gridY][gridX] = TileType::Empty;
                }

#pragma endregion

                EndDrawing();
        }

        CloseWindow();
        return 0;
}