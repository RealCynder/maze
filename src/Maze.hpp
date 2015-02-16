#ifndef MAZE_HPP
#define MAZE_HPP

#include <array>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "BitArray2D.hpp"


/* The get/setCellsWalls functions take and return arrays of booleans
 * representing the state of each wall around a cell.
 *     cw[0] is the wall in the +i direction
 *     cw[1] is the wall in the +j direction
 *     cw[2] is the wall in the -i direction
 *     cw[3] is the wall in the -j direction
 * The borders of the maze are assumed to always have walls. Trying to set a
 * wall along the border to false will fail, but other walls set in the same
 * operation will succeed and the function will return false to signal the
 * invalid wall setting.
 *
 * The maze is drawn using matrix convention for indices and directions:
 * +-------+-------+---> j
 * | (0,0) | (0,1) |
 * +-------+-------+
 * | (1,0) | (1,1) |
 * +-------+-------+
 * |
 * v
 * i
 */


template<int m, int n>
class Maze
{
public:
    Maze();

    std::array<bool, 4> getCellWalls(int i, int j) const;
    bool setCellWalls(int i, int j, std::array<bool, 4> cw);
    
    void fill();
    void clear();
    void randomize();

    bool load(std::string);
    std::string save() const;
    
    void draw(sf::RenderTarget& target,
              float cellSize = 16.f,
              float lineThickness = 2.f,
              sf::Color lineColor = sf::Color::White) const;

private:
    BitArray2D<m - 1, n> mWalls;
    BitArray2D<m, n - 1> nWalls;
};


template<int m, int n>
Maze<m, n>::Maze()
{
    clear();
}


template<int m, int n>
std::array<bool, 4> Maze<m, n>::getCellWalls(int i, int j) const
{
    if (i < 0 || j < 0 || i >= m || j >= n)
        return {false, false, false, false};

    std::array<bool, 4> cw = {
        (m - 1 == i) || mWalls.get(i, j),
        (n - 1 == j) || nWalls.get(i, j),
        (0 == i) || mWalls.get(i - 1, j),
        (0 == j) || nWalls.get(i, j - 1) };

    return cw;
}


template<int m, int n>
bool Maze<m, n>::setCellWalls(int i, int j, std::array<bool, 4> cw)
{
    if (i < 0 || j < 0 || i >= m || j >= n)
        return false;

    bool error = ((m - 1 == i && !cw[0]) ||
                  (n - 1 == j && !cw[1]) ||
                  (0 == i && !cw[2]) ||
                  (0 == j && !cw[3]));

    if (i < m - 1)
        mWalls.set(i, j, cw[0]);

    if (j < n - 1)
        nWalls.set(i, j, cw[1]);

    if (i > 0)
        mWalls.set(i - 1, j, cw[2]);

    if (j > 0)
        nWalls.set(i, j - 1, cw[3]);

    // Returns false if the caller tried to set the boundary walls to false
    return !error;
}


template<int m, int n>
void Maze<m, n>::clear()
{
    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = 0;
    
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls[i] = 0;
}


template<int m, int n>
void Maze<m, n>::fill()
{
    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = 0xff;
    
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls[i] = 0xff;
}


template<int m, int n>
void Maze<m, n>::randomize()
{
    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = std::rand() % 256;
    
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls[i] = std::rand() % 256;
}


template<int m, int n>
bool Maze<m, n>::load(std::string mazestr)
{
    std::istringstream ss(mazestr);
    std::string tmp;

    BitArray2D<m - 1, n> mwTmp;
    BitArray2D<m, n - 1> nwTmp;
    
    try
    {
        if (!std::getline(ss, tmp, ':'))
            return false;
        int mm = std::stoi(tmp);
        if (mm != m)
            return false;
    
        if (!std::getline(ss, tmp, ':'))
            return false;
        int nn = std::stoi(tmp);
        if (nn != n)
            return false;

        if (!std::getline(ss, tmp, ':'))
            return false;
        for (int i = 0; i < mwTmp.size(); ++i)
            mwTmp[i] = std::stoi(tmp.substr(i*2, 2), nullptr, 16);

        if (!std::getline(ss, tmp, ':'))
            return false;
        for (int i = 0; i < nwTmp.size(); ++i)
            nwTmp[i] = std::stoi(tmp.substr(i*2, 2), nullptr, 16);
    }
    catch (...)
    {
        return false;
    }

    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = mwTmp[i];
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls[i] = nwTmp[i];

    return true;
}


template<int m, int n>
std::string Maze<m, n>::save() const
{
    std::ostringstream ss;
    ss << m << ":" << n << ":" << std::hex << std::setfill('0');
    for (int i = 0; i < mWalls.size(); ++i)
        ss << std::setw(2) << int(mWalls[i]);
    ss << ":";
    for (int i = 0; i < nWalls.size(); ++i)
        ss << std::setw(2) <<int(nWalls[i]);
    return ss.str();
}


template<int m, int n>
void Maze<m, n>::draw(sf::RenderTarget& target, float cellSize, float lineThickness, sf::Color lineColor) const
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
            if (mWalls.get(i, j))
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
            if (nWalls.get(i, j))
            {
                float x = (j + 1) * cellSize - lineThickness / 2.f;
                float y = i * cellSize;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }
}

#endif // MAZE_HPP
