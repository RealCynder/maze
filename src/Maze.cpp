#include "Maze.hpp"
#include <exception>
#include <cstdlib>


Maze::Maze(int m, int n) :
    m(m),
    n(n),
    mWalls(m - 1, std::vector<bool>(n)),
    nWalls(m, std::vector<bool>(n - 1))
{
}


std::array<bool, 4> Maze::getCellWalls(int i, int j)
{
    if (i < 0 || j < 0 || i >= m || j >= n)
        throw std::out_of_range(__func__);

    std::array<bool, 4> cw = {
        (m - 1 == i) || mWalls[i][j],
        (n - 1 == j) || nWalls[i][j],
        (0 == i) || mWalls[i - 1][j],
        (0 == j) || nWalls[i][j - 1] };

    return cw;
}


bool Maze::setCellWalls(int i, int j, WallStates ws)
{
    if (i < 0 || j < 0 || i >= m || j >= n)
        throw std::out_of_range(__func__);

    bool error = ((m - 1 == i && !cw[0]) ||
                  (n - 1 == j && !cw[1]) ||
                  (0 == i && !cw[2]) ||
                  (0 == j && !cw[3]));

    if (i < m - 1)
        mWalls[i][j] = cw[0];

    if (j < n - 1)
        nWalls[i][j] = cw[1];

    if (i > 0)
        mWalls[i - 1][j] = cw[2];

    if (j > 0)
        nWalls[i][j - 1] = cw[3];

    // Returns false if the caller tried to set the boundary walls to false
    return !error;
}


void Maze::randomize()
{
    for (int i = 0; i < m - 1; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            mWalls[i][j] = std::rand() % 2;
        }
    }
    
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n - 1; ++j)
        {
            nWalls[i][j] = std::rand() % 2;
        }
    }
}


void Maze::draw(sf::RenderTarget& target, float cellSize, float lineThickness, sf::Color lineColor)
{
    sf::RectangleShape line;
    line.setFillColor(lineColor);

    // Draw borders
    line.setSize(sf::Vector2f(lineThickness / 2.f, cellSize * m));
    line.setPosition(sf::Vector2f(0.f, 0.f));
    target.draw(line);

    line.setPosition(sf::Vector2f(cellSize * n - lineThickness / 2.f, 0.f));
    target.draw(line);

    line.setSize(sf::Vector2f(cellSize * n, lineThickness / 2.f));
    line.setPosition(sf::Vector2f(0.f, 0.f));
    target.draw(line);

    line.setPosition(sf::Vector2f(0.f, cellSize * m - lineThickness / 2.f));
    target.draw(line);

    // Draw m walls
    line.setSize(sf::Vector2f(cellSize, lineThickness));
    
    for (int i = 0; i < m - 1; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (mWalls[i][j])
            {
                float x = j * cellSize;
                float y = (i + 1) * cellSize - lineThickness / 2.f;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }

    // Draw n walls
    line.setSize(sf::Vector2f(lineThickness, cellSize));
    
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n - 1; ++j)
        {
            if (nWalls[i][j])
            {
                float x = (j + 1) * cellSize - lineThickness / 2.f;
                float y = i * cellSize;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }
}
