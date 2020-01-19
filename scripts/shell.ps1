# @file shell.bat
# @author Joseph R Miles <me@josephrmiles.com>
# @date 2020-01-19
#
# This is our shell config script, ported to PowerShell.

# PowerShell provides us an auto-magic variable that is the root directory of
# the script.
$env:Path += ";$($PSScriptRoot)"
