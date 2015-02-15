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
Maze<msize, nsize> discoveredMaze;

Node cursor;
Node mark;
bool markSet = false;

NodeStack bfsPath;
bool showBfs = false;
bool runSim = false;
sf::Clock clk;

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
                ++cursor.i;
                cursor.i = cursor.i < 0 ? 0 : (cursor.i >= msize ? msize - 1: cursor.i);
                break;
            case sf::Keyboard::Key::Up:
                --cursor.i;
                cursor.i = cursor.i < 0 ? 0 : (cursor.i >= msize ? msize - 1 : cursor.i);
                break;
            case sf::Keyboard::Key::Right:
                ++cursor.j;
                cursor.j = cursor.j < 0 ? 0 : (cursor.j >= nsize ? nsize - 1 : cursor.j);
                break;
            case sf::Keyboard::Key::Left:
                --cursor.j;
                cursor.j = cursor.j < 0 ? 0 : (cursor.j >= nsize ? nsize - 1 : cursor.j);
                break;
            case sf::Keyboard::Key::S:
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[0] = !cw[0];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::D:
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[1] = !cw[1];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::W:
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[2] = !cw[2];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::A:
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[3] = !cw[3];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
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
                if (cursor == mark && markSet)
                {
                    markSet = false;
                }
                else
                {
                    mark = cursor;
                    markSet = true;
                }
                break;
            case sf::Keyboard::Key::B:
                showBfs = !showBfs;
                break;
            case sf::Keyboard::Key::X:
                runSim = !runSim;
                clk.restart();
                discoveredMaze.clear();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    if (showBfs && markSet && !runSim)
    {
        bfs(maze, cursor, mark, bfsPath);
    }

    if (runSim && markSet)
    {
        bfs(discoveredMaze, cursor, mark, bfsPath);
        
        auto cw = maze.getCellWalls(cursor.i, cursor.j);
        discoveredMaze.setCellWalls(cursor.i, cursor.j, cw);
        
        if (clk.getElapsedTime().asSeconds() >= 0.5f && bfsPath.size() > 1)
        {
            clk.restart();
            bfsPath.pop();
            auto n = bfsPath.pop();
            cursor.i = n.i;
            cursor.j = n.j;

            if (n == mark)
                markSet = false;
        }
    }
}


void draw()
{
    window.clear();
    if (runSim)
    {
        maze.draw(window, 16.f, 2.f, sf::Color(255, 255, 255, 127));
        discoveredMaze.draw(window);
    }
    else
    {
        maze.draw(window);
    }

    if (showBfs || runSim)
    {
        sf::VertexArray path(sf::LinesStrip, bfsPath.size());
        for (int i = 0; i < bfsPath.size(); ++i)
        {
            auto n = bfsPath[i];
            path[i].position = {n.j * 16.f + 8.f, n.i * 16.f + 8.f};
            path[i].color = sf::Color::Green;
        }
        window.draw(path);
    }

    if (markSet)
    {
        sf::CircleShape markshape(4.f);
        markshape.setPosition(mark.j * 16.f + 4.f, mark.i * 16.f + 4.f);
        markshape.setFillColor(sf::Color::Blue);
        window.draw(markshape);
    }

    sf::CircleShape cursorshape(4.f);
    cursorshape.setPosition(cursor.j * 16.f + 4.f, cursor.i * 16.f + 4.f);
    cursorshape.setFillColor(sf::Color::Red);
    window.draw(cursorshape);
    
    window.display();
}
