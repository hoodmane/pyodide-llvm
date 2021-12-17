Exploring some potential custom llvm passes to simplify compiling packages for
Pyodide. Currently I have the following passes:

- `VoidToIntReturn` converts functions that return `void` to functions that
return `int32`. The converted functions always return 0. This is to help build scipy.

Future ideas:
- A pass to fix the arguments of various C Python methods with incorrect arguments.
- A pass to handle function pointer casting

## VoidToIntReturn

I have a test which demonstrates a similar failure to scipy. Without
help, it fails with linker error messages:
```rust
wasm-ld: error: function signature mismatch: f
>> defined as (i32) -> void in library.a(library.o)
>> defined as (i32) -> i32 in src/other.o

wasm-ld: error: function signature mismatch: do_the_thing
>> defined as (i32, i32) -> i32 in src/main.o
>> defined as (i32, i32) -> void in library.a(library.o)
```
With the help of my llvm pass it links.

### To use

First build and start the docker image, then clone emscripten and binaryen:
```sh
./install_emscripten.sh
```

Then set environment variables for emscripten with
```sh
source activate_emscripten.sh
```
Then build the llvm pass:
```sh
mkdir passes/build
cd passes/build
cmake .. && make
cd ../..
```
Then compile the test:
```
cd tests/VoidToIntReturn
make
```
If everything went well, it should link successfully.
