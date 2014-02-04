RipTide
=======

First game written in C w/ SDL

* Compiler - mingw (though anything support C11 *should* work

Profiled with Dr.Memory (on windows, no Valgrind!)

Build &amp; Run
===============

* download [libsdl (mingw version)]()
* download [sdl_ttf (mingw version)]()
* Place includes/bin/lib  dirs in your usual place
* Compiler Settings
* Standards = C11
* Linker -> SDL2.dll / SDL2_TTF.dll
* -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

Controls
========

Arrow keys to move
Eat them pixels!
