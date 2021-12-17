git clone https://github.com/WebAssembly/binaryen.git --branch version_100 --depth 1
git clone https://github.com/emscripten-core/emscripten.git --branch 2.0.16 --depth 1
cd binaryen 
cmake . && make
cd ..
