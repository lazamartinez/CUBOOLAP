#!/bin/bash
# Script de inicio para Linux/macOS

echo "============================================"
echo "     INICIANDO CUBO VISION OLAP 2025"
echo "============================================"

# Crear directorio de build si no existe
mkdir -p build
cd build

echo ""
echo "[1/3] Configurando Proyecto con CMake..."
cmake ..
if [ $? -ne 0 ]; then
    echo "[ERROR] Fallo en configuracion CMake."
    exit 1
fi

echo ""
echo "[2/3] Compilando Codigo Fuente..."
cmake --build . --config Release -j$(nproc)
if [ $? -ne 0 ]; then
    echo "[ERROR] Fallo de Compilacion. Revisa el codigo."
    exit 1
fi

echo ""
echo "[3/3] Ejecutando Sistema..."
./CuboVision &

echo ""
echo "============================================"
echo "     SISTEMA INICIADO CORRECTAMENTE"
echo "============================================"
