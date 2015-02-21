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
  - M -- Map the maze
  - WASD -- Toggle walls around cursor
  - F -- Fill maze with walls
  - C -- Clear walls
  - R -- Randomize maze
  - U -- Undo last operation that affects the entire maze (F, C, R, U, L)
  - V -- Save maze as string
  - L -- Load maze from string
  

Building on Windows
-----------

You can download a compiler here:
http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download

Install gcc 4.9.2 for x86_64 with SEH exception handling. The installation folder should have a file called `mingw-w64.bat`. Run this to get a command window with the approriate environment variables set for running the compiler.

You will also need to install the SFML libraries such that the compiler can find them. Precompiled builds of SFML can be found at:
http://www.sfml-dev.org/download/sfml/2.2/

If you followed the above instructons to get a compiler, download the SFML files labeled "GCC 4.92 MinGW (SEH) - 64-bit". Extract the contents of the `bin` directory (several .dll files) into `C:\Program Files\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev1\bin`. Extract the contents of the `include` directory into `C:\Program Files\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev1\mingw64\lib\gcc\x86_64-w64-mingw32\4.9.2\include` (the file `SFML/System.cpp` should be at `...\4.9.2\include\SFML\System.cpp`). Extract the contents of the `lib` directory (several .a files) into `C:\Program Files\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev1\mingw64\lib\gcc\x86_64-w64-mingw32\4.9.2`.

Now you should be able to open a command window by running `mingw-w64.bat`, navigating to the directory containing the files from this repository, and running `mingw32-make` in the command window. If all goes well, it will build an executable called `maze`, which you can run by typing `maze` in the command window.
