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
BitArray2D<msize, nsize> unvisitedNodes;
BitArray2D<msize, nsize> inferredNodes;
BitArray2D<msize, nsize> OptimumNodes;

Node cursor;
Node mark;
Node Start;
Node CurrentIdeal;
bool markSet = false;

NodeStack bfsPath;
NodeStack bfsFinal;
bool showBfs = false;
bool runSim = false;
bool mapping = false;
bool optimize = false;
sf::Clock clk;

void update();
void draw();
void bfs();
bool loadMaze(Maze<16, 16>& maze);
void saveMaze(Maze<16, 16> maze);
float ScorePath(NodeStack Path);

int coerce(int a, int l, int u)
{
    return a < l ? l : (a > u ? u: a);
}


int main()
{
    std::srand(std::time(0));
    
    // Load default maze
    maze.load("16:16:28802a48080a1a16645d54fd502a165999055c2e355b156fad1acd82a054:04ff96576e952e4bfc0ac88f804964aaac55848b4c06062a2a554cad4e9a");

    cursor.i = msize - 1;
    
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
                    mapping = false;
                    clk.restart();
                    discoveredMaze.clear();
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    discoveredMaze.setCellWalls(cursor.i, cursor.j, cw);
                    bfs(discoveredMaze, cursor, mark, bfsPath);
                }
                break;

            // Map the maze
            case sf::Keyboard::Key::M:
                {
                    mapping = !mapping;
                    runSim = false;
                    Start = cursor;
                    CurrentIdeal = cursor;
                    clk.restart();
                    discoveredMaze.clear();
                    auto cw = maze.getCellWalls(cursor.i, cursor.j);
                    discoveredMaze.setCellWalls(cursor.i, cursor.j, cw);
                    unvisitedNodes.setAll(true);
                    unvisitedNodes.set(cursor.i, cursor.j, false);
                    inferredNodes.setAll(false);

                    bfs(discoveredMaze, cursor, unvisitedNodes, bfsPath);
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

    if (showBfs && markSet && !runSim && !mapping)
    {
        // Run BFS for display using the entire maze
        bfs(maze, cursor, mark, bfsPath);
    }

    if ((runSim && markSet) || mapping)
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
            if (mapping)
            {
                unvisitedNodes.set(cursor.i, cursor.j, false);

                // Infer the contents of a node if all surrounding nodes have been visited
                for (int i = 0; i < msize; ++i)
                {
                    for (int j = 0; j < nsize; ++j)
                    {
                        if (!unvisitedNodes.get(i, j))
                            continue;

                        if ((i + 1 >= msize || !unvisitedNodes.get(i + 1, j)) &&
                            (j + 1 >= nsize || !unvisitedNodes.get(i, j + 1)) &&
                            (i - 1 < 0 || !unvisitedNodes.get(i - 1, j)) &&
                            (j - 1 < 0 || !unvisitedNodes.get(i, j - 1)))
                        {
                            unvisitedNodes.set(i, j, false);
                            inferredNodes.set(i, j, true);
                        }
                    }
                }

                bfs(discoveredMaze, cursor, CurrentIdeal, bfsPath);
                bfs(discoveredMaze, Start, mark, bfsFinal);

                    OptimumNodes.setAll(false);
                    Node StackTop;
                    while (bfsFinal.size() > 0)
                    {
                        StackTop = bfsFinal.pop();
                        if (true == unvisitedNodes.get(StackTop.i,StackTop.j))
                        {
                        OptimumNodes.set(StackTop.i, StackTop.j, true);
                        }
                    }

                    bfs(discoveredMaze, cursor, OptimumNodes, bfsPath);
                    CurrentIdeal = bfsPath[bfsPath.size()-1];

                bfs(discoveredMaze, Start, mark, bfsFinal);
            }
            else
            {
                bfs(discoveredMaze, cursor, mark, bfsPath);
            }

            if(bfsPath.size() == 0)
            {
                //initilize todo:
                //Needs to compare fastest path after further exploration with fastest path previously,
                //and if the length of the new path is longerm undo changes, and if it is shorter, forget about the imaginary wall, and do process again.

                //Needs to do something after it has completed the check, to ensure it doesen't do itself again and again.
                //Fortunately this currently does nothing! DO NOT IMPLEMENT (This is the 'find faster paths' bit)
                NodeStack bfsCheck;
                NodeStack bfsStore = bfsFinal; NodeStack bfsImprov;
                Node A = bfsStore.pop(); Node B;
                int fakewall = 0;

                InitScore = ScorePath(bfsFinal);

                while(bfsStore.size() != 0)
                {
                    Node B = bfsStore.pop();

                    if(A.i < B.i)
                    {fakewall = 0;}
                    if(A.i > B.i)
                    {fakewall = 2;}
                    if(A.j < B.j)
                    {fakewall = 1;}
                    if(A.j > B.j)
                    {fakewall = 3;}

                    Walls = maze.getCellWalls(A.i,A.j);
                    Walls[fakewall] = true;
                    maze.setCellWalls(A.i,A.j,Walls);

                    bfs(discoveredMaze, Start, mark, bfsImprov);

                    if(InitScore > ScorePath(bfs(Improv)))
                    {bfsFinal = bfsImprov;}
                    else
                    {
                        Walls = maze.getCellWalls(A.i,A.j);
                        Walls[fakewall] = false;
                        maze.setCellWalls(A.i,A.j,Walls);
                    }
                }


            }

            // Stop when the goal is reached or unreachable
            if ((runSim && cursor == mark) || bfsPath.size() == 0)
            {
                runSim = false;
                //mapping = false;
            }
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
    else if (mapping)
    {
        // Undiscovered parts of the maze are show in gray
        maze.draw(window, 16.f, 2.f, sf::Color(255, 255, 255, 127));
        discoveredMaze.draw(window);

        // Mark visited cells
        sf::CircleShape visitedshape(2.f);
        sf::CircleShape inferredshape(2.f);
        visitedshape.setFillColor(sf::Color::Cyan);
        inferredshape.setFillColor(sf::Color::Magenta);
        for (int i = 0; i < msize; ++i)
        {
            for (int j = 0; j < nsize; ++j)
            {
                if (!unvisitedNodes.get(i, j))
                {
                    visitedshape.setPosition(j * 16.f + 6.f, i * 16.f + 6.f);
                    window.draw(visitedshape);
                }

                if (inferredNodes.get(i, j))
                {
                    inferredshape.setPosition(j * 16.f + 6.f, i * 16.f + 6.f);
                    window.draw(inferredshape);
                }
            }
        }
        
    }
    else
    {
        // Draw maze normally
        maze.draw(window);
    }
    
    if (showBfs)
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

        // Draw BFS path
        sf::VertexArray path2(sf::LinesStrip, bfsFinal.size());
        for (int i = 0; i < bfsFinal.size(); ++i)
        {
            auto n = bfsFinal[i];
            path2[i].position = {n.j * 16.f + 8.f, n.i * 16.f + 8.f};
            path2[i].color = sf::Color::Red;
        }
        window.draw(path2);
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

float ScorePath(NodeStack Path)
{
    bool orient = 0;
    bool turn = 0;

    float Score = 0;

    if(Path.size() == 0)
    {
        Score = 129;
        return Score;
    }

    current = Path.pop();

    while(Path.size() != 0)
    {
        next = Path.pop();

        if(current.i == next.i)
            {turn = 0;}
        if(current.j == next.j)
            {turn = 1;}

        if(orient != turn)
            {Score = Score+1;}
        if(orient == turn)
            {Score = Score+0.5;}

        orient = turn;
        current = next;
    }

    return Score;
}
