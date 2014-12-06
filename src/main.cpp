#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Maze.hpp"


sf::RenderWindow window(sf::VideoMode(256, 256), "Maze");
Maze maze(16, 16);


void update();
void draw();


int main()
{
    maze.randomize();

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
        case sf::Event::TextEntered:
            maze.randomize();
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
    window.display();
}
