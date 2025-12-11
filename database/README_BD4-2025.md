# Guía de Uso: Base de Datos bd4-2025

## 📊 Estructura del Cubo OLAP Perfecto

Esta base de datos está diseñada para formar un **cubo OLAP perfecto de 10×10×10** que permite visualizar y ejecutar todas las operaciones OLAP de manera óptima.

![Referencia OLAP](file:///C:/Users/laza_/.gemini/antigravity/brain/82bc197f-19d8-436d-a8bf-836981ee5212/uploaded_image_1765462411692.png)

## 🎯 Características Principales

### Dimensiones del Cubo

| Dimensión     | Tabla           | Valores | Eje Visual | Descripción                                |
| ------------- | --------------- | ------- | ---------- | ------------------------------------------ |
| **Producto**  | `dim_producto`  | 10      | **Eje X**  | Laptops, monitores, accesorios, etc.       |
| **Tiempo**    | `dim_tiempo`    | 10      | **Eje Z**  | 10 meses de 2024 (Enero-Octubre)           |
| **Cliente**   | `dim_cliente`   | 10      | Variedad   | Segmentos: Corporativo, PYME, Retail, etc. |
| **Geografía** | `dim_geografia` | 10      | Drill-down | Regiones de Argentina                      |

### Tabla de Hechos

- **Tabla**: `fact_ventas`
- **Registros**: **1000+** (10 productos × 10 meses × 10 clientes + extras)
- **Medidas**:
  - `total_venta` - Ingresos totales
  - `ganancia` - Beneficio neto
  - `cantidad` - Unidades vendidas
  - `descuento` - Porcentaje de descuento aplicado

## 🚀 Instalación

### Paso 1: Crear la Base de Datos

```bash
# Conectar a PostgreSQL
psql -U postgresql -d postgres

# Crear la base de datos
CREATE DATABASE "bd4-2025";

# Salir
\q
```

### Paso 2: Ejecutar el Script de Semilla

```bash
# Ejecutar el script completo
psql -U postgresql -d bd4-2025 -f database/seed_bd4-2025.sql
```

**Credenciales por defecto:**
- Usuario: `postgresql`
- Password: `laza`
- Database: `bd4-2025`

### Paso 3: Verificar la Instalación

```sql
-- Conectar a la base de datos
\c bd4-2025

-- Verificar tablas creadas
\dt

-- Ver estadísticas del cubo
SELECT 
    'Total registros' as metrica,
    COUNT(*)::TEXT as valor
FROM fact_ventas;

-- Ver distribución dimensional
SELECT 
    COUNT(DISTINCT id_producto) as productos,
    COUNT(DISTINCT id_tiempo) as meses,
    COUNT(DISTINCT id_cliente) as clientes
FROM fact_ventas;
```

## 📈 Estructura de Datos

### Dimensión: Producto (Eje X)

```sql
SELECT * FROM dim_producto ORDER BY id_producto;
```

| ID  | Código   | Nombre            | Categoría   | Precio |
| --- | -------- | ----------------- | ----------- | ------ |
| 1   | PROD-001 | Laptop Pro 15"    | Electrónica | $1,200 |
| 2   | PROD-002 | Mouse Inalámbrico | Electrónica | $25    |
| 3   | PROD-003 | Teclado Mecánico  | Electrónica | $80    |
| ... | ...      | ...               | ...         | ...    |
| 10  | PROD-010 | Tablet 10"        | Electrónica | $400   |

### Dimensión: Tiempo (Eje Z)

```sql
SELECT * FROM dim_tiempo ORDER BY id_tiempo;
```

| ID  | Año  | Trimestre | Mes | Nombre  |
| --- | ---- | --------- | --- | ------- |
| 1   | 2024 | Q1        | 1   | Enero   |
| 2   | 2024 | Q1        | 2   | Febrero |
| 3   | 2024 | Q1        | 3   | Marzo   |
| ... | ...  | ...       | ... | ...     |
| 10  | 2024 | Q4        | 10  | Octubre |

### Dimensión: Cliente

```sql
SELECT * FROM dim_cliente ORDER BY id_cliente;
```

| ID  | Código  | Nombre            | Segmento    | Ciudad       |
| --- | ------- | ----------------- | ----------- | ------------ |
| 1   | CLI-001 | TechCorp SA       | Corporativo | Buenos Aires |
| 2   | CLI-002 | Innovatech SRL    | Corporativo | Buenos Aires |
| 3   | CLI-003 | Digital Solutions | PYME        | Córdoba      |
| ... | ...     | ...               | ...         | ...          |

## 🎲 Operaciones OLAP Soportadas

### 1. **Slice** - Cortar una dimensión

Filtrar solo un valor de una dimensión:

```sql
-- Ventas solo de Enero 2024
SELECT 
    p.nombre_producto,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
WHERE t.nombre_mes = 'Enero'
GROUP BY p.nombre_producto
ORDER BY total DESC;
```

### 2. **Dice** - Filtros múltiples

Aplicar filtros en varias dimensiones:

```sql
-- Ventas de Electrónica en Q1 para clientes Corporativos
SELECT 
    p.nombre_producto,
    t.nombre_mes,
    c.nombre_cliente,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
JOIN dim_cliente c ON v.id_cliente = c.id_cliente
WHERE p.categoria = 'Electrónica'
  AND t.trimestre = 1
  AND c.segmento = 'Corporativo'
GROUP BY p.nombre_producto, t.nombre_mes, c.nombre_cliente
ORDER BY total DESC;
```

### 3. **Drill Down** - Profundizar en jerarquías

De categoría → subcategoría → producto:

```sql
SELECT 
    p.categoria,
    p.subcategoria,
    p.nombre_producto,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY ROLLUP(p.categoria, p.subcategoria, p.nombre_producto)
ORDER BY p.categoria, p.subcategoria, p.nombre_producto;
```

### 4. **Roll Up** - Subir en jerarquías

De mes → trimestre → año:

```sql
SELECT 
    t.anio,
    t.trimestre,
    t.nombre_mes,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY ROLLUP(t.anio, t.trimestre, t.nombre_mes)
ORDER BY t.anio, t.trimestre, t.mes;
```

### 5. **Pivot** - Rotar ejes

Intercambiar Producto ↔ Tiempo:

```sql
-- Vista con Productos en filas, Meses en columnas
SELECT 
    p.nombre_producto,
    SUM(CASE WHEN t.mes = 1 THEN v.total_venta ELSE 0 END) as Enero,
    SUM(CASE WHEN t.mes = 2 THEN v.total_venta ELSE 0 END) as Febrero,
    SUM(CASE WHEN t.mes = 3 THEN v.total_venta ELSE 0 END) as Marzo,
    SUM(CASE WHEN t.mes = 4 THEN v.total_venta ELSE 0 END) as Abril,
    SUM(CASE WHEN t.mes = 5 THEN v.total_venta ELSE 0 END) as Mayo,
    SUM(CASE WHEN t.mes = 6 THEN v.total_venta ELSE 0 END) as Junio,
    SUM(CASE WHEN t.mes = 7 THEN v.total_venta ELSE 0 END) as Julio,
    SUM(CASE WHEN t.mes = 8 THEN v.total_venta ELSE 0 END) as Agosto,
    SUM(CASE WHEN t.mes = 9 THEN v.total_venta ELSE 0 END) as Septiembre,
    SUM(CASE WHEN t.mes = 10 THEN v.total_venta ELSE 0 END) as Octubre
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY p.nombre_producto
ORDER BY p.nombre_producto;
```

### 6. **Ranking** - Top N

Top 10 productos por ventas:

```sql
SELECT 
    p.nombre_producto,
    SUM(v.total_venta) as total_ventas,
    RANK() OVER (ORDER BY SUM(v.total_venta) DESC) as ranking
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY p.nombre_producto
ORDER BY ranking
LIMIT 10;
```

### 7. **Drill Through** - Ver registros detallados

Ver transacciones individuales de una celda:

```sql
-- Todas las ventas de Laptop Pro en Enero
SELECT 
    v.*,
    p.nombre_producto,
    t.nombre_mes,
    c.nombre_cliente,
    g.ciudad
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
JOIN dim_cliente c ON v.id_cliente = c.id_cliente
JOIN dim_geografia g ON v.id_geografia = g.id_geografia
WHERE p.codigo_producto = 'PROD-001'
  AND t.nombre_mes = 'Enero'
ORDER BY v.fecha_venta;
```

## 🎨 Patrones de Datos para Visualización

El cubo está diseñado con **patrones visuales intencionales**:

### Productos Populares (Picos altos)
- **Laptop Pro** (ID 1) - Ventas muy altas
- **Monitor 27"** (ID 4) - Ventas altas
- **Impresora** (ID 9) - Ventas altas
- **Tablet** (ID 10) - Ventas altas

### Meses Clave (Temporadas altas)
- **Marzo** (Q1) - Fin de trimestre
- **Junio** (Q2) - Fin de trimestre
- **Septiembre** (Q3) - Fin de trimestre
- **Octubre** (Q4) - Preparación para fin de año

### Clientes VIP (Mayor volumen)
- **TechCorp SA** (ID 1) - Corporativo
- **MegaStore Retail** (ID 5) - Retail
- **Enterprise Global** (ID 10) - Corporativo

## 📊 Vistas Materializadas

El script crea vistas precalculadas para consultas rápidas:

### Vista: Ventas por Producto y Mes

```sql
SELECT * FROM mv_ventas_producto_mes
ORDER BY venta_total DESC
LIMIT 10;
```

### Vista: Ventas por Cliente y Región

```sql
SELECT * FROM mv_ventas_cliente_region
ORDER BY venta_total DESC
LIMIT 10;
```

**Refrescar vistas:**

```sql
REFRESH MATERIALIZED VIEW mv_ventas_producto_mes;
REFRESH MATERIALIZED VIEW mv_ventas_cliente_region;
```

## 🔍 Consultas Útiles

### Resumen General del Cubo

```sql
SELECT 
    COUNT(*) as total_registros,
    COUNT(DISTINCT id_producto) as productos_distintos,
    COUNT(DISTINCT id_tiempo) as meses_distintos,
    COUNT(DISTINCT id_cliente) as clientes_distintos,
    SUM(total_venta) as ventas_totales,
    SUM(ganancia) as ganancia_total,
    AVG(total_venta) as venta_promedio
FROM fact_ventas;
```

### Top 5 Productos por Ganancia

```sql
SELECT 
    p.nombre_producto,
    SUM(v.ganancia) as ganancia_total,
    SUM(v.cantidad) as unidades_vendidas
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY p.nombre_producto
ORDER BY ganancia_total DESC
LIMIT 5;
```

### Evolución Mensual de Ventas

```sql
SELECT 
    t.nombre_mes,
    t.trimestre,
    SUM(v.total_venta) as venta_mes,
    SUM(SUM(v.total_venta)) OVER (ORDER BY t.mes) as venta_acumulada
FROM fact_ventas v
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY t.mes, t.nombre_mes, t.trimestre
ORDER BY t.mes;
```

### Análisis por Segmento de Cliente

```sql
SELECT 
    c.segmento,
    COUNT(DISTINCT v.id_cliente) as num_clientes,
    SUM(v.total_venta) as venta_total,
    AVG(v.total_venta) as venta_promedio,
    SUM(v.ganancia) as ganancia_total
FROM fact_ventas v
JOIN dim_cliente c ON v.id_cliente = c.id_cliente
GROUP BY c.segmento
ORDER BY venta_total DESC;
```

## 🎯 Uso en Cubo Vision

### Configuración de Conexión

En la aplicación Cubo Vision:

1. **Host**: `localhost`
2. **Puerto**: `5432`
3. **Usuario**: `postgresql`
4. **Password**: `laza`
5. **Base de datos**: `bd4-2025`

### Visualización Esperada

El cubo 3D mostrará:

- **Eje X (horizontal derecho)**: 10 productos
- **Eje Z (horizontal izquierdo)**: 10 meses
- **Eje Y (vertical)**: Valores de ventas (altura de prismas)

**Resultado**: Un cubo denso de **10×10 = 100 celdas visibles** con alturas variables según las ventas.

### Operaciones Recomendadas para Probar

1. **Drill Down**: Hacer clic en "Laptop Pro" para ver solo sus ventas
2. **Slice**: Filtrar solo "Enero" para ver ventas del primer mes
3. **Dice**: Filtrar "Corporativo" + "Q1" para análisis específico
4. **Pivot**: Rotar para ver Tiempo en X y Producto en Z
5. **Ranking**: Mostrar solo Top 5 productos
6. **Drill Through**: Ver transacciones individuales de una celda

## 📝 Mantenimiento

### Agregar Más Datos

```sql
-- Agregar un nuevo producto
INSERT INTO dim_producto (codigo_producto, nombre_producto, categoria, subcategoria, marca, precio_unitario)
VALUES ('PROD-011', 'Smartphone Pro', 'Electrónica', 'Móviles', 'TechMaster', 800.00);

-- Agregar ventas para el nuevo producto
INSERT INTO fact_ventas (id_producto, id_tiempo, id_cliente, id_geografia, cantidad, precio_venta, descuento, costo_producto, total_venta, ganancia, fecha_venta)
SELECT 
    11, -- Nuevo producto
    t.id_tiempo,
    c.id_cliente,
    ((c.id_cliente - 1) % 10) + 1,
    15,
    800.00,
    5.0,
    480.00,
    11400.00,
    3900.00,
    make_date(2024, t.mes, 10)
FROM dim_tiempo t
CROSS JOIN dim_cliente c
WHERE t.mes <= 10;
```

### Limpiar Datos

```sql
-- Eliminar todas las ventas
TRUNCATE TABLE fact_ventas CASCADE;

-- Re-ejecutar el script de semilla
\i database/seed_bd4-2025.sql
```

## ✅ Verificación Final

Ejecutar para confirmar que todo está correcto:

```sql
-- Debe retornar 1000+
SELECT COUNT(*) FROM fact_ventas;

-- Debe retornar 10
SELECT COUNT(*) FROM dim_producto;

-- Debe retornar 10
SELECT COUNT(*) FROM dim_tiempo;

-- Debe retornar 10
SELECT COUNT(*) FROM dim_cliente;

-- Debe retornar 10
SELECT COUNT(*) FROM dim_geografia;

-- Debe retornar combinaciones únicas
SELECT 
    COUNT(DISTINCT CONCAT(id_producto, '-', id_tiempo)) as prod_tiempo,
    COUNT(DISTINCT CONCAT(id_producto, '-', id_cliente)) as prod_cliente,
    COUNT(DISTINCT CONCAT(id_tiempo, '-', id_cliente)) as tiempo_cliente
FROM fact_ventas;
```

## 🎉 ¡Listo!

Tu base de datos **bd4-2025** está lista para ser usada con **Cubo Vision**. El cubo OLAP perfecto de 10×10×10 te permitirá visualizar y ejecutar todas las operaciones OLAP de manera óptima.

**Próximos pasos:**
1. Ejecutar Cubo Vision
2. Conectar a bd4-2025
3. Completar Fases 1-3
4. Explorar el cubo en Fase 4
5. Probar todas las operaciones OLAP
