# Building

## Tested Windows environment

- Windows 11
- Qt 6.11.1
- MinGW-w64 GCC 13.1
- CMake
- Ninja

## Qt Creator

Open CMakeLists.txt and select a Qt 6 Desktop MinGW kit.

## Command line

`powershell
cmake -S . -B C:\b\AXIOM -G Ninja 
  -DCMAKE_BUILD_TYPE=Debug 
  -DBUILD_TESTING=ON 
  -DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\mingw_64

cmake --build C:\b\AXIOM --parallel
ctest --test-dir C:\b\AXIOM --output-on-failure
`

For Windows deployment, build Release and use Qt's windeployqt tool. The release script in this repository
does this automatically.