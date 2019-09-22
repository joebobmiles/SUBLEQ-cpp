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

clang-cl /Zi src\subleq.cpp /o build\subleq.exe
clang-cl /Zi src\subleqc\subleqc.cpp /o build\subleqc.exe
