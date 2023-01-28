$vs = "Visual Studio 17 2022"
cmake -DCMAKE_BUILD_TYPE=Release -G $vs -T host=x64 -A x64 -S . -B build
cmake --build build --config Release --target ALL_BUILD -j 18