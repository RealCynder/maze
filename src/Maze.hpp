#ifndef MAZE_HPP
#define MAZE_HPP

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <utility>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>


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
    std::array<unsigned char, ((m - 1) * n + 7) / 8> mWalls;
    std::array<unsigned char, (m * (n - 1) + 7) / 8> nWalls;

    bool getMWall(int i, int j) const;
    bool getNWall(int i, int j) const;
    void setMWall(int i, int j, bool v);
    void setNWall(int i, int j, bool v);
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
        (m - 1 == i) || getMWall(i, j),
        (n - 1 == j) || getNWall(i, j),
        (0 == i) || getMWall(i - 1, j),
        (0 == j) || getNWall(i, j - 1) };

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
        setMWall(i, j, cw[0]);

    if (j < n - 1)
        setNWall(i, j, cw[1]);

    if (i > 0)
        setMWall(i - 1, j, cw[2]);

    if (j > 0)
        setNWall(i, j - 1, cw[3]);

    // Returns false if the caller tried to set the boundary walls to false
    return !error;
}


template<int m, int n>
void Maze<m, n>::clear()
{
    mWalls.fill(0);
    nWalls.fill(0);
}


template<int m, int n>
void Maze<m, n>::fill()
{
    mWalls.fill(0xff);
    nWalls.fill(0xff);
}


template<int m, int n>
void Maze<m, n>::randomize()
{
    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = rand() % 256;
    
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls[i] = rand() % 256;
}


template<int m, int n>
bool Maze<m, n>::load(std::string mazestr)
{
    std::istringstream ss(mazestr);
    std::string tmp;

    std::array<unsigned char, ((m - 1) * n + 7) / 8> mwTmp;
    std::array<unsigned char, (m * (n - 1) + 7) / 8> nwTmp;
    
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
            mwTmp.at(i) = std::stoi(tmp.substr(i*2, 2), nullptr, 16);

        if (!std::getline(ss, tmp, ':'))
            return false;
        for (int i = 0; i < nwTmp.size(); ++i)
            nwTmp.at(i) = std::stoi(tmp.substr(i*2, 2), nullptr, 16);
    }
    catch (...)
    {
        return false;
    }

    for (int i = 0; i < mWalls.size(); ++i)
        mWalls[i] = mwTmp[i];
    for (int i = 0; i < nWalls.size(); ++i)
        nWalls = nwTmp;

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
            if (getMWall(i, j))
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
            if (getNWall(i, j))
            {
                float x = (j + 1) * cellSize - lineThickness / 2.f;
                float y = i * cellSize;
                line.setPosition(sf::Vector2f(x, y));
                target.draw(line);
            }
        }
    }
}


template<int m, int n>
bool Maze<m, n>::getMWall(int i, int j) const
{
    return (mWalls[(i + j*(m-1))/8] >> ((i + j*(m-1)) % 8)) & 0x1;
}


template<int m, int n>
bool Maze<m, n>::getNWall(int i, int j) const
{
    return (nWalls[(i + j*m)/8] >> ((i + j*m) % 8)) & 0x1;
}


template<int m, int n>
void Maze<m, n>::setMWall(int i, int j, bool b)
{
    mWalls[(i + j*(m-1))/8] &= ~(1 << (i + j*(m-1)) % 8);
    mWalls[(i + j*(m-1))/8] |= b << (i + j*(m-1)) % 8;
}


template<int m, int n>
void Maze<m, n>::setNWall(int i, int j, bool b)
{
    nWalls[(i + j*m)/8] &= ~(1 << (i + j*m) % 8);
    nWalls[(i + j*m)/8] |= b << (i + j*m) % 8;
}


#endif // MAZE_HPP
