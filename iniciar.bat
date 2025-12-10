@echo off
setlocal

echo ============================================
echo      INICIANDO CUBO VISION OLAP 2025
echo ============================================

REM Configurar rutas de Qt6 y MinGW
set QT_DIR=C:\Qt\6.10.1\mingw_64
set MINGW_DIR=C:\Qt\Tools\mingw1310_64
set CMAKE_DIR=C:\Qt\Tools\CMake_64\bin
set NINJA_DIR=C:\Qt\Tools\Ninja

set CMAKE_PREFIX_PATH=%QT_DIR%
set PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%CMAKE_DIR%;%NINJA_DIR%;%PATH%

echo [INFO] Usando Qt6 en: %QT_DIR%
echo [INFO] Usando MinGW en: %MINGW_DIR%

REM Limpiar y crear directorio de build
if exist build rmdir /s /q build
mkdir build
cd build

echo.
echo [1/3] Configurando Proyecto con CMake (MinGW)...
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_C_COMPILER="%MINGW_DIR%\bin\gcc.exe" -DCMAKE_CXX_COMPILER="%MINGW_DIR%\bin\g++.exe"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo en configuracion CMake.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Compilando Codigo Fuente...
cmake --build . -j4
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Fallo de Compilacion. Revisa el codigo.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Ejecutando Sistema...
start CuboVision.exe

echo.
echo ============================================
echo      SISTEMA INICIADO CORRECTAMENTE
echo ============================================
pause
