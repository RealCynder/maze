#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include "Maze.hpp"


sf::RenderWindow window(sf::VideoMode(256, 256), "Maze");

const int msize = 16;
const int nsize = 16;
Maze<msize, nsize> maze;
Maze<msize, nsize> undoMaze;

int icursor = 0;
int jcursor = 0;

void update();
void draw();


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

    sf::CircleShape cursor(4.f);
    cursor.setPosition(jcursor*nsize + 4.f, icursor*msize + 4.f);
    cursor.setFillColor(sf::Color::Red);
    window.draw(cursor);
    
    window.display();
}


