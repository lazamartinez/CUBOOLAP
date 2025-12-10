-- Semilla para Cubo Visión OLAP 2025
-- Ejecutar en PostgreSQL: psql -U postgres -d bd2025 -f semilla.sql

-- 1. Limpieza
DROP TABLE IF EXISTS hechos_ventas CASCADE;
DROP TABLE IF EXISTS dim_tiempo CASCADE;
DROP TABLE IF EXISTS dim_geografia CASCADE;
DROP TABLE IF EXISTS dim_producto CASCADE;

-- 2. Dimensiones
CREATE TABLE dim_tiempo (
    id_tiempo SERIAL PRIMARY KEY,
    anio INT NOT NULL,
    trimestre INT NOT NULL,
    mes INT NOT NULL,
    dia INT NOT NULL
);

CREATE TABLE dim_geografia (
    id_geografia SERIAL PRIMARY KEY,
    pais VARCHAR(50),
    region VARCHAR(50),
    ciudad VARCHAR(50)
);

CREATE TABLE dim_producto (
    id_producto SERIAL PRIMARY KEY,
    categoria VARCHAR(50),
    nombre VARCHAR(100),
    precio_unitario DECIMAL(10,2)
);

-- 3. Tabla de Hechos (Ventas)
CREATE TABLE hechos_ventas (
    id_venta SERIAL PRIMARY KEY,
    id_tiempo INT REFERENCES dim_tiempo(id_tiempo),
    id_geografia INT REFERENCES dim_geografia(id_geografia),
    id_producto INT REFERENCES dim_producto(id_producto),
    cantidad INT,
    monto_total DECIMAL(12,2)
);

-- 4. Datos de Prueba (Poblado Masivo Simulado)

-- Tiempo (2024-2025)
INSERT INTO dim_tiempo (anio, trimestre, mes, dia) VALUES 
(2024, 1, 1, 15), (2024, 1, 2, 20), (2024, 2, 4, 10),
(2024, 3, 7, 5), (2025, 1, 1, 12);

-- Geografía
INSERT INTO dim_geografia (pais, region, ciudad) VALUES 
('Argentina', 'Buenos Aires', 'La Plata'),
('Argentina', 'Córdoba', 'Córdoba Capital'),
('Chile', 'Metropolitana', 'Santiago'),
('Uruguay', 'Montevideo', 'Montevideo'),
('España', 'Madrid', 'Madrid');

-- Productos
INSERT INTO dim_producto (categoria, nombre, precio_unitario) VALUES 
('Electrónica', 'Laptop Gamer X', 1500.00),
('Electrónica', 'Smartphone Pro', 800.00),
('Hogar', 'Cafetera Automática', 120.50),
('Ropa', 'Camiseta Developer', 25.00);

-- Hechos (Ventas Aleatorias)
INSERT INTO hechos_ventas (id_tiempo, id_geografia, id_producto, cantidad, monto_total) VALUES 
(1, 1, 1, 2, 3000.00),
(1, 2, 2, 5, 4000.00),
(2, 3, 3, 10, 1205.00),
(2, 4, 4, 50, 1250.00),
(3, 5, 1, 1, 1500.00),
(3, 1, 2, 3, 2400.00),
(4, 2, 3, 2, 241.00),
(5, 3, 4, 100, 2500.00);

-- 5. Confirmación
SELECT 'Base de datos poblada exitosamente' as estado;
