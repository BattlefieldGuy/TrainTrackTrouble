/* command to run
g++ Main.cpp -o train.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/

#include "raylib.h"
#include <vector>

struct Train // train
{
        std::vector<Vector2> path;

        enum direction
        {
                up,
                down,
                left,
                right
        };

        float moveSpeed = 0.8;

        float move;

        int trainX, trainY;

        int targetPathIndex;

        int stationTargetID;

        bool isMoving = false;

        // the stats
};

struct TrackNode
{
        int x, y;

        std::vector<Vector2> adjacents;
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

        std::vector<TrackNode> trackNodes;

        int gridRows = screenWidth / tileSize;
        int gridCols = screenHeight / tileSize;

        // painter
        TileType activeTile = TileType::Track;

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

        // temps
        train.trainX = 16;
        train.trainY = 16;

        train.path.push_back(Vector2{15, 16});
        train.path.push_back(Vector2{15, 15});
        train.path.push_back(Vector2{14, 15});

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

#pragma endregion

#pragma region - train -

                if (train.trainX >= 0 && train.trainX <= screenWidth / tileSize &&
                    train.trainY >= 0 && train.trainY <= screenHeight / tileSize)
                {
                        DrawRectangle(train.trainX * tileSize + 2, train.trainY * tileSize + 2, tileSize - 4, tileSize - 4, DARKGREEN);
                }

#pragma region - train movement -

                train.move -= GetFrameTime();

                if (train.move <= 0 && train.isMoving)
                {
                        Vector2 _nextMove = train.path[train.targetPathIndex];

                        train.trainX = _nextMove.x;
                        train.trainY = _nextMove.y;

                        train.move = train.moveSpeed;

                        if (train.targetPathIndex < train.path.size() - 1)
                                train.targetPathIndex++;
                }

#pragma endregion // sub

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

#pragma region - start grid scan -

                // if (IsKeyPressed(KEY_SPACE))
                //         train.isMoving = true;

                if (IsKeyPressed(KEY_B))
                {

                        for (float x = 0; x < gridRows; x++)
                        {
                                for (float y = 0; y < gridCols; y++)
                                {
                                        if (grid[x][y] == TileType::Track)
                                        {
                                                TrackNode _trackNode;

                                                _trackNode.x = x;
                                                _trackNode.y = y;

                                                //adjacents
                                                if (y > 0 && grid[x][y - 1] == TileType::Track)// above
                                                        _trackNode.adjacents.push_back(Vector2{x, y - 1});
                                                if (y < gridCols - 1 && grid[x][y + 1] == TileType::Track)// below
                                                        _trackNode.adjacents.push_back(Vector2{x, y + 1});
                                                if (x > 0 && grid[x - 1][y] == TileType::Track)// left
                                                        _trackNode.adjacents.push_back(Vector2{x - 1, y});
                                                if (x < gridRows - 1 && grid[x + 1][y] == TileType::Track)// right
                                                        _trackNode.adjacents.push_back(Vector2{x + 1, y});

                                                trackNodes.push_back(_trackNode);
                                        }
                                }
                        }
                }

#pragma endregion

                EndDrawing();
        }

        CloseWindow();
        return 0;
}