#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
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
bool loadMaze(Maze<16, 16>& maze);
void saveMaze(Maze<16, 16> maze);

int coerce(int a, int l, int u)
{
    return a < l ? l : (a > u ? u: a);
}


int main()
{
    std::srand(std::time(0));
    
    // Load default maze
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
            // Needed for close window button to work
            window.close();
            break;
            
        case sf::Event::KeyPressed:
            // Handle controls
            switch (event.key.code)
            {
                
            // Cursor movement
            case sf::Keyboard::Key::Down:
                ++cursor.i;
                cursor.i = coerce(cursor.i, 0, msize - 1);
                break;
            case sf::Keyboard::Key::Up:
                --cursor.i;
                cursor.i = coerce(cursor.i, 0, msize - 1);
                break;
            case sf::Keyboard::Key::Right:
                ++cursor.j;
                cursor.j= coerce(cursor.j, 0, nsize - 1);
                break;
            case sf::Keyboard::Key::Left:
                --cursor.j;
                cursor.j= coerce(cursor.j, 0, nsize - 1);
                break;

            // Set/unset walls around cursor
            case sf::Keyboard::Key::S: // Bottom
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[0] = !cw[0];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::D: // Right
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[1] = !cw[1];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::W: // Top
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[2] = !cw[2];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;
            case sf::Keyboard::Key::A: // Left
                {
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    cw[3] = !cw[3];
                    maze.setCellWalls(cursor.i, cursor.j, cw);
                }
                break;

            // Modify maze globally
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

            // Undo
            case sf::Keyboard::Key::U:
                {
                    Maze<msize, nsize> tmp = maze;
                    maze = undoMaze;
                    undoMaze = tmp;
                }
                break;

            // Load/save maze
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

            // Place mark
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

            // Show BFS path
            case sf::Keyboard::Key::B:
                showBfs = !showBfs;
                break;

            // Run simulation
            case sf::Keyboard::Key::X:
                {
                    runSim = !runSim;
                    clk.restart();
                    discoveredMaze.clear();
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    discoveredMaze.setCellWalls(cursor.i, cursor.j, cw);
                    bfs(discoveredMaze, cursor, mark, bfsPath);
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

    if (showBfs && markSet && !runSim)
    {
        // Run BFS for display using the entire maze
        bfs(maze, cursor, mark, bfsPath);
    }

    if (runSim && markSet)
    {
        // Cursor steps along the BFS path every so often
        if (clk.getElapsedTime().asSeconds() >= 0.5f)
        {
            clk.restart();

            if (bfsPath.size() > 0)
            {
                bfsPath.pop(); // First node is the current node; remove it
                cursor = bfsPath.pop(); // Set cursor position to next node
            }
                
            // Sense walls in current cell
            auto cw = maze.getCellWalls(cursor.i, cursor.j);
            discoveredMaze.setCellWalls(cursor.i, cursor.j, cw);

            // Run BFS using only the discovered parts of the maze
            bfs(discoveredMaze, cursor, mark, bfsPath);

            // Stop when the mark is reached
            if (cursor == mark)
                runSim = false;
        }
    }
}


void draw()
{
    window.clear();
    
    if (runSim)
    {
        // Undiscovered parts of the maze are show in gray
        maze.draw(window, 16.f, 2.f, sf::Color(255, 255, 255, 127));
        discoveredMaze.draw(window);
    }
    else
    {
        // Draw maze normally
        maze.draw(window);
    }
    
    if (showBfs || runSim)
    {
        // Draw BFS path
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
        // Draw mark
        sf::CircleShape markshape(4.f);
        markshape.setPosition(mark.j * 16.f + 4.f, mark.i * 16.f + 4.f);
        markshape.setFillColor(sf::Color::Blue);
        window.draw(markshape);
    }

    // Draw cursor
    sf::CircleShape cursorshape(4.f);
    cursorshape.setPosition(cursor.j * 16.f + 4.f, cursor.i * 16.f + 4.f);
    cursorshape.setFillColor(sf::Color::Red);
    window.draw(cursorshape);
    
    window.display();
}


bool loadMaze(Maze<16, 16>& maze)
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


void saveMaze(Maze<16, 16> maze)
{
    std::cout << "Maze saved:" << std::endl;
    std::cout << maze.save() << std::endl;
}
