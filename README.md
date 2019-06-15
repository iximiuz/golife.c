# The Game of Life

Yet another implementation of Conway's Game of Life. This time in C with SDL2.

Compilation to WebAssembly and asm.js via `emscripten` is also supported. 

Play on <a target="_blank" href="https://iximiuz.com/golife">iximiuz.com/golife</a>.

```
# install SDL2 first (eg. brew install sdl2)
cc -O3 -lsdl2 -I/usr/local/Cellar/sdl2/2.0.8/include/SDL2/ src/*.c -o life
# ./life

# install emsdk first (see emsdk project)
emcc -O3 -s WASM=1 -s USE_SDL=2 -I/usr/local/Cellar/sdl2/2.0.8/include/SDL2/ src/*.c -o web/life.wasm -o web/life.js
# cd web; python ./server.py and open browser on http://localhost:8000/index.html
```

