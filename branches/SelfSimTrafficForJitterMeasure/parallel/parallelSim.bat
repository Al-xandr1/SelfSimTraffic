@echo off
echo.
echo Testing parallel SelfSimTrafficForJitterMeasure project
echo.
if "%*"=="" pause

rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

set NEDPATH=..
start /b /belownormal ..\SelfSimTrafficForJitterMeasure -p0,2 -n ..\ %*
start /b /belownormal ..\SelfSimTrafficForJitterMeasure -p1,2 -n ..\ %*
#start /b /belownormal ..\SelfSimTrafficForJitterMeasure -p2,4
#start /b /belownormal ..\SelfSimTrafficForJitterMeasure -p3,4 