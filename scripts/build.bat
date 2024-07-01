@echo off

set BUILD_DIR=build
set GENERATOR="MinGW Makefiles"
@REM set GENERATOR="Visual Studio 17 2022" 
@REM set 'TOOLCHAIN="ClangCL"' for clang toolchain. Note: supported only in visual studio generator
set PLATFORM=x64
set TOOLCHAIN=""
set CONFIGURATION=Debug

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -G %GENERATOR% -DCMAKE_BUILD_TYPE=%CONFIGURATION% -T %TOOLCHAIN% ..

cmake --build . --config %CONFIGURATION%

cd ..