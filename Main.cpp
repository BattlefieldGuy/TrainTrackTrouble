/* command to run
g++ Main.cpp -o train.exe -IC:/raylib/include -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/

#include "raylib.h"
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

struct IntVec2
{
        int x, y;

        bool operator<(const IntVec2 &other) const
        {
                return (y < other.y) || (y == other.y && x < other.x);
        }

        bool operator==(const IntVec2 &other) const
        {
                return x == other.x && y == other.y;
        }
};

struct Train // train
{
        std::vector<IntVec2> path;

        enum direction
        {
                up,
                down,
                left,
                right
        };

        float moveSpeed = 0.8;

        float move;

        float stationWaitTime = 0.0f;

        int trainX, trainY;

        int targetPathIndex;

        int stationTargetID;

        bool isMoving = false;

        // the stats
};

struct TrackNode
{
        int x, y;

        std::vector<IntVec2> adjacents;
};

Train train;

int tileSize = 32;

int screenWidth = 800;
int screenHeight = 800;

int typeIndicatorSize = 32;
int typeIndicatorX = 16;
int typeIndicatorY = 16;

int nextStationId = 1;

bool isTrainPlaced = false;

std::vector<IntVec2> findPath(IntVec2 _start, IntVec2 _end, const std::vector<TrackNode> &_trackNodes);

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

        std::vector<std::vector<TileType>> tileGrid;

        std::vector<std::vector<int>> stationsGrid;

        std::vector<TrackNode> trackNodes;

        int gridCols = screenWidth / tileSize;
        int gridRows = screenHeight / tileSize;

        // painter
        TileType activeTile = TileType::Track;

#pragma endregion

#pragma region - grid initializer -
        // regular grid
        tileGrid.resize(gridRows);

        for (int i = 0; i < gridRows; i++)
        {
                tileGrid[i].resize(gridCols, TileType::Empty);
        }

        // grid to keep track of station numbers
        stationsGrid.resize(gridRows);

        for (int i = 0; i < gridRows; i++)
        {
                stationsGrid[i].resize(gridCols, -1);
        }
#pragma endregion

        // start
        // train.trainX = 16;
        // train.trainY = 16;

        // tileGrid[train.trainY][train.trainX] = TileType::Track;

        while (!WindowShouldClose())
        {

                BeginDrawing();
                ClearBackground(RAYWHITE);

#pragma region - draw grid -
                for (int row = 0; row < gridRows; row++)
                {
                        for (int col = 0; col < gridCols; col++)
                        {
                                int cellx = col * tileSize;
                                int celly = row * tileSize;

                                switch (tileGrid[row][col])
                                {
                                case TileType::Track:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, GRAY);
                                        break;
                                case TileType::Station:
                                        DrawRectangle(cellx, celly, tileSize, tileSize, GREEN);
                                        if (stationsGrid[row][col] >= 0) // shows station number
                                                DrawText(TextFormat("%d", stationsGrid[row][col]), cellx, celly, 32, BLACK);
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
                        if (gridY >= 0 && gridY < tileGrid.size() &&
                            gridX >= 0 && gridX < tileGrid[0].size())

                                if (activeTile == TileType::Station && tileGrid[gridY][gridX] != TileType::Station) // give station an ID
                                {
                                        stationsGrid[gridY][gridX] = nextStationId;
                                        nextStationId++;
                                }

                        tileGrid[gridY][gridX] = activeTile;

                        if (!isTrainPlaced && activeTile == TileType::Track)
                        {
                                train.trainX = gridX;
                                train.trainY = gridY;
                                isTrainPlaced = true;
                        }
                }

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                        if (gridY >= 0 && gridY < tileGrid.size() &&
                            gridX >= 0 && gridX < tileGrid[0].size())
                                tileGrid[gridY][gridX] = TileType::Empty;
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

                if (train.stationWaitTime > 0.0f)
                {
                        train.stationWaitTime -= GetFrameTime();
                }
                else if (train.move <= 0)
                {
                        if (train.isMoving && !train.path.empty())
                        {

                                if (train.targetPathIndex >= train.path.size())
                                {
                                        train.isMoving = false;
                                }
                                else
                                {
                                        IntVec2 _nextMove = train.path[train.targetPathIndex];
                                        train.trainX = _nextMove.x;
                                        train.trainY = _nextMove.y;

                                        if (tileGrid[train.trainY][train.trainX] == TileType::Station)
                                        {
                                                train.stationWaitTime = 3.0f; // pause at station
                                        }

                                        train.move = train.moveSpeed;

                                        if (train.targetPathIndex < train.path.size() - 1)
                                                train.targetPathIndex++;
                                }
                        }
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

#pragma region - start scanning and planning -

                if (IsKeyPressed(KEY_P))
                {
                        // grid scan

                        trackNodes.clear();
                        train.path.clear();

                        DrawRectangle(50, 50, 50, 50, RED);

                        for (int row = 0; row < gridRows; row++)
                        {
                                for (int col = 0; col < gridCols; col++)
                                {
                                        if (tileGrid[row][col] == TileType::Track || tileGrid[row][col] == TileType::Station)
                                        {
                                                TrackNode _trackNode;

                                                _trackNode.x = col;
                                                _trackNode.y = row;

                                                // adjacents
                                                if (row > 0 && (tileGrid[row - 1][col] == TileType::Track || tileGrid[row - 1][col] == TileType::Station))  // up
    _trackNode.adjacents.push_back(IntVec2{col, row - 1});

if (row < gridRows - 1 && (tileGrid[row + 1][col] == TileType::Track || tileGrid[row + 1][col] == TileType::Station))  // down
    _trackNode.adjacents.push_back(IntVec2{col, row + 1});

if (col > 0 && (tileGrid[row][col - 1] == TileType::Track || tileGrid[row][col - 1] == TileType::Station))  // left
    _trackNode.adjacents.push_back(IntVec2{col - 1, row});

if (col < gridCols - 1 && (tileGrid[row][col + 1] == TileType::Track || tileGrid[row][col + 1] == TileType::Station))  // right
    _trackNode.adjacents.push_back(IntVec2{col + 1, row});


                                                trackNodes.push_back(_trackNode);

                                                DrawRectangle(110, 50, 50, 50, RED);
                                        }
                                }
                        }

                        // make path
                        IntVec2 _currentPos = {train.trainX, train.trainY};

                        bool isValidStart = std::any_of(trackNodes.begin(), trackNodes.end(), [&_currentPos](const TrackNode &_n)
                                                        { return _n.x == _currentPos.x && _n.y == _currentPos.y; });

                        if (!isValidStart)
                        {
                                TraceLog(LOG_WARNING, "Train is not on a walid node !!!!");
                                continue;
                        }

                        // Find a station tile
                        IntVec2 _target = {-1, -1};
                        for (int _row = 0; _row < gridRows; _row++)
                        {
                                for (int _col = 0; _col < gridCols; _col++)
                                {
                                        if (stationsGrid[_row][_col] >= 1)
                                        {
                                                _target = IntVec2{_col, _row};
                                                break;
                                        }
                                }
                                if (_target.x >= 0)
                                        break;
                        }

                        // Only if station found
                        if (_target.x >= 0)
                        {
                                TraceLog(LOG_INFO, "Calling findPath from (%d, %d) to (%d, %d)", train.trainX, train.trainY, (int)_target.x, (int)_target.y);

                                std::vector<IntVec2> _path = findPath(_currentPos, _target, trackNodes);
                                TraceLog(LOG_INFO, "Train path size: %d", train.path.size());
                                TraceLog(LOG_INFO, "Initial targetPathIndex: %d", train.targetPathIndex);

                                if (!_path.empty())
                                {
                                        for (auto step : _path)
                                        {
                                                TraceLog(LOG_INFO, "Path step: (%d, %d)", step.x, step.y);
                                                TraceLog(LOG_INFO, "Start: (%d, %d), End: (%d, %d)", _currentPos.x, _currentPos.y, _target.x, _target.y);
                                        }
                                        DrawRectangle(170, 50, 50, 50, BLUE);
                                        train.path = _path;
                                        train.targetPathIndex = (_path.size() > 1 ? 1 : 0);
                                        train.isMoving = true;
                                        train.move = train.moveSpeed;
                                }
                        }
                }

#pragma endregion
                EndDrawing();
        }

        CloseWindow();
        return 0;
}

#pragma region - pathfinding -

std::vector<IntVec2> findPath(IntVec2 _start, IntVec2 _end, const std::vector<TrackNode> &_trackNodes)
{
        TraceLog(LOG_INFO, "findPath(): _trackNodes.size() = %d", _trackNodes.size());

        std::queue<IntVec2> _toVisit;
        std::map<IntVec2, IntVec2> _predecessors;
        std::set<IntVec2> _visited;

        _toVisit.push(_start);
        _visited.insert(_start);

        while (!_toVisit.empty())
        {
                IntVec2 _current = _toVisit.front();
                _toVisit.pop();

                if (_current.x == _end.x && _current.y == _end.y)
                {
                        // Reconstruct path
                        std::vector<IntVec2> _path;
                        for (IntVec2 _at = _end; !(_at.x == _start.x && _at.y == _start.y); _at = _predecessors[_at])
                        {
                                _path.push_back(_at);
                        }
                        _path.push_back(_start);
                        std::reverse(_path.begin(), _path.end());
                        return _path;
                }

                // Find the tracknode corresponding to _current
                auto _it = std::find_if(_trackNodes.begin(), _trackNodes.end(), [&_current](const TrackNode &_node)
                                        { return _node.x == _current.x && _node.y == _current.y; });

                if (_it != _trackNodes.end())
                {
                        for (const IntVec2 &_neighbor : _it->adjacents)
                        {
                                if (_visited.find(_neighbor) == _visited.end())
                                {
                                        _toVisit.push(_neighbor);
                                        _visited.insert(_neighbor);
                                        _predecessors[_neighbor] = _current;
                                }
                        }
                }
        }

        return {}; // No path found
}

#pragma endregion