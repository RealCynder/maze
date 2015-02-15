#include "BFS.hpp"
#include "BitArray2D.hpp"


bool bfs(const Maze<16, 16>& maze, Node start, Node goal, NodeStack& bfsPath)
{
    bfsPath.clear();

    if (start == goal)
    {
        bfsPath.push(goal);
        return true;
    }
    
    NodeQueue q;
    BitArray2D<16, 16> nodeMarks;
    EdgeStack edges;
    
    q.push(start);
    
    while (!q.empty())
    {
        auto v = q.pop();
        auto cw = maze.getCellWalls(v.i, v.j);
        for (int i = 0; i < 4; ++i)
        {
            if (cw[i])
                continue;

            auto u = v;
            if (0 == i)
                ++u.i;
            else if (1 == i)
                ++u.j;
            else if (2 == i)
                --u.i;
            else if (3 == i)
                --u.j;

            if (!nodeMarks.get(u.i, u.j))
            {
                nodeMarks.set(u.i, u.j, true);
                edges.push({v, u});
                q.push(u);
            }

            if (u == goal)
            {
                auto e = edges.pop();
                bfsPath.push(e.b);
                bfsPath.push(e.a);
                
                while (!edges.empty())
                {
                    e = edges.pop();
                    if (e.b == bfsPath.peek())
                        bfsPath.push(e.a);
                }

                return true;
            }      
        }
    }

    return false;
}
