-- ============================================================================
-- SEMILLA DE BASE DE DATOS PARA CUBO OLAP PERFECTO
-- Base de datos: bd4-2025
-- Estructura: Cubo 10x10x10 con datos balanceados para visualización óptima
-- ============================================================================

-- Conectar a PostgreSQL y crear la base de datos
-- Ejecutar: psql -U postgresql -d postgres
-- CREATE DATABASE "bd4-2025";
-- \c bd4-2025

-- ============================================================================
-- DIMENSIÓN: TIEMPO (10 valores - Eje Z)
-- ============================================================================
DROP TABLE IF EXISTS dim_tiempo CASCADE;
CREATE TABLE dim_tiempo (
    id_tiempo SERIAL PRIMARY KEY,
    anio INTEGER NOT NULL,
    trimestre INTEGER NOT NULL,
    mes INTEGER NOT NULL,
    nombre_mes VARCHAR(20) NOT NULL,
    nombre_trimestre VARCHAR(10) NOT NULL
);

INSERT INTO dim_tiempo (anio, trimestre, mes, nombre_mes, nombre_trimestre) VALUES
(2024, 1, 1, 'Enero', 'Q1-2024'),
(2024, 1, 2, 'Febrero', 'Q1-2024'),
(2024, 1, 3, 'Marzo', 'Q1-2024'),
(2024, 2, 4, 'Abril', 'Q2-2024'),
(2024, 2, 5, 'Mayo', 'Q2-2024'),
(2024, 2, 6, 'Junio', 'Q2-2024'),
(2024, 3, 7, 'Julio', 'Q3-2024'),
(2024, 3, 8, 'Agosto', 'Q3-2024'),
(2024, 3, 9, 'Septiembre', 'Q3-2024'),
(2024, 4, 10, 'Octubre', 'Q4-2024');

-- ============================================================================
-- DIMENSIÓN: PRODUCTO (10 valores - Eje X)
-- ============================================================================
DROP TABLE IF EXISTS dim_producto CASCADE;
CREATE TABLE dim_producto (
    id_producto SERIAL PRIMARY KEY,
    codigo_producto VARCHAR(10) NOT NULL UNIQUE,
    nombre_producto VARCHAR(100) NOT NULL,
    categoria VARCHAR(50) NOT NULL,
    subcategoria VARCHAR(50) NOT NULL,
    marca VARCHAR(50) NOT NULL,
    precio_unitario DECIMAL(10,2) NOT NULL
);

INSERT INTO dim_producto (codigo_producto, nombre_producto, categoria, subcategoria, marca, precio_unitario) VALUES
('PROD-001', 'Laptop Pro 15"', 'Electrónica', 'Computadoras', 'TechMaster', 1200.00),
('PROD-002', 'Mouse Inalámbrico', 'Electrónica', 'Accesorios', 'TechMaster', 25.00),
('PROD-003', 'Teclado Mecánico', 'Electrónica', 'Accesorios', 'GamePro', 80.00),
('PROD-004', 'Monitor 27" 4K', 'Electrónica', 'Pantallas', 'ViewMax', 450.00),
('PROD-005', 'Auriculares Bluetooth', 'Electrónica', 'Audio', 'SoundWave', 120.00),
('PROD-006', 'Webcam HD', 'Electrónica', 'Video', 'ViewMax', 90.00),
('PROD-007', 'Disco SSD 1TB', 'Electrónica', 'Almacenamiento', 'DataFast', 150.00),
('PROD-008', 'Router WiFi 6', 'Electrónica', 'Redes', 'NetSpeed', 180.00),
('PROD-009', 'Impresora Láser', 'Electrónica', 'Impresión', 'PrintPro', 300.00),
('PROD-010', 'Tablet 10"', 'Electrónica', 'Tablets', 'TechMaster', 400.00);

-- ============================================================================
-- DIMENSIÓN: CLIENTE (10 valores - Eje Y conceptual, pero usaremos para variedad)
-- ============================================================================
DROP TABLE IF EXISTS dim_cliente CASCADE;
CREATE TABLE dim_cliente (
    id_cliente SERIAL PRIMARY KEY,
    codigo_cliente VARCHAR(10) NOT NULL UNIQUE,
    nombre_cliente VARCHAR(100) NOT NULL,
    segmento VARCHAR(50) NOT NULL,
    pais VARCHAR(50) NOT NULL,
    region VARCHAR(50) NOT NULL,
    ciudad VARCHAR(50) NOT NULL
);

INSERT INTO dim_cliente (codigo_cliente, nombre_cliente, segmento, pais, region, ciudad) VALUES
('CLI-001', 'TechCorp SA', 'Corporativo', 'Argentina', 'CABA', 'Buenos Aires'),
('CLI-002', 'Innovatech SRL', 'Corporativo', 'Argentina', 'CABA', 'Buenos Aires'),
('CLI-003', 'Digital Solutions', 'PYME', 'Argentina', 'Córdoba', 'Córdoba'),
('CLI-004', 'SmartBiz Inc', 'PYME', 'Argentina', 'Santa Fe', 'Rosario'),
('CLI-005', 'MegaStore Retail', 'Retail', 'Argentina', 'Mendoza', 'Mendoza'),
('CLI-006', 'EduTech Academy', 'Educación', 'Argentina', 'CABA', 'Buenos Aires'),
('CLI-007', 'HealthPlus Clinic', 'Salud', 'Argentina', 'Córdoba', 'Córdoba'),
('CLI-008', 'GovTech Bureau', 'Gobierno', 'Argentina', 'CABA', 'Buenos Aires'),
('CLI-009', 'StartupHub', 'Startup', 'Argentina', 'CABA', 'Buenos Aires'),
('CLI-010', 'Enterprise Global', 'Corporativo', 'Argentina', 'Santa Fe', 'Rosario');

-- ============================================================================
-- DIMENSIÓN: GEOGRAFÍA (10 valores adicionales para drill-down)
-- ============================================================================
DROP TABLE IF EXISTS dim_geografia CASCADE;
CREATE TABLE dim_geografia (
    id_geografia SERIAL PRIMARY KEY,
    pais VARCHAR(50) NOT NULL,
    region VARCHAR(50) NOT NULL,
    provincia VARCHAR(50) NOT NULL,
    ciudad VARCHAR(50) NOT NULL,
    codigo_postal VARCHAR(10) NOT NULL
);

INSERT INTO dim_geografia (pais, region, provincia, ciudad, codigo_postal) VALUES
('Argentina', 'Centro', 'Buenos Aires', 'CABA', 'C1000'),
('Argentina', 'Centro', 'Buenos Aires', 'La Plata', 'B1900'),
('Argentina', 'Centro', 'Córdoba', 'Córdoba Capital', 'X5000'),
('Argentina', 'Centro', 'Santa Fe', 'Rosario', 'S2000'),
('Argentina', 'Cuyo', 'Mendoza', 'Mendoza Capital', 'M5500'),
('Argentina', 'NOA', 'Tucumán', 'San Miguel', 'T4000'),
('Argentina', 'NOA', 'Salta', 'Salta Capital', 'A4400'),
('Argentina', 'Patagonia', 'Neuquén', 'Neuquén Capital', 'Q8300'),
('Argentina', 'Patagonia', 'Río Negro', 'Bariloche', 'R8400'),
('Argentina', 'Litoral', 'Entre Ríos', 'Paraná', 'E3100');

-- ============================================================================
-- TABLA DE HECHOS: VENTAS
-- Estructura: 10 productos × 10 meses × 10 clientes = 1000 registros base
-- Con variaciones para crear un cubo denso y visualmente interesante
-- ============================================================================
DROP TABLE IF EXISTS fact_ventas CASCADE;
CREATE TABLE fact_ventas (
    id_venta SERIAL PRIMARY KEY,
    id_producto INTEGER NOT NULL REFERENCES dim_producto(id_producto),
    id_tiempo INTEGER NOT NULL REFERENCES dim_tiempo(id_tiempo),
    id_cliente INTEGER NOT NULL REFERENCES dim_cliente(id_cliente),
    id_geografia INTEGER NOT NULL REFERENCES dim_geografia(id_geografia),
    cantidad INTEGER NOT NULL,
    precio_venta DECIMAL(10,2) NOT NULL,
    descuento DECIMAL(5,2) DEFAULT 0,
    costo_producto DECIMAL(10,2) NOT NULL,
    total_venta DECIMAL(12,2) NOT NULL,
    ganancia DECIMAL(12,2) NOT NULL,
    fecha_venta DATE NOT NULL
);

-- ============================================================================
-- GENERACIÓN DE DATOS: CUBO PERFECTO 10x10x10
-- Estrategia: Crear ventas para cada combinación de Producto × Tiempo × Cliente
-- con valores que formen patrones visuales interesantes en el cubo 3D
-- ============================================================================

-- Función auxiliar para generar ventas con patrón de cubo
DO $$
DECLARE
    v_id_producto INTEGER;
    v_id_tiempo INTEGER;
    v_id_cliente INTEGER;
    v_id_geografia INTEGER;
    v_cantidad INTEGER;
    v_precio DECIMAL(10,2);
    v_descuento DECIMAL(5,2);
    v_costo DECIMAL(10,2);
    v_total DECIMAL(12,2);
    v_ganancia DECIMAL(12,2);
    v_fecha DATE;
    v_mes INTEGER;
    v_anio INTEGER;
BEGIN
    -- Iterar sobre todas las combinaciones para crear un cubo denso
    FOR v_id_producto IN 1..10 LOOP
        FOR v_id_tiempo IN 1..10 LOOP
            FOR v_id_cliente IN 1..10 LOOP
                
                -- Asignar geografía basada en cliente (para coherencia)
                v_id_geografia := ((v_id_cliente - 1) % 10) + 1;
                
                -- Obtener precio del producto
                SELECT precio_unitario INTO v_precio 
                FROM dim_producto WHERE id_producto = v_id_producto;
                
                -- Obtener mes y año
                SELECT mes, anio INTO v_mes, v_anio
                FROM dim_tiempo WHERE id_tiempo = v_id_tiempo;
                
                -- Generar cantidad con patrón interesante
                -- Más ventas en productos populares (1,4,7,10) y meses clave (3,6,9,12)
                v_cantidad := 5 + 
                    CASE 
                        WHEN v_id_producto IN (1,4,7,10) THEN 10
                        ELSE 0
                    END +
                    CASE 
                        WHEN v_id_tiempo IN (3,6,9,10) THEN 8
                        ELSE 0
                    END +
                    CASE
                        WHEN v_id_cliente IN (1,5,10) THEN 5
                        ELSE 0
                    END;
                
                -- Descuento variable (0-20%)
                v_descuento := (((v_id_producto + v_id_tiempo + v_id_cliente) % 5) * 4.0);
                
                -- Costo es 60% del precio
                v_costo := v_precio * 0.6;
                
                -- Calcular total con descuento
                v_total := v_cantidad * v_precio * (1 - v_descuento/100);
                
                -- Calcular ganancia
                v_ganancia := v_total - (v_cantidad * v_costo);
                
                -- Fecha de venta (día aleatorio del mes)
                v_fecha := make_date(v_anio, v_mes, 1 + ((v_id_producto + v_id_cliente) % 28));
                
                -- Insertar registro
                INSERT INTO fact_ventas (
                    id_producto, id_tiempo, id_cliente, id_geografia,
                    cantidad, precio_venta, descuento, costo_producto,
                    total_venta, ganancia, fecha_venta
                ) VALUES (
                    v_id_producto, v_id_tiempo, v_id_cliente, v_id_geografia,
                    v_cantidad, v_precio, v_descuento, v_costo,
                    v_total, v_ganancia, v_fecha
                );
                
            END LOOP;
        END LOOP;
    END LOOP;
    
    RAISE NOTICE 'Cubo de datos generado: 1000 registros (10×10×10)';
END $$;

-- ============================================================================
-- DATOS ADICIONALES: Crear variaciones para operaciones OLAP
-- ============================================================================

-- Agregar más ventas para crear "picos" visuales en el cubo
INSERT INTO fact_ventas (id_producto, id_tiempo, id_cliente, id_geografia, cantidad, precio_venta, descuento, costo_producto, total_venta, ganancia, fecha_venta)
SELECT 
    1, -- Laptop (producto más vendido)
    t.id_tiempo,
    c.id_cliente,
    ((c.id_cliente - 1) % 10) + 1,
    50, -- Alta cantidad
    1200.00,
    10.0,
    720.00,
    54000.00,
    18000.00,
    make_date(2024, t.mes, 15)
FROM dim_tiempo t
CROSS JOIN dim_cliente c
WHERE t.mes IN (3, 6, 9, 12) -- Trimestres clave
  AND c.segmento = 'Corporativo'; -- Clientes corporativos

-- Agregar ventas especiales para Navidad (Diciembre)
INSERT INTO fact_ventas (id_producto, id_tiempo, id_cliente, id_geografia, cantidad, precio_venta, descuento, costo_producto, total_venta, ganancia, fecha_venta)
SELECT 
    p.id_producto,
    10, -- Octubre (último mes)
    c.id_cliente,
    ((c.id_cliente - 1) % 10) + 1,
    30,
    p.precio_unitario,
    15.0,
    p.precio_unitario * 0.6,
    p.precio_unitario * 30 * 0.85,
    p.precio_unitario * 30 * 0.25,
    '2024-10-25'
FROM dim_producto p
CROSS JOIN dim_cliente c
WHERE p.categoria = 'Electrónica'
  AND c.segmento IN ('Retail', 'Corporativo');

-- ============================================================================
-- ÍNDICES PARA OPTIMIZACIÓN DE CONSULTAS OLAP
-- ============================================================================

-- Índices en tabla de hechos
CREATE INDEX idx_fact_ventas_producto ON fact_ventas(id_producto);
CREATE INDEX idx_fact_ventas_tiempo ON fact_ventas(id_tiempo);
CREATE INDEX idx_fact_ventas_cliente ON fact_ventas(id_cliente);
CREATE INDEX idx_fact_ventas_geografia ON fact_ventas(id_geografia);
CREATE INDEX idx_fact_ventas_fecha ON fact_ventas(fecha_venta);

-- Índices compuestos para queries multidimensionales
CREATE INDEX idx_fact_ventas_prod_tiempo ON fact_ventas(id_producto, id_tiempo);
CREATE INDEX idx_fact_ventas_prod_cliente ON fact_ventas(id_producto, id_cliente);
CREATE INDEX idx_fact_ventas_tiempo_cliente ON fact_ventas(id_tiempo, id_cliente);
CREATE INDEX idx_fact_ventas_prod_tiempo_cliente ON fact_ventas(id_producto, id_tiempo, id_cliente);

-- Índices en dimensiones
CREATE INDEX idx_dim_producto_categoria ON dim_producto(categoria);
CREATE INDEX idx_dim_producto_subcategoria ON dim_producto(subcategoria);
CREATE INDEX idx_dim_tiempo_trimestre ON dim_tiempo(trimestre);
CREATE INDEX idx_dim_tiempo_mes ON dim_tiempo(mes);
CREATE INDEX idx_dim_cliente_segmento ON dim_cliente(segmento);
CREATE INDEX idx_dim_cliente_region ON dim_cliente(region);
CREATE INDEX idx_dim_geografia_region ON dim_geografia(region);
CREATE INDEX idx_dim_geografia_provincia ON dim_geografia(provincia);

-- ============================================================================
-- VISTAS MATERIALIZADAS PARA AGREGACIONES PRECALCULADAS
-- ============================================================================

-- Vista: Ventas por Producto y Mes
CREATE MATERIALIZED VIEW mv_ventas_producto_mes AS
SELECT 
    p.id_producto,
    p.nombre_producto,
    p.categoria,
    t.id_tiempo,
    t.nombre_mes,
    t.trimestre,
    COUNT(*) as num_transacciones,
    SUM(v.cantidad) as cantidad_total,
    SUM(v.total_venta) as venta_total,
    SUM(v.ganancia) as ganancia_total,
    AVG(v.total_venta) as venta_promedio
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY p.id_producto, p.nombre_producto, p.categoria, t.id_tiempo, t.nombre_mes, t.trimestre;

CREATE INDEX idx_mv_ventas_prod_mes ON mv_ventas_producto_mes(id_producto, id_tiempo);

-- Vista: Ventas por Cliente y Región
CREATE MATERIALIZED VIEW mv_ventas_cliente_region AS
SELECT 
    c.id_cliente,
    c.nombre_cliente,
    c.segmento,
    g.region,
    g.provincia,
    COUNT(*) as num_transacciones,
    SUM(v.cantidad) as cantidad_total,
    SUM(v.total_venta) as venta_total,
    SUM(v.ganancia) as ganancia_total
FROM fact_ventas v
JOIN dim_cliente c ON v.id_cliente = c.id_cliente
JOIN dim_geografia g ON v.id_geografia = g.id_geografia
GROUP BY c.id_cliente, c.nombre_cliente, c.segmento, g.region, g.provincia;

CREATE INDEX idx_mv_ventas_cli_reg ON mv_ventas_cliente_region(id_cliente, region);

-- ============================================================================
-- ESTADÍSTICAS Y VERIFICACIÓN
-- ============================================================================

-- Actualizar estadísticas para el optimizador
ANALYZE fact_ventas;
ANALYZE dim_producto;
ANALYZE dim_tiempo;
ANALYZE dim_cliente;
ANALYZE dim_geografia;

-- Verificar el cubo generado
SELECT 
    'Total de registros en fact_ventas' as metrica,
    COUNT(*)::TEXT as valor
FROM fact_ventas
UNION ALL
SELECT 
    'Productos únicos',
    COUNT(DISTINCT id_producto)::TEXT
FROM fact_ventas
UNION ALL
SELECT 
    'Meses únicos',
    COUNT(DISTINCT id_tiempo)::TEXT
FROM fact_ventas
UNION ALL
SELECT 
    'Clientes únicos',
    COUNT(DISTINCT id_cliente)::TEXT
FROM fact_ventas
UNION ALL
SELECT 
    'Total de ventas (suma)',
    TO_CHAR(SUM(total_venta), 'FM$999,999,999.00')
FROM fact_ventas
UNION ALL
SELECT 
    'Ganancia total',
    TO_CHAR(SUM(ganancia), 'FM$999,999,999.00')
FROM fact_ventas
UNION ALL
SELECT 
    'Venta promedio',
    TO_CHAR(AVG(total_venta), 'FM$999,999.00')
FROM fact_ventas;

-- Mostrar distribución del cubo
SELECT 
    'Distribución Producto × Tiempo' as dimension,
    COUNT(DISTINCT CONCAT(id_producto, '-', id_tiempo))::TEXT as combinaciones
FROM fact_ventas
UNION ALL
SELECT 
    'Distribución Producto × Cliente',
    COUNT(DISTINCT CONCAT(id_producto, '-', id_cliente))::TEXT
FROM fact_ventas
UNION ALL
SELECT 
    'Distribución Tiempo × Cliente',
    COUNT(DISTINCT CONCAT(id_tiempo, '-', id_cliente))::TEXT
FROM fact_ventas;

-- ============================================================================
-- CONSULTAS DE EJEMPLO PARA PROBAR OPERACIONES OLAP
-- ============================================================================

-- SLICE: Ventas solo de Enero 2024
SELECT 
    p.nombre_producto,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
WHERE t.nombre_mes = 'Enero'
GROUP BY p.nombre_producto
ORDER BY total DESC;

-- DICE: Ventas de Electrónica en Q1 para clientes Corporativos
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

-- DRILL DOWN: De categoría a subcategoría a producto
SELECT 
    p.categoria,
    p.subcategoria,
    p.nombre_producto,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY ROLLUP(p.categoria, p.subcategoria, p.nombre_producto)
ORDER BY p.categoria, p.subcategoria, p.nombre_producto;

-- ROLL UP: De mes a trimestre a año
SELECT 
    t.anio,
    t.trimestre,
    t.nombre_mes,
    SUM(v.total_venta) as total
FROM fact_ventas v
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY ROLLUP(t.anio, t.trimestre, t.nombre_mes)
ORDER BY t.anio, t.trimestre, t.mes;

-- RANKING: Top 10 productos por ventas
SELECT 
    p.nombre_producto,
    SUM(v.total_venta) as total_ventas,
    RANK() OVER (ORDER BY SUM(v.total_venta) DESC) as ranking
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY p.nombre_producto
ORDER BY ranking
LIMIT 10;

-- ============================================================================
-- INFORMACIÓN FINAL
-- ============================================================================

SELECT 
    '✅ Base de datos bd4-2025 creada exitosamente' as status,
    'Cubo OLAP 10×10×10 con ' || COUNT(*)::TEXT || ' registros' as detalle
FROM fact_ventas;

RAISE NOTICE '============================================';
RAISE NOTICE 'CUBO OLAP GENERADO EXITOSAMENTE';
RAISE NOTICE '============================================';
RAISE NOTICE 'Dimensiones:';
RAISE NOTICE '  - Productos: 10 (Eje X)';
RAISE NOTICE '  - Tiempo: 10 meses (Eje Z)';
RAISE NOTICE '  - Clientes: 10 (Variedad)';
RAISE NOTICE '  - Geografía: 10 regiones';
RAISE NOTICE '';
RAISE NOTICE 'Estructura del cubo: 10×10×10 = 1000+ registros';
RAISE NOTICE 'Medidas: total_venta, ganancia, cantidad';
RAISE NOTICE '';
RAISE NOTICE 'Operaciones OLAP soportadas:';
RAISE NOTICE '  ✓ Drill Down / Roll Up';
RAISE NOTICE '  ✓ Slice / Dice';
RAISE NOTICE '  ✓ Pivot / Rotate';
RAISE NOTICE '  ✓ Ranking / Top N';
RAISE NOTICE '  ✓ Drill Through';
RAISE NOTICE '============================================';
