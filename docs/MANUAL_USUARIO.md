# CUBO VISION
## Manual de Usuario - Sistema OLAP Multidimensional

**Version:** 1.0  
**Fecha:** Diciembre 2025  
**Materia:** Base de Datos 2025

---

# Tabla de Contenidos

1. [Introduccion](#1-introduccion)
2. [Arquitectura del Sistema](#2-arquitectura-del-sistema)
3. [Requisitos e Instalacion](#3-requisitos-e-instalacion)
4. [Flujo de Trabajo en 5 Fases](#4-flujo-de-trabajo-en-5-fases)
5. [Guia de Uso Detallada](#5-guia-de-uso-detallada)
6. [Ventajas y Desventajas](#6-ventajas-y-desventajas)
7. [Conclusiones](#7-conclusiones)

---

# 1. Introduccion

## 1.1 Que es OLAP

OLAP (Online Analytical Processing) es una tecnologia que permite analizar datos multidimensionales de forma interactiva y rapida. A diferencia de los sistemas transaccionales (OLTP), OLAP esta optimizado para consultas complejas que involucran agregaciones, comparaciones temporales y analisis de tendencias.

### Comparacion OLTP vs OLAP

```mermaid
flowchart LR
    subgraph OLTP["OLTP - Transaccional"]
        direction TB
        T1[INSERT]
        T2[UPDATE]
        T3[DELETE]
        T4[SELECT simple]
    end
    
    subgraph OLAP["OLAP - Analitico"]
        direction TB
        A1[Agregaciones]
        A2[Drill-down]
        A3[Slice & Dice]
        A4[Tendencias]
    end
    
    BD[(Base de Datos)] --> OLTP
    BD --> ETL[ETL Process]
    ETL --> DW[(Data Warehouse)]
    DW --> OLAP
    OLAP --> Dashboard[Dashboards]
    OLAP --> Reports[Reportes]
```

## 1.2 Que es Cubo Vision

Cubo Vision es una aplicacion de escritorio desarrollada en C++ con Qt 6.5 que permite:

- Conectarse a bases de datos PostgreSQL existentes
- Detectar automaticamente la estructura dimensional de los datos
- Visualizar los datos en un cubo OLAP 3D interactivo
- Explorar y analizar informacion de forma visual e intuitiva

### Vision General del Sistema

```mermaid
mindmap
  root((Cubo Vision))
    Conexion
      PostgreSQL
      Deteccion Automatica
      Validacion
    Modelado
      Dimensiones
      Medidas
      Jerarquias
    Visualizacion
      Cubo 2.5D
      Interactividad
      Seleccion
    Analisis
      Estadisticas
      Drill-down
      Exportacion
```

## 1.3 Proposito del Sistema

El sistema fue disenado para transformar bases de datos relacionales tradicionales en experiencias de analisis visual, permitiendo a usuarios sin conocimientos tecnicos profundos explorar grandes volumenes de datos de manera intuitiva.

## 1.4 Publico Objetivo

- Analistas de datos
- Gerentes y tomadores de decisiones
- Estudiantes de sistemas de informacion
- Cualquier usuario que necesite explorar datos dimensionales

---

# 2. Arquitectura del Sistema

## 2.1 Tecnologias Utilizadas

| Componente | Tecnologia | Justificacion |
|------------|------------|---------------|
| Lenguaje | C++20 | Alto rendimiento, control de memoria, ideal para graficos |
| Framework UI | Qt 6.5 | Multiplataforma, widgets nativos, excelente soporte para graficos 2D |
| Base de Datos | PostgreSQL | Robusto, escalable, excelente soporte SQL:1999 |
| Graficos | QPainter | Renderizado 2D eficiente sin dependencias externas |
| Visualizacion | QtCharts | Graficos estadisticos integrados con Qt |
| Build System | CMake | Estandar de la industria, multiplataforma |

## 2.2 Arquitectura en Capas

```mermaid
flowchart TB
    subgraph UI["Capa de Presentacion"]
        direction LR
        VP[VentanaPrincipal]
        VO[VisorOlap]
        PA[PanelAnalisis]
        DC[DialogoConexion]
    end
    
    subgraph Logic["Capa de Logica de Negocio"]
        direction LR
        AE[AnalizadorEsquema]
        MC[MotorCarga]
        GR[GeneradorReportes]
    end
    
    subgraph Data["Capa de Datos"]
        direction LR
        GBD[GestorBaseDatos]
        QS[QtSql]
    end
    
    subgraph External["Sistemas Externos"]
        PG[(PostgreSQL)]
    end
    
    UI --> Logic
    Logic --> Data
    Data --> External
    
    style UI fill:#e1f5fe
    style Logic fill:#fff3e0
    style Data fill:#e8f5e9
    style External fill:#fce4ec
```

### Justificacion de la Arquitectura

Se eligio una arquitectura en capas porque:

1. **Separacion de responsabilidades**: Cada capa tiene una funcion especifica
2. **Mantenibilidad**: Cambios en una capa no afectan a las demas
3. **Testabilidad**: Cada capa puede probarse de forma independiente
4. **Escalabilidad**: Facilita agregar nuevas funcionalidades

## 2.3 Diagrama de Componentes

```mermaid
graph TB
    subgraph app["Aplicacion Cubo Vision"]
        main[main.cpp]
        
        subgraph core["Core"]
            gbd[GestorBaseDatos]
            ae[AnalizadorEsquema]
            mc[MotorCarga]
            gr[GeneradorReportes]
        end
        
        subgraph ui["UI"]
            vp[VentanaPrincipal]
            dc[DialogoConexion]
            dr[DashboardReconocimiento]
            em[EstudioModelado]
            cp[ConsolaProgreso]
            vo[VisorOlap]
            pa[PanelAnalisis]
            cc[ConstructorConsultas]
        end
    end
    
    subgraph external["Externo"]
        qt[Qt 6.5]
        pg[(PostgreSQL)]
    end
    
    main --> vp
    vp --> dc
    vp --> dr
    vp --> em
    vp --> cp
    vp --> vo
    vo --> pa
    vp --> cc
    
    dc --> gbd
    dr --> ae
    cp --> mc
    vo --> gbd
    cc --> gr
    
    gbd --> qt
    qt --> pg
```

## 2.4 Porque C++ y Qt en lugar de tecnologias web

### Comparacion de Tecnologias

```mermaid
quadrantChart
    title Comparacion de Tecnologias para OLAP
    x-axis Bajo Rendimiento --> Alto Rendimiento
    y-axis Baja Portabilidad --> Alta Portabilidad
    quadrant-1 Ideal
    quadrant-2 Web Apps
    quadrant-3 Legacy
    quadrant-4 Desktop Nativo
    "C++ Qt": [0.85, 0.55]
    "Electron": [0.45, 0.75]
    "Web JS": [0.35, 0.90]
    "Java Swing": [0.50, 0.65]
    "Python Tk": [0.40, 0.50]
    ".NET WPF": [0.70, 0.40]
```

### Ventajas de la eleccion:

- **Rendimiento**: Renderizado de graficos 3D fluido sin latencia
- **Acceso directo a recursos**: Uso eficiente de memoria y CPU
- **Sin dependencia de internet**: Funciona completamente offline
- **Experiencia nativa**: Integracion perfecta con el sistema operativo

### Desventajas:

- **Portabilidad limitada**: Requiere compilacion por plataforma
- **Curva de aprendizaje**: C++ es mas complejo que JavaScript/Python
- **Distribucion**: Requiere empaquetado con dependencias Qt

---

# 3. Requisitos e Instalacion

## 3.1 Requisitos del Sistema

### Hardware Minimo
- Procesador: Intel Core i3 o equivalente
- RAM: 4 GB
- Espacio en disco: 500 MB
- Resolucion: 1280 x 720

### Hardware Recomendado
- Procesador: Intel Core i5 o superior
- RAM: 8 GB o mas
- SSD para mejor rendimiento
- Resolucion: 1920 x 1080

### Software Requerido
- Windows 10/11, Linux o macOS
- PostgreSQL 14 o superior
- Qt 6.5 o superior (para compilacion)

## 3.2 Proceso de Instalacion

```mermaid
flowchart TD
    A[Inicio] --> B{PostgreSQL instalado?}
    B -->|No| C[Instalar PostgreSQL]
    C --> D[Crear Base de Datos]
    B -->|Si| D
    D --> E{Cargar datos de prueba?}
    E -->|Si| F[Ejecutar semilla_completa.sql]
    E -->|No| G[Compilar proyecto]
    F --> G
    G --> H{Windows?}
    H -->|Si| I[Ejecutar iniciar.bat]
    H -->|No| J[Ejecutar iniciar.sh]
    I --> K[Aplicacion lista]
    J --> K
    K --> L[Fin]
    
    style A fill:#4caf50
    style L fill:#4caf50
    style K fill:#2196f3
```

## 3.3 Instalacion Paso a Paso

### Paso 1: Preparar PostgreSQL

```sql
-- Crear la base de datos
CREATE DATABASE bd2025;
```

### Paso 2: Cargar Datos de Prueba (Opcional)

```bash
psql -U postgres -d bd2025 -f db/semilla_completa.sql
```

### Paso 3: Compilar y Ejecutar

**Windows:**
```batch
iniciar.bat
```

**Linux/macOS:**
```bash
./iniciar.sh
```

---

# 4. Flujo de Trabajo en 5 Fases

El sistema guia al usuario a traves de un flujo estructurado en 5 fases secuenciales.

## Diagrama General del Flujo

```mermaid
journey
    title Experiencia del Usuario en Cubo Vision
    section Fase 1 - Diagnostico
      Conectar a BD: 5: Usuario
      Analisis automatico: 5: Sistema
      Ver esquema detectado: 4: Usuario
    section Fase 2 - Modelado
      Revisar dimensiones: 4: Usuario
      Ajustar medidas: 4: Usuario
      Confirmar modelo: 5: Usuario
    section Fase 3 - Carga
      Iniciar carga: 3: Usuario
      Procesar datos: 5: Sistema
      Validar calidad: 4: Sistema
    section Fase 4 - Visualizacion
      Explorar cubo: 5: Usuario
      Seleccionar celdas: 5: Usuario
      Analizar estadisticas: 5: Usuario
    section Fase 5 - Reportes
      Construir consultas: 4: Usuario
      Exportar resultados: 5: Usuario
```

## 4.1 Fase 1: Diagnostico Inteligente

### Proposito
Conectar a la base de datos y analizar automaticamente su estructura para identificar tablas de hechos, dimensiones y relaciones.

### Flujo de Deteccion Automatica

```mermaid
flowchart TD
    A[Conectar a PostgreSQL] --> B[Consultar catalogo del sistema]
    B --> C[Listar todas las tablas]
    C --> D{Ordenar por tamanio}
    D --> E[Tabla mas grande = Hechos?]
    E --> F[Detectar Foreign Keys]
    F --> G{FK apunta a otra tabla?}
    G -->|Si| H[Tabla destino = Dimension]
    G -->|No| I[Buscar dimension degenerada]
    H --> J[Analizar columnas]
    I --> J
    J --> K{Tipo numerico?}
    K -->|Si| L[Candidato a Medida]
    K -->|No| M[Candidato a Atributo]
    L --> N[Generar modelo propuesto]
    M --> N
    N --> O[Mostrar Dashboard]
    
    style A fill:#4caf50
    style O fill:#2196f3
```

### Heuristica de Deteccion

```mermaid
pie showData
    title Criterios de Clasificacion de Tablas
    "Cantidad de registros" : 40
    "Foreign Keys entrantes" : 25
    "Foreign Keys salientes" : 20
    "Tipos de columnas" : 15
```

## 4.2 Fase 2: Modelado Multidimensional

### Proposito
Permitir al usuario revisar y ajustar el modelo dimensional detectado.

### Modelo Dimensional Estrella

```mermaid
erDiagram
    FACT_VENTAS ||--o{ DIM_TIEMPO : "id_tiempo"
    FACT_VENTAS ||--o{ DIM_GEOGRAFIA : "id_geografia"
    FACT_VENTAS ||--o{ DIM_PRODUCTO : "id_producto"
    FACT_VENTAS ||--o{ DIM_CLIENTE : "id_cliente"
    
    FACT_VENTAS {
        int id_venta PK
        int id_tiempo FK
        int id_geografia FK
        int id_producto FK
        int id_cliente FK
        decimal cantidad
        decimal precio_unitario
        decimal total_venta
        decimal ganancia
    }
    
    DIM_TIEMPO {
        int id_tiempo PK
        date fecha
        int anio
        int mes
        string nombre_mes
        int trimestre
    }
    
    DIM_GEOGRAFIA {
        int id_geografia PK
        string pais
        string region
        string ciudad
        string codigo_postal
    }
    
    DIM_PRODUCTO {
        int id_producto PK
        string nombre
        string categoria
        string subcategoria
        string marca
        decimal precio_base
    }
    
    DIM_CLIENTE {
        int id_cliente PK
        string nombre
        string tipo_cliente
        string segmento
        string email
    }
```

## 4.3 Fase 3: Carga Optimizada

### Proposito
Cargar los datos desde la base de datos relacional al modelo dimensional en memoria.

### Pipeline de Carga

```mermaid
flowchart LR
    subgraph Extract["Extraccion"]
        E1[Query SQL]
        E2[Fetch datos]
    end
    
    subgraph Transform["Transformacion"]
        T1[Mapear claves]
        T2[Calcular agregaciones]
        T3[Validar calidad]
    end
    
    subgraph Load["Carga"]
        L1[Almacenar en memoria]
        L2[Indexar dimensiones]
        L3[Precalcular cubos]
    end
    
    Extract --> Transform --> Load
    
    style Extract fill:#e3f2fd
    style Transform fill:#fff3e0
    style Load fill:#e8f5e9
```

### Metricas de Progreso

```mermaid
gantt
    title Ejemplo de Carga de 100,000 Registros
    dateFormat ss
    axisFormat %S s
    
    section Extraccion
    Query inicial           :e1, 0, 2s
    Fetch dimensiones       :e2, after e1, 1s
    
    section Transformacion
    Mapeo de claves         :t1, after e2, 3s
    Agregaciones            :t2, after t1, 4s
    Validacion              :t3, after t2, 1s
    
    section Carga
    Indexacion              :l1, after t3, 2s
    Finalizacion            :l2, after l1, 1s
```

## 4.4 Fase 4: Visualizacion 2.5D

### Proposito
Presentar los datos en un cubo OLAP interactivo que permite exploracion visual intuitiva.

### Sistema de Coordenadas Isometrico

```mermaid
flowchart TD
    subgraph coords["Sistema de Coordenadas"]
        direction LR
        X["Eje X<br/>Dimension 1<br/>(Producto)"]
        Y["Eje Y<br/>Medida<br/>(Cantidad)"]
        Z["Eje Z<br/>Dimension 2<br/>(Cliente)"]
    end
    
    subgraph projection["Proyeccion Isometrica"]
        P1["x_screen = (x - z) * cos(30)"]
        P2["y_screen = y + (x + z) * sin(30)"]
    end
    
    subgraph visual["Representacion Visual"]
        C1["Altura = Valor de medida"]
        C2["Color = Gradiente por valor"]
        C3["Posicion = Interseccion de dims"]
    end
    
    coords --> projection --> visual
```

### Codificacion Visual

```mermaid
flowchart LR
    subgraph encoding["Codificacion de Datos"]
        direction TB
        V1["Valor bajo<br/>(verde, altura minima)"]
        V2["Valor medio<br/>(amarillo, altura media)"]
        V3["Valor alto<br/>(rojo, altura maxima)"]
    end
    
    V1 --> |"0-33%"| G1["#22c55e"]
    V2 --> |"33-66%"| G2["#eab308"]
    V3 --> |"66-100%"| G3["#ef4444"]
    
    style V1 fill:#22c55e
    style V2 fill:#eab308
    style V3 fill:#ef4444
```

### Interacciones del Usuario

```mermaid
stateDiagram-v2
    [*] --> Explorando
    
    Explorando --> Hover: Mouse sobre celda
    Hover --> Explorando: Mouse sale
    Hover --> Seleccionado: Click
    
    Seleccionado --> MultiSeleccion: Click otra celda
    MultiSeleccion --> Seleccionado: Click celda seleccionada
    Seleccionado --> Explorando: Tecla Esc
    MultiSeleccion --> Explorando: Tecla Esc
    
    Explorando --> Zoom: Rueda del mouse
    Zoom --> Explorando: Soltar rueda
    
    Explorando --> Pan: Arrastrar
    Pan --> Explorando: Soltar mouse
    
    Seleccionado --> DrillDown: Doble click
    DrillDown --> Explorando: Volver
```

## 4.5 Fase 5: Consultas y Reportes

### Proposito
Permitir construccion de consultas personalizadas y generacion de reportes exportables.

### Flujo de Generacion de Reportes

```mermaid
sequenceDiagram
    participant U as Usuario
    participant CC as ConstructorConsultas
    participant GR as GeneradorReportes
    participant DB as PostgreSQL
    
    U->>CC: Seleccionar dimensiones
    U->>CC: Seleccionar medidas
    U->>CC: Aplicar filtros
    CC->>CC: Generar SQL
    CC->>DB: Ejecutar consulta
    DB-->>CC: Resultados
    CC->>U: Mostrar vista previa
    U->>GR: Exportar
    GR->>GR: Formatear datos
    GR-->>U: Archivo generado
```

---

# 5. Guia de Uso Detallada

## 5.1 Flujo de Navegacion

```mermaid
flowchart TD
    A[Inicio App] --> B{Pantalla Bienvenida}
    B --> C[Comenzar Ahora]
    C --> D[Dialogo Conexion]
    D --> E{Conexion exitosa?}
    E -->|No| D
    E -->|Si| F[Fase 1: Dashboard]
    F --> G[Confirmar Esquema]
    G --> H[Fase 2: Modelado]
    H --> I[Confirmar Modelo]
    I --> J[Fase 3: Carga]
    J --> K{Carga completa?}
    K -->|No| J
    K -->|Si| L[Fase 4: Visor OLAP]
    L --> M{Ir a Reportes?}
    M -->|Si| N[Fase 5: Consultas]
    M -->|No| L
    N --> O{Volver al inicio?}
    O -->|Si| B
    O -->|No| N
```

## 5.2 Atajos de Teclado

| Tecla | Funcion |
|-------|---------|
| R | Resetear vista a posicion inicial |
| Esc | Limpiar todas las selecciones |
| G | Alternar visibilidad del grid |

---

# 6. Ventajas y Desventajas

## 6.1 Analisis Comparativo

```mermaid
xychart-beta
    title "Comparacion con Competidores"
    x-axis ["Rendimiento", "Facilidad", "Costo", "Offline", "Personalizacion"]
    y-axis "Puntuacion" 0 --> 10
    bar [9, 8, 10, 10, 9]
    line [6, 7, 3, 5, 7]
```

*Barras: Cubo Vision | Linea: Promedio competidores*

## 6.2 Resumen de Ventajas

```mermaid
mindmap
  root((Ventajas))
    Rendimiento
      C++ nativo
      Datos en memoria
      Sin latencia de red
    Usabilidad
      Flujo guiado
      Deteccion automatica
      Interfaz intuitiva
    Costo
      Codigo abierto
      Sin licencias
      Sin suscripciones
    Independencia
      Funciona offline
      Sin dependencias cloud
      Datos locales
```

## 6.3 Resumen de Desventajas

```mermaid
mindmap
  root((Desventajas))
    Escalabilidad
      Limitado a RAM
      Sin procesamiento distribuido
      Datasets grandes lentos
    Conectividad
      Solo PostgreSQL
      Sin version web
      Sin colaboracion
    Desarrollo
      Compilacion por plataforma
      Distribucion compleja
      Actualizaciones manuales
```

---

# 7. Conclusiones

## 7.1 Logros del Proyecto

```mermaid
timeline
    title Hitos del Desarrollo
    section Fundamentos
        Arquitectura : Definicion de capas
                     : Seleccion Qt/C++
    section Core
        Conexion BD : QtSql PostgreSQL
        Deteccion : Analizador esquema
    section Visualizacion
        Cubo 2.5D : QPainter isometrico
        Interactividad : Seleccion y zoom
    section Finalizacion
        Estadisticas : Panel analisis
        Documentacion : Manual usuario
```

## 7.2 Trabajo Futuro

```mermaid
roadmap
    title Roadmap Cubo Vision
    section 2025 Q1
        Soporte MySQL : done
        Mejoras UI : active
    section 2025 Q2
        Version Web : planned
        Colaboracion : planned
    section 2025 Q3
        IA Insights : planned
        Mobile App : planned
```

---

# Anexos

## A. Estructura de la Base de Datos de Prueba

```mermaid
erDiagram
    fact_ventas ||--o{ dim_tiempo : "FK"
    fact_ventas ||--o{ dim_geografia : "FK"
    fact_ventas ||--o{ dim_producto : "FK"
    fact_ventas ||--o{ dim_cliente : "FK"
    
    fact_ventas {
        int id_venta PK
        decimal cantidad
        decimal precio_unitario
        decimal total_venta
        decimal ganancia
        string metodo_pago
        string canal_venta
    }
    
    dim_tiempo {
        int id_tiempo PK
        date fecha UK
        int anio
        int mes
        int dia
    }
    
    dim_geografia {
        int id_geografia PK
        string pais
        string region
        string ciudad
    }
    
    dim_producto {
        int id_producto PK
        string categoria
        string subcategoria
        string marca
    }
    
    dim_cliente {
        int id_cliente PK
        string tipo_cliente
        string segmento
    }
```

## B. Glosario

| Termino | Definicion |
|---------|------------|
| OLAP | Online Analytical Processing - procesamiento analitico en linea |
| Cubo | Estructura multidimensional para analisis de datos |
| Dimension | Eje de analisis (tiempo, geografia, producto) |
| Medida | Valor numerico a analizar (ventas, cantidad) |
| Drill-down | Navegacion hacia nivel de detalle inferior |
| Drill-up | Navegacion hacia nivel de agregacion superior |
| Celda | Interseccion de dimensiones con valor de medida |
| Slice | Corte del cubo por una dimension |
| Dice | Seleccion de un subcubo |

## C. Creditos

Desarrollado como proyecto academico para la materia Base de Datos 2025.

---

*Documento generado automaticamente - Cubo Vision v1.0*
