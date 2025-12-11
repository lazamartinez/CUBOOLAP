@echo off
REM Script para ejecutar la semilla de bd4-2025
REM Busca PostgreSQL en ubicaciones comunes

echo ============================================
echo Ejecutando semilla de bd4-2025
echo ============================================
echo.

REM Buscar PostgreSQL en ubicaciones comunes
set PSQL_PATH=

if exist "C:\Program Files\PostgreSQL\16\bin\psql.exe" set PSQL_PATH=C:\Program Files\PostgreSQL\16\bin\psql.exe
if exist "C:\Program Files\PostgreSQL\15\bin\psql.exe" if "%PSQL_PATH%"=="" set PSQL_PATH=C:\Program Files\PostgreSQL\15\bin\psql.exe
if exist "C:\Program Files\PostgreSQL\14\bin\psql.exe" if "%PSQL_PATH%"=="" set PSQL_PATH=C:\Program Files\PostgreSQL\14\bin\psql.exe
if exist "C:\Program Files\PostgreSQL\13\bin\psql.exe" if "%PSQL_PATH%"=="" set PSQL_PATH=C:\Program Files\PostgreSQL\13\bin\psql.exe
if exist "C:\Program Files (x86)\PostgreSQL\16\bin\psql.exe" if "%PSQL_PATH%"=="" set PSQL_PATH=C:\Program Files (x86)\PostgreSQL\16\bin\psql.exe
if exist "C:\Program Files (x86)\PostgreSQL\15\bin\psql.exe" if "%PSQL_PATH%"=="" set PSQL_PATH=C:\Program Files (x86)\PostgreSQL\15\bin\psql.exe

if "%PSQL_PATH%"=="" (
    echo ERROR: No se encontro PostgreSQL instalado.
    echo.
    echo Por favor, instala PostgreSQL o agrega psql.exe al PATH.
    echo.
    echo Ubicaciones buscadas:
    echo   - C:\Program Files\PostgreSQL\[version]\bin\psql.exe
    echo   - C:\Program Files (x86)\PostgreSQL\[version]\bin\psql.exe
    echo.
    pause
    exit /b 1
)

echo PostgreSQL encontrado en: %PSQL_PATH%
echo.

REM Configurar variables de entorno para PostgreSQL
set PGPASSWORD=laza
set PGUSER=postgresql

echo Paso 1: Eliminando base de datos existente (si existe)...
"%PSQL_PATH%" -U postgresql -d postgres -c "DROP DATABASE IF EXISTS \"bd4-2025\";"
if errorlevel 1 (
    echo ERROR: No se pudo conectar a PostgreSQL.
    echo Verifica que:
    echo   1. PostgreSQL este ejecutandose
    echo   2. El usuario 'postgresql' exista
    echo   3. La password sea 'laza'
    echo.
    pause
    exit /b 1
)

echo Paso 2: Creando base de datos bd4-2025...
"%PSQL_PATH%" -U postgresql -d postgres -c "CREATE DATABASE \"bd4-2025\";"
if errorlevel 1 (
    echo ERROR: No se pudo crear la base de datos.
    pause
    exit /b 1
)

echo Paso 3: Ejecutando script de semilla...
echo (Esto puede tomar 10-30 segundos)
echo.
"%PSQL_PATH%" -U postgresql -d bd4-2025 -f "%~dp0seed_bd4-2025.sql"
if errorlevel 1 (
    echo ERROR: Hubo un problema ejecutando el script de semilla.
    pause
    exit /b 1
)

echo.
echo ============================================
echo EXITO: Base de datos bd4-2025 creada!
echo ============================================
echo.
echo Estadisticas:
"%PSQL_PATH%" -U postgresql -d bd4-2025 -c "SELECT COUNT(*) as total_ventas FROM fact_ventas;"
echo.
"%PSQL_PATH%" -U postgresql -d bd4-2025 -c "SELECT COUNT(*) as productos FROM dim_producto;"
echo.
"%PSQL_PATH%" -U postgresql -d bd4-2025 -c "SELECT COUNT(*) as meses FROM dim_tiempo;"
echo.
"%PSQL_PATH%" -U postgresql -d bd4-2025 -c "SELECT COUNT(*) as clientes FROM dim_cliente;"
echo.
echo ============================================
echo La base de datos esta lista para usar!
echo ============================================
echo.
echo Conexion:
echo   Host: localhost
echo   Puerto: 5432
echo   Usuario: postgresql
echo   Password: laza
echo   Database: bd4-2025
echo.
pause
