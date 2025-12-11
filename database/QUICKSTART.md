# 🚀 Quick Start: bd4-2025

## Instalación Rápida

```bash
# 1. Crear base de datos
psql -U postgresql -d postgres -c "CREATE DATABASE \"bd4-2025\";"

# 2. Ejecutar semilla
psql -U postgresql -d bd4-2025 -f database/seed_bd4-2025.sql

# 3. Verificar
psql -U postgresql -d bd4-2025 -c "SELECT COUNT(*) FROM fact_ventas;"
```

## 📊 Estructura del Cubo

```
        10 Productos (Eje X)
            ↓
    ┌───────────────┐
    │   CUBO 3D     │  ← 10 Meses (Eje Z)
    │   10×10×10    │
    └───────────────┘
            ↑
    Altura = Ventas (Eje Y)
```

## 🎯 Datos Clave

- **Productos**: 10 (Laptop, Mouse, Teclado, Monitor, etc.)
- **Tiempo**: 10 meses (Enero-Octubre 2024)
- **Clientes**: 10 (Corporativo, PYME, Retail, etc.)
- **Registros**: 1000+ ventas
- **Medida principal**: `total_venta`

## 🔌 Conexión en Cubo Vision

| Campo    | Valor        |
| -------- | ------------ |
| Host     | `localhost`  |
| Puerto   | `5432`       |
| Usuario  | `postgresql` |
| Password | `laza`       |
| Database | `bd4-2025`   |

## 🎲 Operaciones OLAP

| Operación         | Descripción         | Ejemplo                    |
| ----------------- | ------------------- | -------------------------- |
| **Slice**         | Filtrar 1 dimensión | Solo Enero                 |
| **Dice**          | Filtros múltiples   | Q1 + Corporativo           |
| **Drill Down**    | Profundizar         | Categoría → Producto       |
| **Roll Up**       | Subir nivel         | Mes → Trimestre            |
| **Pivot**         | Rotar ejes          | X ↔ Z                      |
| **Ranking**       | Top N               | Top 10 productos           |
| **Drill Through** | Ver registros       | Transacciones individuales |

## 📈 Consultas Rápidas

### Total de Ventas
```sql
SELECT SUM(total_venta) FROM fact_ventas;
```

### Top 5 Productos
```sql
SELECT p.nombre_producto, SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY p.nombre_producto
ORDER BY total DESC LIMIT 5;
```

### Ventas por Mes
```sql
SELECT t.nombre_mes, SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY t.nombre_mes, t.mes
ORDER BY t.mes;
```

## ✅ Verificación

```sql
-- Debe retornar 1000+
SELECT COUNT(*) FROM fact_ventas;

-- Debe retornar 10, 10, 10
SELECT 
    COUNT(DISTINCT id_producto) as productos,
    COUNT(DISTINCT id_tiempo) as meses,
    COUNT(DISTINCT id_cliente) as clientes
FROM fact_ventas;
```

## 🎨 Patrones Visuales

El cubo tiene **picos intencionales** para visualización:

- **Productos populares**: Laptop (1), Monitor (4), Tablet (10)
- **Meses clave**: Marzo (3), Junio (6), Septiembre (9), Octubre (10)
- **Clientes VIP**: TechCorp (1), MegaStore (5), Enterprise (10)

Esto crea un cubo **visualmente interesante** con variación de alturas.

## 📚 Documentación Completa

Ver [`README_BD4-2025.md`](file:///c:/Proyectos%20Facu/OLAPBD2025/database/README_BD4-2025.md) para:
- Instrucciones detalladas
- Ejemplos de todas las operaciones OLAP
- Consultas avanzadas
- Mantenimiento y troubleshooting
