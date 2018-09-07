# The Game of Life

Yet another implementation of Conway's Game of Life. This time in C with SDL2.

Compilation to WebAssembly and asm.js via `emscripten` is also supported.

```
# brew install sdl2
cc -O3 -lsdl2 -I/usr/local/Cellar/sdl2/2.0.8/include/SDL2/ src/*.c -o life

# install emsdk first
emcc -O3 -s WASM=1 -s USE_SDL=2 -I/usr/local/Cellar/sdl2/2.0.8/include/SDL2/ src/*.c -o life.html

# python -m SimpleHTTPServer and open http://localhost:8000/life.html
```

