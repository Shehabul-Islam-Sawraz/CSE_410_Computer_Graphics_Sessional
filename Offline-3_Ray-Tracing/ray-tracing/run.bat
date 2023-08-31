@echo off

cls
del main.exe
g++ 1705066_main_x.cpp -o main.exe -lopengl32 -lglu32 -lfreeglut
main.exe
