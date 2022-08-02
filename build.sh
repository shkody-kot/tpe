#! /bin/bash

cd public/static
rm enc.js
rm enc.wasm
cd ../..
pwd
./emsdk/upstream/emscripten/emcc encryption.cpp tpe.cpp aes_rnd.cpp -s WASM=1 -s EXPORT_ALL=1 -s EXPORTED_RUNTIME_METHODS='["cwrap", "ccall", "allocate", "intArrayFromString"]' -s 'EXPORTED_FUNCTIONS=["_free", "_main"]' -O3 -o enc.js || exit 1
mv enc.js ./public/static
mv enc.wasm ./public/static
cd public
npx nodemon server.js
