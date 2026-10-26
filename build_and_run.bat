@echo off
cd /d "%~dp0"
g++ -std=c++11 -Iinclude main.cpp src/CarBST.cpp src/CustomerList.cpp src/Menu.cpp src/Utils.cpp -o car_rental
if errorlevel 1 (pause & exit /b 1)
car_rental.exe
pause
