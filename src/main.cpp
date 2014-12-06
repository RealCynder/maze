#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdlib>


const int scale = 1;
const int viewHeight = 16 * 16;
const int viewWidth = 16 * 16;
sf::RenderWindow window(sf::VideoMode(viewWidth * scale, viewHeight * scale), "Maze");
sf::Clock clk;
bool keyPressed = false;


struct Maze
{
    static const int dim = 16; 
    int8_t hwalls[dim * (dim+1)];
    int8_t vwalls[dim * (dim+1)];

    void randomize()
    {
        for (int i = 0; i < dim * (dim+1); ++i)
        {
            hwalls[i] = std::rand() % 2;
            vwalls[i] = std::rand() % 2;
        }
    }
};

Maze maze;


void update();
void draw(const Maze&);
void processEvents();


int main()
{
    maze.randomize();

    while (window.isOpen())
    {
        processEvents();
        update();
        draw(maze);

        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}


void update()
{
    if (keyPressed)
        maze.randomize();
    
    keyPressed = false;
}


void draw(const Maze& maze)
{
    const float linel = 16.f;

    sf::RectangleShape line;
    line.setFillColor(sf::Color::White);
    
    window.clear();

    line.setSize(sf::Vector2f(2.f, linel));
    for (int i = 0; i < maze.dim + 1; ++i)
    {
        for (int j = 0; j < maze.dim; ++j)
        {
            if (maze.vwalls[i*maze.dim + j])
            {
                float x = i * linel;
                float y = j * linel;
                line.setPosition(sf::Vector2f(x - 1.f, y));
                window.draw(line);
            }
        }
    }

    line.setSize(sf::Vector2f(linel, 2.f));
    for (int i = 0; i < maze.dim; ++i)
    {
        for (int j = 0; j < maze.dim + 1; ++j)
        {
            if (maze.hwalls[i*maze.dim + j])
            {
                float x = i * linel;
                float y = j * linel;
                line.setPosition(sf::Vector2f(x, y - 1.f));
                window.draw(line);
            }
        }
    }
    
    window.display();
}


void processEvents()
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
            keyPressed = true;
            break;
        default:
            break;
        }
    }
}
