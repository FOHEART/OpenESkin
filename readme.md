### 编译工程
cmake -S . -B build -DCMAKE_PREFIX_PATH="D:\Qt5.15.2\5.15.2\msvc2019_64\lib\cmake\Qt5" -G "Visual Studio 17 2022" -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
### 编译Debug Release版本
cmake --build build --config Debug
cmake --build build --config Release
