#ifndef MAZE_HPP
#define MAZE_HPP

#include <vector>
#include <SFML/Graphics.hpp>

/* Uses matrix convention for indexing:
 * +-------+-------+
 * | (0,0) | (0,1) |
 * +-------+-------+
 * | (1,0) | (1,1) |
 * +-------+-------+
 */


struct WallStates
{
    bool north, west, south, east;
};


class Maze
{
public:
    Maze() : Maze(16) {}
    Maze(int size) : Maze(size, size) {}
    Maze(int height, int width);

    WallStates getCellWalls(int i, int j);
    bool setCellWalls(int i, int j, WallStates ws);
    void randomize();
    void draw(sf::RenderTarget& target,
              float cellSize = 16.f,
              float lineThickness = 2.f,
              sf::Color lineColor = sf::Color::White);
    
    const int height;
    const int width;

private:
    std::vector<std::vector<bool>> horizontalWalls;
    std::vector<std::vector<bool>> verticalWalls;
    
};


#endif // MAZE_HPP
