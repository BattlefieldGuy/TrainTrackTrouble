/* command to run
g++ Main.cpp -o train.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/

#include "raylib.h"
#include <vector>

struct Train
{
        std::vector<Vector2> path;

        enum direction
        {
                up,
                down,
                left,
                right
        };

        float moveSpeed = 1;

        int trainX, trainY;

        int targetPathIndex;

        int stationTargetID;

        bool isMoving;
};

Train train;

int tileSize = 32;

int screenWidth = 800;
int screenHeight = 800;

int typeIndicatorSize = 32;
int typeIndicatorX = 16;
int typeIndicatorY = 16;

int nextStationId = 1;

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
                Obstacle,
                COUNT
                // room for more .....
        };

        const int maxTileTypes = static_cast<int>(TileType::COUNT);

        std::vector<std::vector<TileType>> grid;

        std::vector<std::vector<int>> stationsGrid;

        int gridRows = screenWidth / tileSize;
        int gridCols = screenHeight / tileSize;

#pragma endregion

#pragma region - grid initializer -
        // regular grid
        grid.resize(gridRows);

        for (int i = 0; i < gridRows; i++)
        {
                grid[i].resize(gridCols, TileType::Empty);
        }

        // grid to keep track of station numbers
        stationsGrid.resize(gridRows);

        for (int i = 0; i < gridRows; i++)
        {
                stationsGrid[i].resize(gridCols, -1);
        }
#pragma endregion

        // painter
        TileType activeTile = TileType::Track;

        train.trainX = 16;
        train.trainY = 16;

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
                                        if (stationsGrid[y][x] >= 0) // shows station number
                                                DrawText(TextFormat("%d", stationsGrid[y][x]), cellx, celly, 32, BLACK);
                                        break;
                                case TileType::Obstacle:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, DARKGRAY);
                                        break;

                                default:
                                        break;
                                }

                                // always draw outlines
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

                DrawRectangle(tileX, tileY, tileSize, tileSize, BLUE);

#pragma endregion

#pragma region - tile drawing -

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                        if (gridY >= 0 && gridY < grid.size() &&
                            gridX >= 0 && gridX < grid[0].size())

                                if (activeTile == TileType::Station && grid[gridY][gridX] != TileType::Station) // give station an ID
                                {
                                        stationsGrid[gridY][gridX] = nextStationId;
                                        nextStationId++;
                                }
                        grid[gridY][gridX] = activeTile;
                }

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                        if (gridY >= 0 && gridY < grid.size() &&
                            gridX >= 0 && gridX < grid[0].size())
                                grid[gridY][gridX] = TileType::Empty;
                }

                if (GetMouseWheelMove != 0) // reads mousewheel input to set the tile type to paint
                {
                        int typeIndex = static_cast<int>(activeTile);
                        typeIndex += GetMouseWheelMove();

                        if (typeIndex < 0)
                                typeIndex = maxTileTypes - 1;
                        if (typeIndex >= maxTileTypes)
                                typeIndex = 0;

                        activeTile = static_cast<TileType>(typeIndex);
                }

#pragma endRegion

#pragma region - train drawing -

                if (train.trainX >= 0 && train.trainX <= screenWidth / tileSize &&
                    train.trainY >= 0 && train.trainY <= screenHeight / tileSize)
                {
                        DrawRectangle(train.trainX * tileSize + 2, train.trainY * tileSize + 2, tileSize - 4, tileSize - 4, DARKGREEN);
                }

#pragma endregion

#pragma region - type indicator -

                switch (activeTile) // type indicator
                {
                case TileType::Empty:
                        DrawRectangle(typeIndicatorX, typeIndicatorY, typeIndicatorSize, typeIndicatorSize, WHITE);
                        DrawRectangleLines(typeIndicatorX, typeIndicatorY, typeIndicatorSize, typeIndicatorSize, RED);
                        break;
                case TileType::Track:
                        DrawRectangle(typeIndicatorX, typeIndicatorY, typeIndicatorSize, typeIndicatorSize, GRAY);
                        break;
                case TileType::Station:
                        DrawRectangle(typeIndicatorX, typeIndicatorY, typeIndicatorSize, typeIndicatorSize, GREEN);
                        break;
                case TileType::Obstacle:
                        DrawRectangle(typeIndicatorX, typeIndicatorY, typeIndicatorSize, typeIndicatorSize, DARKGRAY);
                        break;

                default:
                        break;
                }

#pragma endregion

                EndDrawing();
        }

        CloseWindow();
        return 0;
}