cmd /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake.exe -S . -B build -G "Ninja"
cmake --build build --config Release -j 18