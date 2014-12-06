#ifndef MAZE_HPP
#define MAZE_HPP

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>


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


class Maze
{
public:
    Maze() : Maze(16) {}
    Maze(int size) : Maze(size, size) {}
    Maze(int m, int n);

    std::array<bool, 4> getCellWalls(int i, int j);
    bool setCellWalls(int i, int j, std::array<bool, 4> cw);
    void randomize();
    void draw(sf::RenderTarget& target,
              float cellSize = 16.f,
              float lineThickness = 2.f,
              sf::Color lineColor = sf::Color::White);
    
    const int m;
    const int n;

private:
    std::vector<std::vector<bool>> mWalls;
    std::vector<std::vector<bool>> nWalls;
    
};


#endif // MAZE_HPP
