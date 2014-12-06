#include "Maze.hpp"
#include <exception>
#include <cstdlib>


Maze::Maze(int height, int width) :
    height(height),
    width(width),
    horizontalWalls(height - 1, std::vector<bool>(width)),
    verticalWalls(height, std::vector<bool>(width - 1))
{

}


WallStates Maze::getCellWalls(int i, int j)
{
    if (i < 0 || j < 0 || i >= height || j >= width)
        throw std::out_of_range("__func__");

    WallStates ws;

    ws.north = (0 == i) || horizontalWalls[i - 1][j];
    ws.west = (0 == j) || verticalWalls[i][j - 1];
    ws.south = (height - 1 == i) || horizontalWalls[i][j];
    ws.east = (width - 1 == j) || verticalWalls[i][j];

    return ws;
}


bool Maze::setCellWalls(int i, int j, WallStates ws)
{
    if (i < 0 || j < 0 || i >= height || j >= width)
        throw std::out_of_range("__func__");

    bool error = ((0 == i && !ws.north) ||
                  (0 == j && !ws.west) ||
                  (height - 1 == i && !ws.south) ||
                  (width - 1 == j && !ws.east));

    if (i > 0)
        horizontalWalls[i - 1][j] = ws.north;

    if (j > 0)
        verticalWalls[i][j - 1] = ws.west;

    if (i < height - 1)
        horizontalWalls[i][j] = ws.south;

    if (j < width - 1)
        verticalWalls[i][j] = ws.east;

    // Returns false if the caller tried to set the boundary walls to false
    return !error;
}


void Maze::randomize()
{
    for (int i = 0; i < height - 1; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            horizontalWalls[i][j] = std::rand() % 2;
        }
    }
    
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width - 1; ++j)
        {
            verticalWalls[i][j] = std::rand() % 2;
        }
    }
}


void Maze::draw(sf::RenderTarget& target, float cellSize, float lineThickness, sf::Color lineColor)
{
    sf::RectangleShape line;
    line.setFillColor(lineColor);

    // Draw borders
    line.setSize(sf::Vector2f(lineThickness / 2.f, cellSize * height));
    line.setPosition(sf::Vector2f(0.f, 0.f));
    target.draw(line);

    line.setPosition(sf::Vector2f(cellSize * width - lineThickness / 2.f, 0.f));
    target.draw(line);

    line.setSize(sf::Vector2f(cellSize * width, lineThickness / 2.f));
    line.setPosition(sf::Vector2f(0.f, 0.f));
    target.draw(line);

    line.setPosition(sf::Vector2f(0.f, cellSize * height - lineThickness / 2.f));
    target.draw(line);

    // Draw horizontal walls
    line.setSize(sf::Vector2f(cellSize, lineThickness));
    
    for (int i = 0; i < height - 1; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (horizontalWalls[i][j])
            {
                float x = j * cellSize;
                float y = (i + 1) * cellSize - lineThickness / 2.f;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }

    

    // Draw vertical walls
    line.setSize(sf::Vector2f(lineThickness, cellSize));
    
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width - 1; ++j)
        {
            if (verticalWalls[i][j])
            {
                float x = (j + 1) * cellSize - lineThickness / 2.f;
                float y = i * cellSize;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }
}
