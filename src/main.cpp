#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <queue>
#include "Maze.hpp"
#include "BitArray2D.hpp"
#include "BFS.hpp"


sf::RenderWindow window(sf::VideoMode(256, 256), "Maze");

const int msize = 16;
const int nsize = 16;
Maze<msize, nsize> maze;
Maze<msize, nsize> undoMaze;

int icursor = 0;
int jcursor = 0;
int imark = 0;
int jmark = 0;

NodeStack bfsPath;
bool showBfs = false;

void update();
void draw();
void bfs();


template<int m, int n>
bool loadMaze(Maze<n, m>& maze)
{
    std::cout << "Enter maze string:" << std::endl;
    std::string mazestr;
    std::getline(std::cin, mazestr);
    if (maze.load(mazestr))
    {
        std::cout << "Loaded maze" << std::endl;
        return true;
    }
    else
    {
        std::cout << "Failed to load maze" << std::endl;
        return false;
    }
}


template<int m, int n>
void saveMaze(Maze<n, m> maze)
{
    std::cout << "Maze saved:" << std::endl;
    std::cout << maze.save() << std::endl;
}


int main()
{
    maze.load("16:16:28802a48080a1a16645d54fd502a165999055c2e355b156fad1acd82a054:04ff96576e952e4bfc0ac88f804964aaac55848b4c06062a2a554cad4e9a");
    
    while (window.isOpen())
    {
        update();
        draw();

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}


void update()
{
    sf::Event event;

    // Handle window events
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
            case sf::Keyboard::Key::Down:
                ++icursor;
                icursor = icursor < 0 ? 0 : (icursor >= msize ? msize - 1: icursor);
                break;
            case sf::Keyboard::Key::Up:
                --icursor;
                icursor = icursor < 0 ? 0 : (icursor >= msize ? msize - 1 : icursor);
                break;
            case sf::Keyboard::Key::Right:
                ++jcursor;
                jcursor = jcursor < 0 ? 0 : (jcursor >= nsize ? nsize - 1 : jcursor);
                break;
            case sf::Keyboard::Key::Left:
                --jcursor;
                jcursor = jcursor < 0 ? 0 : (jcursor >= nsize ? nsize - 1 : jcursor);
                break;
            case sf::Keyboard::Key::S:
                {
                    auto cw = maze.getCellWalls(icursor, jcursor);
                    cw[0] = !cw[0];
                    maze.setCellWalls(icursor, jcursor, cw);
                }
                break;
            case sf::Keyboard::Key::D:
                {
                    auto cw = maze.getCellWalls(icursor, jcursor);
                    cw[1] = !cw[1];
                    maze.setCellWalls(icursor, jcursor, cw);
                }
                break;
            case sf::Keyboard::Key::W:
                {
                    auto cw = maze.getCellWalls(icursor, jcursor);
                    cw[2] = !cw[2];
                    maze.setCellWalls(icursor, jcursor, cw);
                }
                break;
            case sf::Keyboard::Key::A:
                {
                    auto cw = maze.getCellWalls(icursor, jcursor);
                    cw[3] = !cw[3];
                    maze.setCellWalls(icursor, jcursor, cw);
                }
                break;
            case sf::Keyboard::Key::C:
                undoMaze = maze;
                maze.clear();
                break;
            case sf::Keyboard::Key::F:
                undoMaze = maze;
                maze.fill();
                break;
            case sf::Keyboard::Key::R:
                undoMaze = maze;
                maze.randomize();
                break;
            case sf::Keyboard::Key::U:
                {
                    Maze<msize, nsize> tmp = maze;
                    maze = undoMaze;
                    undoMaze = tmp;
                }
                break;
            case sf::Keyboard::Key::L:
                {
                    Maze<msize, nsize> tmp;
                    if (loadMaze(tmp))
                    {
                        undoMaze = maze;
                        maze = tmp;
                    }
                }
                break;
            case sf::Keyboard::Key::V:
                saveMaze(maze);
                break;
            case sf::Keyboard::Key::Space:
                imark = icursor;
                jmark = jcursor;
                break;
            case sf::Keyboard::Key::B:
                showBfs = !showBfs;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}


void draw()
{
    window.clear();
    maze.draw(window);

    if (showBfs)
    {
        bfs(maze, {icursor, jcursor}, {imark, jmark}, bfsPath);
        
        sf::VertexArray path(sf::LinesStrip, bfsPath.size());
        for (int i = 0; i < bfsPath.size(); ++i)
        {
            auto n = bfsPath[i];
            path[i].position = {n.j * 16.f + 8.f, n.i * 16.f + 8.f};
            path[i].color = sf::Color::Green;
        }
        window.draw(path);
    }

    sf::CircleShape mark(4.f);
    mark.setPosition(jmark * 16.f + 4.f, imark * 16.f + 4.f);
    mark.setFillColor(sf::Color::Blue);
    window.draw(mark);

    sf::CircleShape cursor(4.f);
    cursor.setPosition(jcursor * 16.f + 4.f, icursor * 16.f + 4.f);
    cursor.setFillColor(sf::Color::Red);
    window.draw(cursor);
    
    window.display();
}
