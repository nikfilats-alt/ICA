@echo off
echo Компилируем ICA...
g++ main.cpp src/bigint.cpp src/mathfuncs.cpp src/parser.cpp -I include -o ICA.exe -O2 -std=c++17
if %errorlevel% neq 0 (
    echo.
    echo [ОШИБКА] Компиляция не удалась. Убедись что g++ установлен.
    pause
    exit /b 1
)
echo Готово! Запускаю...
echo.
ICA.exe
pause
