#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Maze.hpp"
#include <cstdlib>


sf::RenderWindow window(sf::VideoMode(256, 256), "Maze");
Maze maze(16, 16);
Maze undoMaze  ;


void update();
void draw();

int icursor = 0;
int jcursor = 0;


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
                icursor = icursor < 0 ? 0 : (icursor >= maze.m() ? maze.m() - 1: icursor);
                break;
            case sf::Keyboard::Key::Up:
                --icursor;
                icursor = icursor < 0 ? 0 : (icursor >= maze.m() ? maze.m() - 1 : icursor);
                break;
            case sf::Keyboard::Key::Right:
                ++jcursor;
                jcursor = jcursor < 0 ? 0 : (jcursor >= maze.n() ? maze.n() - 1 : jcursor);
                break;
            case sf::Keyboard::Key::Left:
                --jcursor;
                jcursor = jcursor < 0 ? 0 : (jcursor >= maze.n() ? maze.n() - 1 : jcursor);
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
                    Maze tmp = maze;
                    maze = undoMaze;
                    undoMaze = tmp;
                }
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
    cursor.setPosition(jcursor*16.f + 4.f, icursor*16.f + 4.f);
    cursor.setFillColor(sf::Color::Red);
    window.draw(cursor);
    
    window.display();
}
