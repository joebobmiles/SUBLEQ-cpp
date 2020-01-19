# @file build.ps1
# @author Joseph R Miles <me@josephrmiles.com>
# @date 2020-01-19
#
# This is our build script, migrated to PowerShell.

$build_directory = "./build/"

if (-not (Test-Path $build_directory -PathType Container))
{
    mkdir $build_directory
}

clang-cl /Zi "./src/subleq.cpp" /o "./build/subleq.exe"
clang-cl /Zi "./src/subleqc/subleqc.cpp" /o "./build/subleqc.exe"
