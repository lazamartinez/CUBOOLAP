cmake_minimum_required(VERSION 3.16)

project(CuboVision VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Sql Charts Gui Core REQUIRED)

# Archivos fuente
set(PROJECT_SOURCES
    src/main.cpp
    src/ui/VentanaPrincipal.cpp
    src/ui/VentanaPrincipal.h
    src/ui/DialogoConexion.cpp
    src/ui/DialogoConexion.h
    src/ui/DashboardReconocimiento.cpp
    src/ui/DashboardReconocimiento.h
    src/ui/Estilos.h
    src/core/GestorBaseDatos.cpp
    src/core/GestorBaseDatos.h
    src/core/AnalizadorEsquema.cpp
    src/core/AnalizadorEsquema.h
    src/ui/EstudioModelado.cpp
    src/ui/EstudioModelado.h
    src/ui/PanelDimensiones.cpp
    src/ui/PanelDimensiones.h
    src/ui/PanelMedidas.cpp
    src/ui/PanelMedidas.h
    src/ui/SimuladorRendimiento.cpp
    src/ui/SimuladorRendimiento.h
    src/ui/ConsolaProgreso.cpp
    src/ui/ConsolaProgreso.h
    src/core/MotorCarga.cpp
    src/core/MotorCarga.h
    src/ui/VisorOlap.cpp
    src/ui/VisorOlap.h
    src/ui/PanelAnalisis.cpp
    src/ui/PanelAnalisis.h
    src/ui/ConstructorConsultas.cpp
    src/ui/ConstructorConsultas.h
    src/ui/TablaDinamica.cpp
    src/ui/TablaDinamica.h
    src/core/GeneradorReportes.cpp
    src/core/GeneradorReportes.h
    src/ui/PantallaIntroduccion.cpp
    src/ui/PantallaIntroduccion.h
)

add_executable(CuboVision
    ${PROJECT_SOURCES}
)

target_link_libraries(CuboVision PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::Gui
    Qt6::Core
)

# Copiar DLLs de PostgreSQL si es necesario en Windows (Opcional, depende del entorno)
# O simplemente asumir que están en el PATH.
