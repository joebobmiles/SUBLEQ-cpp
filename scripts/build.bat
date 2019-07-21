@echo off

rem
rem @file build.bat
rem @author Joseph Miles <josephmiles2015@gmail.com>
rem @date 2019-02-03
rem
rem This is the build script for Windows, using clang-cl.
rem

if not exist build\ (
    md build\
)

pushd build\
echo Building subleq.exe...
clang-cl /Zi ..\src\subleq.cpp /o subleq.exe
echo Building subleqc.exe...
clang-cl /Zi ..\src\subleqc\subleqc.cpp /o subleqc.exe
popd
