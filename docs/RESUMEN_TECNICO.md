# Resumen Técnico - Cubo Vision OLAP

## Indice

1. [Descripción General](#descripción-general)
2. [Arquitectura del Sistema](#arquitectura-del-sistema)
3. [Fases del Sistema](#fases-del-sistema)
4. [Operaciones OLAP](#operaciones-olap)
5. [Componentes Simulados vs Reales](#componentes-simulados-vs-reales)
6. [Ventajas y Desventajas](#ventajas-y-desventajas)
7. [Stack Tecnológico](#stack-tecnológico)

---

## Descripción General

**Cubo Vision** es un sistema OLAP (Online Analytical Processing) educativo desarrollado en C++20 con Qt 6.5 que permite analizar datos multidimensionales almacenados en PostgreSQL mediante una interfaz visual interactiva.

### Objetivo Principal

Proporcionar una plataforma completa de análisis multidimensional que guíe al usuario desde la conexión a la base de datos hasta la generación de reportes, pasando por la visualización 3D interactiva del cubo OLAP.

### Público Objetivo

- Estudiantes de bases de datos
- Analistas de datos
- Desarrolladores aprendiendo OLAP
- Profesionales evaluando herramientas de BI

---

## Arquitectura del Sistema

### Patrón Arquitectónico

El sistema utiliza una arquitectura **MVC (Model-View-Controller)** adaptada para Qt:

```
┌─────────────────────────────────────────────────┐
│              PRESENTACIÓN (UI)                   │
│  VentanaPrincipal, Diálogos, Visualizadores     │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│           LÓGICA DE NEGOCIO (Core)              │
│  AnalizadorEsquema, GestorBaseDatos, MotorCarga │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│          PERSISTENCIA (PostgreSQL)              │
│     Tablas de Hechos y Dimensiones              │
└─────────────────────────────────────────────────┘
```

### Componentes Principales

#### 1. Capa de Presentación (UI)

| Componente                | Responsabilidad                       |
| ------------------------- | ------------------------------------- |
| `VentanaPrincipal`        | Orquestación de fases y navegación    |
| `DialogoConexion`         | Gestión de credenciales y conexión    |
| `DashboardReconocimiento` | Visualización de diagnóstico (Fase 1) |
| `EstudioModelado`         | Configuración de dimensiones (Fase 2) |
| `ConsolaProgreso`         | Monitoreo de carga (Fase 3)           |
| `VisorOlap`               | Visualización 3D del cubo (Fase 4)    |
| `PanelAnalisis`           | Operaciones OLAP (Fase 4)             |
| `ConstructorConsultas`    | Builder de queries (Fase 5)           |

#### 2. Capa de Lógica (Core)

| Componente          | Responsabilidad                     |
| ------------------- | ----------------------------------- |
| `GestorBaseDatos`   | Singleton para conexión PostgreSQL  |
| `AnalizadorEsquema` | Detección de tablas fact/dim y FK   |
| `MotorCarga`        | Procesamiento y validación de datos |
| `GeneradorReportes` | Exportación CSV/PDF                 |

#### 3. Capa de Datos

- **PostgreSQL 14+**: Base de datos relacional
- **Esquema Estrella**: Tablas de hechos y dimensiones
- **QtSql**: Driver QPSQL para comunicación

---

## Fases del Sistema

### Fase 1: Diagnóstico del Esquema

**Objetivo**: Analizar automáticamente la estructura de la base de datos conectada.

**Proceso**:
1. Usuario ingresa credenciales de PostgreSQL
2. Sistema valida conexión y permisos
3. `AnalizadorEsquema` consulta `information_schema`:
   - Detecta tablas con prefijo `dim_*` (dimensiones)
   - Detecta tablas con prefijo `fact_*` (hechos)
   - Analiza foreign keys para relaciones
   - Calcula estadísticas (filas, tamaño, distribución)

**Visualización**:
- **Cards de estadísticas**: Total tablas, registros, relaciones, tamaño
- **Gráfico de pastel**: Distribución de datos por tabla (con % y leyenda)
- **Gráfico de barras**: Top 5 tablas por número de filas
- **Lista de tablas**: Detalle de todas las tablas detectadas
- **Sugerencias**: Posibles tablas de hechos y dimensiones

**Tecnología**:
- `QSqlQuery` para consultas a `information_schema`
- `QtCharts` (QPieSeries, QHorizontalBarSeries)
- `QListWidget` para tablas

---

### Fase 2: Modelado Multidimensional

**Objetivo**: Definir la estructura del cubo OLAP (dimensiones y medidas).

**Proceso**:
1. Sistema carga dimensiones detectadas en Fase 1
2. Usuario revisa dimensiones y jerarquías en `PanelDimensiones`
3. Usuario revisa medidas numéricas en `PanelMedidas`
4. `SimuladorRendimiento` estima tamaño del cubo

**Componentes**:

| Panel                    | Función                                                                   |
| ------------------------ | ------------------------------------------------------------------------- |
| **PanelDimensiones**     | Lista jerárquica de dimensiones (ej: Tiempo → Año → Mes)                  |
| **PanelMedidas**         | Medidas numéricas con funciones de agregación (SUM, AVG, COUNT, MAX, MIN) |
| **SimuladorRendimiento** | Estimación de cardinalidad y tamaño del cubo                              |

**Visualización**:
- **Explorador de dimensiones**: TreeView con jerarquías
- **Definición de medidas**: Lista con tipo de agregación
- **Indicadores de rendimiento**: Cardinalidad estimada, índice de optimización

**Tecnología**:
- `QTreeWidget` para jerarquías
- `QListWidget` para medidas
- Cálculos de cardinalidad: `Π(|dim_i|)` para cubo denso

---

### Fase 3: Carga de Datos

**Objetivo**: Procesar y cargar datos en memoria para análisis OLAP.

**Proceso**:
1. `MotorCarga` ejecuta queries de agregación
2. Construye estructura de cubo en memoria
3. Valida calidad de datos (nulos, duplicados)
4. Genera índices para acceso rápido

**Monitoreo en Tiempo Real**:
- **Barra de progreso**: 0-100% con gradiente animado
- **Estadísticas**:
  - Registros procesados
  - Velocidad (reg/seg)
  - Tiempo restante estimado
  - Calidad de datos (%)
- **Bitácora**: Log con timestamps de operaciones
- **Alertas**: Panel de advertencias y errores

**Visualización**:
- **Cards de métricas**: Registros, velocidad, tiempo, calidad
- **Barra de progreso**: Gradiente verde-azul animado
- **Log de bitácora**: Consola estilo terminal (fondo oscuro, texto verde)
- **Panel de alertas**: Fondo amarillo para advertencias

**Tecnología**:
- `QProgressBar` con stylesheet personalizado
- `QTextEdit` para logs (fuente monospace 12px)
- `QTimer` para actualización de estadísticas
- Procesamiento en thread secundario (simulado)

---

### Fase 4: Visor OLAP 3D

**Objetivo**: Explorar el cubo OLAP mediante visualización isométrica interactiva.

**Proceso**:
1. `VisorOlap` renderiza cubo 3D con `QPainter`
2. Usuario selecciona celdas haciendo click
3. `PanelAnalisis` muestra estadísticas de la celda
4. Usuario aplica operaciones OLAP (ver sección siguiente)

**Visualización 3D**:

El cubo se renderiza usando **proyección isométrica**:

```cpp
// Transformación isométrica
x_screen = (x - z) * cos(30°)
y_screen = y + (x + z) * sin(30°)
```

**Características del Cubo**:
- **Celdas coloreadas**: Gradiente según valor de medida
- **Ejes etiquetados**: Dimensiones en X, Y, Z
- **Selección interactiva**: Click para seleccionar celda
- **Tooltips**: Información al pasar el mouse
- **Escala de valores**: Leyenda con gradiente de colores

**Panel de Análisis** (Derecha):

| Sección                | Contenido                                    |
| ---------------------- | -------------------------------------------- |
| **Celda Seleccionada** | Dimensiones y valor de la celda              |
| **Estadísticas**       | Suma, promedio, máximo, mínimo               |
| **Acciones OLAP**      | 8 botones con emojis (ver siguiente sección) |

**Tecnología**:
- `QPainter` para renderizado 2.5D
- `QPolygon` para dibujar cubos
- `QLinearGradient` para colores
- `QMouseEvent` para interacción

---

### Fase 5: Constructor de Consultas

**Objetivo**: Permitir al usuario construir consultas SQL personalizadas de forma visual.

**Proceso**:
1. Sistema carga dinámicamente dimensiones y medidas desde BD
2. Usuario arrastra dimensiones a **Filas** o **Columnas**
3. Usuario arrastra medida a **Medidas**
4. Usuario selecciona función de agregación (SUM, AVG, COUNT, MAX, MIN)
5. (Opcional) Usuario agrega filtros SQL manuales
6. Sistema genera SQL dinámicamente
7. Ejecuta query y muestra resultados en tabla

**Detección Dinámica**:

```cpp
// Dimensiones: Detecta tablas dim_*
SELECT table_name, column_name 
FROM information_schema.tables t
JOIN information_schema.columns c USING (table_name)
WHERE table_name LIKE 'dim_%'

// Medidas: Detecta columnas numéricas de fact_*
SELECT column_name 
FROM information_schema.columns
WHERE table_name = 'fact_ventas'
  AND data_type IN ('integer', 'numeric', 'double precision')
```

**Generación de SQL**:

El sistema construye queries dinámicamente:

```sql
SELECT 
  dim_cliente.tipo_cliente,
  dim_producto.categoria,
  SUM(fact_ventas.total_venta) as resultado
FROM fact_ventas f
JOIN dim_cliente c ON f.id_cliente = c.id_cliente
JOIN dim_producto p ON f.id_producto = p.id_producto
WHERE total_venta > 1000
GROUP BY c.tipo_cliente, p.categoria
LIMIT 500
```

**Visualización**:
- **Recursos Disponibles**: Listas de dimensiones y medidas
- **Definición de Consulta**: Áreas de drop para filas, columnas, medidas, filtros
- **Resultados**: Tabla con datos y estadísticas
- **Historial**: Consultas ejecutadas previamente

**Exportación**:
- **📊 CSV**: Exportar resultados a Excel
- **📄 PDF**: Generar reporte imprimible

**Tecnología**:
- `QListWidget` con drag & drop habilitado
- `QComboBox` para agregaciones
- `QTableWidget` para resultados
- `QSqlQuery` para ejecución
- Generación dinámica de SQL con `QString::arg()`

---

## Operaciones OLAP

### 1. Drill Down (Profundizar)

**Descripción**: Navegar de un nivel agregado a un nivel más detallado en una jerarquía.

**Ejemplo**:
```
Año 2024 → Trimestre Q1 → Mes Enero → Día 15
```

**Implementación**:
- Usuario selecciona celda agregada
- Sistema identifica jerarquía de la dimensión
- Expande al siguiente nivel más detallado
- Recalcula agregaciones

**Estado**: ✅ **Implementado** (con jerarquías predefinidas)

---

### 2. Roll Up (Resumir)

**Descripción**: Navegar de un nivel detallado a un nivel más agregado.

**Ejemplo**:
```
Día 15 → Mes Enero → Trimestre Q1 → Año 2024
```

**Implementación**:
- Usuario hace click en "Roll Up"
- Sistema sube un nivel en la jerarquía
- Reagrupa datos al nivel superior

**Estado**: ✅ **Implementado**

---

### 3. Slice (Cortar)

**Descripción**: Filtrar el cubo por un valor específico de una dimensión.

**Ejemplo**:
```
Filtrar solo: Año = 2024
Resultado: Cubo 2D (Cliente × Producto)
```

**Implementación**:
- Usuario selecciona dimensión y valor
- Sistema aplica filtro WHERE en SQL
- Reduce dimensionalidad del cubo

**Estado**: ✅ **Implementado** (con `DialogoFiltros`)

---

### 4. Dice (Dados)

**Descripción**: Filtrar el cubo por múltiples valores en múltiples dimensiones.

**Ejemplo**:
```
Filtrar: Año IN (2023, 2024) AND País IN ('Argentina', 'Brasil')
Resultado: Sub-cubo con datos filtrados
```

**Implementación**:
- Usuario selecciona múltiples dimensiones
- Para cada dimensión, selecciona múltiples valores
- Sistema combina filtros con AND

**Estado**: ✅ **Implementado** (con `DialogoFiltros` en modo múltiple)

---

### 5. Pivot (Rotar)

**Descripción**: Rotar los ejes del cubo para cambiar la perspectiva de análisis.

**Ejemplo**:
```
Antes: X=Cliente, Y=Producto, Z=Tiempo
Después: X=Tiempo, Y=Cliente, Z=Producto
```

**Implementación**:
- Usuario hace click en "Pivot"
- Sistema rota dimensiones en sentido horario
- Re-renderiza cubo con nueva orientación

**Estado**: ✅ **Implementado**

---

### 6. Swap (Intercambiar)

**Descripción**: Intercambiar dos dimensiones específicas del cubo.

**Ejemplo**:
```
Intercambiar eje X con eje Y
```

**Implementación**:
- Usuario selecciona dos dimensiones
- Sistema intercambia sus posiciones
- Actualiza visualización

**Estado**: ✅ **Implementado**

---

### 7. Drill Through (Ver Detalles)

**Descripción**: Ver los registros individuales que componen una celda agregada.

**Ejemplo**:
```
Celda: Cliente=C1, Producto=P1, Tiempo=2024 → Total=1000
Drill Through → Muestra las 50 transacciones que suman 1000
```

**Implementación**:
- Usuario selecciona celda
- Sistema ejecuta query sin GROUP BY
- Muestra registros en `DialogoDrillThrough`

**Estado**: ✅ **Implementado**

---

### 8. Ranking (Top N)

**Descripción**: Mostrar solo las N celdas con mayores/menores valores.

**Ejemplo**:
```
Top 10 productos por ventas
```

**Implementación**:
- Usuario ingresa N y orden (DESC/ASC)
- Sistema aplica ORDER BY y LIMIT
- Filtra cubo a top N

**Estado**: ✅ **Implementado**

---

## Componentes Simulados vs Reales

### Componentes Reales (Funcionales)

| Componente               | Descripción                                                           |
| ------------------------ | --------------------------------------------------------------------- |
| **Conexión PostgreSQL**  | Conexión real con validación de credenciales                          |
| **Análisis de Esquema**  | Queries reales a `information_schema`                                 |
| **Detección de Tablas**  | Identificación automática de fact/dim                                 |
| **Generación de SQL**    | Construcción dinámica de queries                                      |
| **Ejecución de Queries** | Queries reales con `QSqlQuery`                                        |
| **Visualización 3D**     | Renderizado isométrico con `QPainter`                                 |
| **Operaciones OLAP**     | Drill Down, Roll Up, Slice, Dice, Pivot, Swap, Drill Through, Ranking |
| **Exportación**          | CSV funcional, PDF con estructura                                     |
| **Drag & Drop**          | Importación de archivos SQL                                           |
| **Filtros Dinámicos**    | Detección de valores únicos desde BD                                  |

### Componentes Simulados (Educativos)

| Componente                  | Descripción                          | Motivo                               |
| --------------------------- | ------------------------------------ | ------------------------------------ |
| **Carga de Datos (Fase 3)** | Progreso simulado con delays         | Fines educativos, muestra proceso    |
| **Validación de Calidad**   | Calidad fija al 100%                 | Simplificación didáctica             |
| **Índices de BD**           | No se crean índices reales           | Permisos de BD limitados             |
| **Caché de Cubo**           | Cubo se recalcula en cada operación  | Optimización futura                  |
| **Exportación PDF**         | Genera estructura pero sin contenido | Requiere librería externa (QPrinter) |

### Componentes Parcialmente Simulados

| Componente       | Real                             | Simulado                         |
| ---------------- | -------------------------------- | -------------------------------- |
| **Jerarquías**   | Estructura definida              | Datos de jerarquía hardcodeados  |
| **Agregaciones** | Cálculos reales (SUM, AVG, etc.) | Jerarquías predefinidas          |
| **Rendimiento**  | Estimación matemática            | No mide tiempo real de ejecución |

---

## Ventajas y Desventajas

### Ventajas

#### 1. **Educativo y Didáctico**
- Flujo guiado en 5 fases fácil de entender
- Visualización clara del proceso OLAP
- Ideal para aprender conceptos de BI

#### 2. **Interfaz Moderna**
- Diseño limpio con tema blanco y azul
- Emojis en botones para mejor UX
- Gráficos interactivos con QtCharts
- Visualización 3D isométrica atractiva

#### 3. **Detección Automática**
- Identifica automáticamente fact/dim
- Detecta relaciones por foreign keys
- Carga dinámica de dimensiones y medidas
- No requiere configuración manual

#### 4. **Operaciones OLAP Completas**
- 8 operaciones OLAP implementadas
- Filtros dinámicos (Slice/Dice)
- Navegación jerárquica (Drill Down/Up)
- Transformaciones (Pivot/Swap)

#### 5. **Multiplataforma**
- Funciona en Windows, Linux, macOS
- Compilación con CMake
- Dependencias estándar (Qt + PostgreSQL)

#### 6. **Código Limpio**
- Arquitectura MVC clara
- Separación de responsabilidades
- Comentarios en español
- Fácil de extender

### Desventajas

#### 1. **Rendimiento Limitado**
- No optimizado para grandes volúmenes (>1M registros)
- Cubo se recalcula en cada operación
- Sin caché de agregaciones
- Renderizado 2.5D puede ser lento con muchas celdas

#### 2. **Funcionalidad Limitada**
- Jerarquías hardcodeadas (no dinámicas)
- Exportación PDF incompleta
- Sin soporte para cubos dispersos (sparse)
- No soporta medidas calculadas complejas

#### 3. **Dependencias Externas**
- Requiere Qt 6.5+ (pesado, ~2GB)
- Requiere PostgreSQL instalado
- No funciona con otras BD (MySQL, SQLite)

#### 4. **Escalabilidad**
- Cubo en memoria (limitado por RAM)
- Sin particionamiento de datos
- Sin procesamiento distribuido
- Límite de 500 filas en consultas (seguridad)

#### 5. **Simulaciones**
- Fase 3 (Carga) es simulada
- Calidad de datos fija al 100%
- No crea índices reales en BD

#### 6. **Curva de Aprendizaje**
- Requiere conocimientos de SQL
- Requiere entender esquema estrella
- Configuración inicial puede ser compleja

---

## Stack Tecnológico

### Lenguajes y Frameworks

| Tecnología | Versión        | Uso                   |
| ---------- | -------------- | --------------------- |
| **C++**    | C++20          | Lenguaje principal    |
| **Qt**     | 6.5+           | Framework UI y lógica |
| **SQL**    | PostgreSQL 14+ | Base de datos         |
| **CMake**  | 3.16+          | Build system          |

### Librerías Qt Utilizadas

| Módulo         | Uso                                             |
| -------------- | ----------------------------------------------- |
| `Qt6::Widgets` | Interfaz gráfica (QWidget, QPushButton, etc.)   |
| `Qt6::Sql`     | Conexión a PostgreSQL (QSqlDatabase, QSqlQuery) |
| `Qt6::Charts`  | Gráficos (QPieSeries, QBarSeries)               |
| `Qt6::Gui`     | Renderizado (QPainter, QPixmap)                 |
| `Qt6::Core`    | Utilidades (QString, QVector, QTimer)           |

### Herramientas de Desarrollo

| Herramienta    | Propósito             |
| -------------- | --------------------- |
| **Qt Creator** | IDE recomendado       |
| **MinGW 13**   | Compilador Windows    |
| **GCC 11+**    | Compilador Linux      |
| **pgAdmin 4**  | Gestión de PostgreSQL |
| **Git**        | Control de versiones  |

---

## Estructura de Datos

### Esquema Estrella

```
        dim_tiempo
             │
             │
        dim_cliente ─── fact_ventas ─── dim_producto
                            │
                            │
                       dim_geografia
```

### Tablas de Dimensiones

| Tabla           | Columnas Clave        | Jerarquía            |
| --------------- | --------------------- | -------------------- |
| `dim_tiempo`    | fecha, anio, mes, dia | Año → Mes → Día      |
| `dim_geografia` | pais, ciudad          | País → Ciudad        |
| `dim_producto`  | categoria, nombre     | Categoría → Producto |
| `dim_cliente`   | tipo_cliente, nombre  | Tipo → Cliente       |

### Tabla de Hechos

| Tabla         | Medidas                         | Foreign Keys                                     |
| ------------- | ------------------------------- | ------------------------------------------------ |
| `fact_ventas` | total_venta, cantidad, ganancia | id_tiempo, id_geografia, id_producto, id_cliente |

---

## Conclusión

**Cubo Vision** es un sistema OLAP educativo completo que demuestra los conceptos fundamentales de análisis multidimensional de forma visual e interactiva. Aunque tiene limitaciones de rendimiento y escalabilidad, cumple perfectamente su objetivo didáctico de enseñar OLAP, visualización de datos y arquitectura de BI.

### Ideal Para:
- ✅ Aprendizaje de conceptos OLAP
- ✅ Demostraciones académicas
- ✅ Prototipos de BI
- ✅ Análisis exploratorio de datos

### No Recomendado Para:
- ❌ Producción empresarial
- ❌ Big Data (>1M registros)
- ❌ Análisis en tiempo real
- ❌ Dashboards públicos

---

**Desarrollado para**: Bases de Datos  
**Universidad**: Universidad Nacional de Misiones (UNAM)  
**Facultad**: Ciencias Exactas, Químicas y Naturales  
**Módulo**: Apóstoles  
**Año**: 2025
