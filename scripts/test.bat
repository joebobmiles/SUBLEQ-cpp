@echo off

rem
rem @file test.bat
rem @author Joseph Miles <josephmiles2015@gmail.com>
rem @date 2019-06-11
rem
rem This is a script for automating the sorta tedious process of testing the
rem SUBLEQ compiler output.
rem

pushd build
subleqc.exe ..\data\test.s ..\data\test.x
subleq.exe ..\data\test.x
popd
