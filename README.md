maze
====

A maze generator/solver/simulation used to prepare for a Micromouse robotics competition.

Dependencies
------------

SFML 2.x


Compiling and Running
-----------

    make
    ./maze


Controls
-----------

  - Arrow keys -- Move cursor
  - Space -- Place mark
  - B -- Show BFS path from cursor to mark
  - X -- Run search simulation from cursor to mark
  - WASD -- Toggle walls around cursor
  - F -- Fill maze with walls
  - C -- Clear walls
  - R -- Randomize maze
  - U -- Undo last operation that affects the entire maze (F, C, R, U, L)
  - V -- Save maze as string
  - L -- Load maze from string
  
