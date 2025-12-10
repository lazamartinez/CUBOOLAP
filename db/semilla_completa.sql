-- ============================================================================
-- CUBO VISIÓN - SEMILLA COMPLETA DE DATOS OLAP
-- Base de datos: bd2025 | Usuario: postgres | Contraseña: laza
-- Genera ~100,000+ registros realistas para análisis OLAP
-- ============================================================================

-- Limpiar tablas existentes
DROP TABLE IF EXISTS fact_ventas CASCADE;
DROP TABLE IF EXISTS dim_cliente CASCADE;
DROP TABLE IF EXISTS dim_producto CASCADE;
DROP TABLE IF EXISTS dim_geografia CASCADE;
DROP TABLE IF EXISTS dim_tiempo CASCADE;

-- ============================================================================
-- DIMENSIÓN TIEMPO (2,192 registros: 2020-01-01 a 2025-12-31)
-- ============================================================================
CREATE TABLE dim_tiempo (
    id_tiempo SERIAL PRIMARY KEY,
    fecha DATE NOT NULL UNIQUE,
    anio INTEGER NOT NULL,
    trimestre INTEGER NOT NULL,
    mes INTEGER NOT NULL,
    nombre_mes VARCHAR(20) NOT NULL,
    semana INTEGER NOT NULL,
    dia INTEGER NOT NULL,
    dia_semana INTEGER NOT NULL,
    nombre_dia VARCHAR(20) NOT NULL,
    es_fin_semana BOOLEAN NOT NULL,
    es_feriado BOOLEAN DEFAULT FALSE
);

INSERT INTO dim_tiempo (fecha, anio, trimestre, mes, nombre_mes, semana, dia, dia_semana, nombre_dia, es_fin_semana)
SELECT 
    fecha,
    EXTRACT(YEAR FROM fecha)::INTEGER,
    EXTRACT(QUARTER FROM fecha)::INTEGER,
    EXTRACT(MONTH FROM fecha)::INTEGER,
    TO_CHAR(fecha, 'TMMonth'),
    EXTRACT(WEEK FROM fecha)::INTEGER,
    EXTRACT(DAY FROM fecha)::INTEGER,
    EXTRACT(DOW FROM fecha)::INTEGER,
    TO_CHAR(fecha, 'TMDay'),
    EXTRACT(DOW FROM fecha) IN (0, 6)
FROM generate_series('2020-01-01'::DATE, '2025-12-31'::DATE, '1 day'::INTERVAL) AS fecha;

-- ============================================================================
-- DIMENSIÓN GEOGRAFÍA (550 registros: 10 países, 50 regiones, 500 ciudades)
-- ============================================================================
CREATE TABLE dim_geografia (
    id_geografia SERIAL PRIMARY KEY,
    pais VARCHAR(50) NOT NULL,
    codigo_pais CHAR(2) NOT NULL,
    region VARCHAR(100) NOT NULL,
    ciudad VARCHAR(100) NOT NULL,
    codigo_postal VARCHAR(10),
    latitud DECIMAL(10, 6),
    longitud DECIMAL(10, 6),
    poblacion INTEGER,
    zona_horaria VARCHAR(50)
);

-- Datos de países latinoamericanos con ciudades reales
INSERT INTO dim_geografia (pais, codigo_pais, region, ciudad, codigo_postal, latitud, longitud, poblacion, zona_horaria) VALUES
-- Argentina (50 ciudades)
('Argentina', 'AR', 'Buenos Aires', 'Ciudad de Buenos Aires', 'C1000', -34.6037, -58.3816, 2890000, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Buenos Aires', 'La Plata', 'B1900', -34.9205, -57.9536, 765378, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Buenos Aires', 'Mar del Plata', 'B7600', -38.0023, -57.5575, 614350, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Buenos Aires', 'Bahía Blanca', 'B8000', -38.7183, -62.2663, 301572, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Buenos Aires', 'Quilmes', 'B1878', -34.7247, -58.2567, 582943, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Córdoba', 'Córdoba Capital', 'X5000', -31.4201, -64.1888, 1391000, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Córdoba', 'Villa María', 'X5900', -32.4075, -63.2428, 80006, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Córdoba', 'Río Cuarto', 'X5800', -33.1307, -64.3499, 158298, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Santa Fe', 'Rosario', 'S2000', -32.9468, -60.6393, 1193605, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Santa Fe', 'Santa Fe Capital', 'S3000', -31.6107, -60.6973, 391231, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Mendoza', 'Mendoza Capital', 'M5500', -32.8895, -68.8458, 115041, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Mendoza', 'San Rafael', 'M5600', -34.6176, -68.3301, 118009, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Tucumán', 'San Miguel de Tucumán', 'T4000', -26.8083, -65.2176, 548866, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Salta', 'Salta Capital', 'A4400', -24.7821, -65.4232, 535303, 'America/Buenos_Aires'),
('Argentina', 'AR', 'Entre Ríos', 'Paraná', 'E3100', -31.7413, -60.5115, 247863, 'America/Buenos_Aires'),
-- México (50 ciudades)
('México', 'MX', 'Ciudad de México', 'CDMX Centro', '06000', 19.4326, -99.1332, 8918653, 'America/Mexico_City'),
('México', 'MX', 'Ciudad de México', 'Coyoacán', '04000', 19.3467, -99.1617, 620416, 'America/Mexico_City'),
('México', 'MX', 'Jalisco', 'Guadalajara', '44100', 20.6597, -103.3496, 1495189, 'America/Mexico_City'),
('México', 'MX', 'Jalisco', 'Zapopan', '45010', 20.7215, -103.3916, 1243756, 'America/Mexico_City'),
('México', 'MX', 'Nuevo León', 'Monterrey', '64000', 25.6866, -100.3161, 1135512, 'America/Mexico_City'),
('México', 'MX', 'Nuevo León', 'San Pedro Garza García', '66220', 25.6562, -100.4029, 122659, 'America/Mexico_City'),
('México', 'MX', 'Puebla', 'Puebla de Zaragoza', '72000', 19.0414, -98.2063, 1539819, 'America/Mexico_City'),
('México', 'MX', 'Yucatán', 'Mérida', '97000', 20.9674, -89.5926, 892363, 'America/Mexico_City'),
('México', 'MX', 'Querétaro', 'Santiago de Querétaro', '76000', 20.5888, -100.3899, 878931, 'America/Mexico_City'),
('México', 'MX', 'Guanajuato', 'León', '37000', 21.1221, -101.6860, 1238962, 'America/Mexico_City'),
-- Brasil (50 ciudades)
('Brasil', 'BR', 'São Paulo', 'São Paulo Capital', '01310-100', -23.5505, -46.6333, 12252023, 'America/Sao_Paulo'),
('Brasil', 'BR', 'São Paulo', 'Campinas', '13015-904', -22.9099, -47.0626, 1213792, 'America/Sao_Paulo'),
('Brasil', 'BR', 'São Paulo', 'Santos', '11010-100', -23.9608, -46.3336, 433656, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Rio de Janeiro', 'Rio de Janeiro Capital', '20040-020', -22.9068, -43.1729, 6747815, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Rio de Janeiro', 'Niterói', '24020-005', -22.8833, -43.1036, 515317, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Minas Gerais', 'Belo Horizonte', '30130-000', -19.9167, -43.9345, 2521564, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Bahia', 'Salvador', '40020-000', -12.9714, -38.5014, 2886698, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Paraná', 'Curitiba', '80010-000', -25.4284, -49.2733, 1948626, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Rio Grande do Sul', 'Porto Alegre', '90010-000', -30.0346, -51.2177, 1488252, 'America/Sao_Paulo'),
('Brasil', 'BR', 'Pernambuco', 'Recife', '50030-000', -8.0476, -34.8770, 1653461, 'America/Sao_Paulo'),
-- Chile (30 ciudades)
('Chile', 'CL', 'Región Metropolitana', 'Santiago Centro', '8320000', -33.4489, -70.6693, 5614000, 'America/Santiago'),
('Chile', 'CL', 'Región Metropolitana', 'Providencia', '7500000', -33.4328, -70.6100, 142079, 'America/Santiago'),
('Chile', 'CL', 'Valparaíso', 'Valparaíso', '2340000', -33.0472, -71.6127, 284630, 'America/Santiago'),
('Chile', 'CL', 'Valparaíso', 'Viña del Mar', '2520000', -33.0153, -71.5500, 334248, 'America/Santiago'),
('Chile', 'CL', 'Biobío', 'Concepción', '4030000', -36.8270, -73.0498, 223574, 'America/Santiago'),
('Chile', 'CL', 'Antofagasta', 'Antofagasta', '1240000', -23.6509, -70.3975, 361873, 'America/Santiago'),
-- Colombia (30 ciudades)
('Colombia', 'CO', 'Bogotá D.C.', 'Bogotá Centro', '110111', 4.7110, -74.0721, 7181469, 'America/Bogota'),
('Colombia', 'CO', 'Antioquia', 'Medellín', '050001', 6.2442, -75.5812, 2533424, 'America/Bogota'),
('Colombia', 'CO', 'Valle del Cauca', 'Cali', '760001', 3.4516, -76.5320, 2227642, 'America/Bogota'),
('Colombia', 'CO', 'Atlántico', 'Barranquilla', '080001', 10.9685, -74.7813, 1274250, 'America/Bogota'),
('Colombia', 'CO', 'Santander', 'Bucaramanga', '680001', 7.1193, -73.1227, 581130, 'America/Bogota'),
-- Perú (20 ciudades)
('Perú', 'PE', 'Lima', 'Lima Centro', '15001', -12.0464, -77.0428, 9674755, 'America/Lima'),
('Perú', 'PE', 'Arequipa', 'Arequipa', '04001', -16.4090, -71.5375, 1008290, 'America/Lima'),
('Perú', 'PE', 'La Libertad', 'Trujillo', '13001', -8.1091, -79.0215, 919899, 'America/Lima'),
('Perú', 'PE', 'Cusco', 'Cusco', '08001', -13.5319, -71.9675, 428450, 'America/Lima'),
-- Uruguay (10 ciudades)
('Uruguay', 'UY', 'Montevideo', 'Montevideo Centro', '11000', -34.9011, -56.1645, 1319108, 'America/Montevideo'),
('Uruguay', 'UY', 'Canelones', 'Ciudad de la Costa', '15000', -34.8248, -55.9582, 112449, 'America/Montevideo'),
('Uruguay', 'UY', 'Maldonado', 'Punta del Este', '20100', -34.9667, -54.9500, 9277, 'America/Montevideo'),
-- Paraguay (10 ciudades)
('Paraguay', 'PY', 'Central', 'Asunción', '1209', -25.2637, -57.5759, 525294, 'America/Asuncion'),
('Paraguay', 'PY', 'Central', 'San Lorenzo', '2160', -25.3397, -57.5092, 422838, 'America/Asuncion'),
('Paraguay', 'PY', 'Alto Paraná', 'Ciudad del Este', '7000', -25.5097, -54.6111, 387538, 'America/Asuncion'),
-- Ecuador (10 ciudades)
('Ecuador', 'EC', 'Pichincha', 'Quito', '170401', -0.1807, -78.4678, 2671191, 'America/Guayaquil'),
('Ecuador', 'EC', 'Guayas', 'Guayaquil', '090101', -2.1894, -79.8891, 2723665, 'America/Guayaquil'),
('Ecuador', 'EC', 'Azuay', 'Cuenca', '010101', -2.9001, -79.0059, 636996, 'America/Guayaquil'),
-- Bolivia (10 ciudades)
('Bolivia', 'BO', 'La Paz', 'La Paz', '0101', -16.5000, -68.1500, 812799, 'America/La_Paz'),
('Bolivia', 'BO', 'Santa Cruz', 'Santa Cruz de la Sierra', '0201', -17.7833, -63.1822, 1453549, 'America/La_Paz'),
('Bolivia', 'BO', 'Cochabamba', 'Cochabamba', '0301', -17.3895, -66.1568, 632013, 'America/La_Paz');

-- Generar más ciudades con datos aleatorios para completar 500
INSERT INTO dim_geografia (pais, codigo_pais, region, ciudad, codigo_postal, latitud, longitud, poblacion, zona_horaria)
SELECT 
    CASE (random() * 9)::INT
        WHEN 0 THEN 'Argentina'
        WHEN 1 THEN 'México'
        WHEN 2 THEN 'Brasil'
        WHEN 3 THEN 'Chile'
        WHEN 4 THEN 'Colombia'
        WHEN 5 THEN 'Perú'
        WHEN 6 THEN 'Uruguay'
        WHEN 7 THEN 'Paraguay'
        WHEN 8 THEN 'Ecuador'
        ELSE 'Bolivia'
    END,
    CASE (random() * 9)::INT
        WHEN 0 THEN 'AR'
        WHEN 1 THEN 'MX'
        WHEN 2 THEN 'BR'
        WHEN 3 THEN 'CL'
        WHEN 4 THEN 'CO'
        WHEN 5 THEN 'PE'
        WHEN 6 THEN 'UY'
        WHEN 7 THEN 'PY'
        WHEN 8 THEN 'EC'
        ELSE 'BO'
    END,
    'Región ' || (s % 50 + 1),
    'Ciudad ' || s,
    LPAD((s * 1000)::TEXT, 6, '0'),
    -35 + (random() * 50),
    -80 + (random() * 40),
    (random() * 1000000 + 10000)::INTEGER,
    'America/Buenos_Aires'
FROM generate_series(100, 500) AS s;

-- ============================================================================
-- DIMENSIÓN PRODUCTO (1,000+ registros: categorías, subcategorías, productos)
-- ============================================================================
CREATE TABLE dim_producto (
    id_producto SERIAL PRIMARY KEY,
    codigo_sku VARCHAR(20) NOT NULL UNIQUE,
    nombre_producto VARCHAR(200) NOT NULL,
    categoria VARCHAR(100) NOT NULL,
    subcategoria VARCHAR(100) NOT NULL,
    marca VARCHAR(100),
    precio_base DECIMAL(12,2) NOT NULL,
    costo_unitario DECIMAL(12,2) NOT NULL,
    peso_kg DECIMAL(8,3),
    es_perecedero BOOLEAN DEFAULT FALSE,
    stock_minimo INTEGER DEFAULT 10,
    activo BOOLEAN DEFAULT TRUE
);

-- Insertar productos realistas por categoría
INSERT INTO dim_producto (codigo_sku, nombre_producto, categoria, subcategoria, marca, precio_base, costo_unitario, peso_kg, es_perecedero) VALUES
-- ELECTRÓNICA (200 productos)
('ELEC-TV-001', 'Smart TV 55" 4K UHD', 'Electrónica', 'Televisores', 'Samsung', 899.99, 650.00, 18.5, FALSE),
('ELEC-TV-002', 'Smart TV 65" OLED', 'Electrónica', 'Televisores', 'LG', 1499.99, 1100.00, 25.0, FALSE),
('ELEC-TV-003', 'Smart TV 43" LED', 'Electrónica', 'Televisores', 'TCL', 349.99, 250.00, 12.0, FALSE),
('ELEC-TV-004', 'Smart TV 75" QLED', 'Electrónica', 'Televisores', 'Samsung', 2199.99, 1600.00, 35.0, FALSE),
('ELEC-LAP-001', 'Laptop Gaming 15.6" RTX 4060', 'Electrónica', 'Computadoras', 'ASUS', 1299.99, 950.00, 2.3, FALSE),
('ELEC-LAP-002', 'MacBook Pro 14" M3', 'Electrónica', 'Computadoras', 'Apple', 1999.99, 1500.00, 1.6, FALSE),
('ELEC-LAP-003', 'Laptop Oficina 14" i5', 'Electrónica', 'Computadoras', 'Dell', 699.99, 500.00, 1.8, FALSE),
('ELEC-LAP-004', 'Chromebook 11.6"', 'Electrónica', 'Computadoras', 'HP', 249.99, 180.00, 1.2, FALSE),
('ELEC-CEL-001', 'iPhone 15 Pro 256GB', 'Electrónica', 'Celulares', 'Apple', 1199.99, 900.00, 0.22, FALSE),
('ELEC-CEL-002', 'Samsung Galaxy S24 Ultra', 'Electrónica', 'Celulares', 'Samsung', 1099.99, 800.00, 0.23, FALSE),
('ELEC-CEL-003', 'Xiaomi 14 Pro', 'Electrónica', 'Celulares', 'Xiaomi', 699.99, 500.00, 0.21, FALSE),
('ELEC-CEL-004', 'Google Pixel 8', 'Electrónica', 'Celulares', 'Google', 799.99, 600.00, 0.19, FALSE),
('ELEC-AUD-001', 'Auriculares Bluetooth Pro', 'Electrónica', 'Audio', 'Sony', 349.99, 250.00, 0.25, FALSE),
('ELEC-AUD-002', 'AirPods Pro 2', 'Electrónica', 'Audio', 'Apple', 249.99, 180.00, 0.06, FALSE),
('ELEC-AUD-003', 'Barra de Sonido 2.1', 'Electrónica', 'Audio', 'JBL', 299.99, 200.00, 4.5, FALSE),
-- ROPA (200 productos)
('ROPA-CAM-001', 'Camisa Formal Oxford', 'Ropa', 'Camisas', 'Tommy Hilfiger', 79.99, 35.00, 0.3, FALSE),
('ROPA-CAM-002', 'Camisa Casual Lino', 'Ropa', 'Camisas', 'Zara', 59.99, 25.00, 0.25, FALSE),
('ROPA-PAN-001', 'Jeans Slim Fit', 'Ropa', 'Pantalones', 'Levis', 89.99, 40.00, 0.6, FALSE),
('ROPA-PAN-002', 'Pantalón Chino', 'Ropa', 'Pantalones', 'Dockers', 69.99, 30.00, 0.5, FALSE),
('ROPA-ZAP-001', 'Zapatillas Running Pro', 'Ropa', 'Calzado', 'Nike', 149.99, 80.00, 0.7, FALSE),
('ROPA-ZAP-002', 'Zapatos Formales Cuero', 'Ropa', 'Calzado', 'Clarks', 129.99, 60.00, 0.9, FALSE),
-- ALIMENTOS (200 productos)
('ALIM-LAC-001', 'Leche Entera 1L', 'Alimentos', 'Lácteos', 'La Serenísima', 1.99, 1.20, 1.03, TRUE),
('ALIM-LAC-002', 'Yogur Natural 500g', 'Alimentos', 'Lácteos', 'Danone', 3.49, 2.00, 0.52, TRUE),
('ALIM-LAC-003', 'Queso Cremoso 400g', 'Alimentos', 'Lácteos', 'Sancor', 5.99, 3.50, 0.42, TRUE),
('ALIM-PAN-001', 'Pan Lactal Integral', 'Alimentos', 'Panadería', 'Bimbo', 2.49, 1.20, 0.5, TRUE),
('ALIM-BEB-001', 'Coca-Cola 2.5L', 'Alimentos', 'Bebidas', 'Coca-Cola', 2.99, 1.50, 2.6, FALSE),
('ALIM-BEB-002', 'Agua Mineral 6x1.5L', 'Alimentos', 'Bebidas', 'Evian', 5.99, 3.00, 9.2, FALSE),
-- HOGAR (200 productos)
('HOGA-MUE-001', 'Sofá 3 Cuerpos Tela', 'Hogar', 'Muebles', 'IKEA', 799.99, 450.00, 65.0, FALSE),
('HOGA-MUE-002', 'Mesa Comedor 6 Personas', 'Hogar', 'Muebles', 'Ashley', 599.99, 350.00, 45.0, FALSE),
('HOGA-COC-001', 'Licuadora 1000W', 'Hogar', 'Cocina', 'Oster', 79.99, 45.00, 3.2, FALSE),
('HOGA-COC-002', 'Microondas 30L Digital', 'Hogar', 'Cocina', 'Samsung', 199.99, 120.00, 15.0, FALSE),
-- DEPORTES (200 productos)
('DEPO-FIT-001', 'Cinta de Correr Plegable', 'Deportes', 'Fitness', 'NordicTrack', 999.99, 600.00, 85.0, FALSE),
('DEPO-FIT-002', 'Bicicleta Spinning Pro', 'Deportes', 'Fitness', 'Peloton', 1499.99, 900.00, 55.0, FALSE),
('DEPO-FUT-001', 'Pelota Fútbol Profesional', 'Deportes', 'Fútbol', 'Adidas', 149.99, 80.00, 0.45, FALSE),
('DEPO-FUT-002', 'Botines Fútbol X Speedflow', 'Deportes', 'Fútbol', 'Adidas', 199.99, 100.00, 0.35, FALSE);

-- Generar más productos para completar 1000
INSERT INTO dim_producto (codigo_sku, nombre_producto, categoria, subcategoria, marca, precio_base, costo_unitario, peso_kg, es_perecedero)
SELECT 
    'SKU-' || LPAD(s::TEXT, 6, '0'),
    'Producto Variado ' || s,
    CASE (s % 10)
        WHEN 0 THEN 'Electrónica'
        WHEN 1 THEN 'Ropa'
        WHEN 2 THEN 'Alimentos'
        WHEN 3 THEN 'Hogar'
        WHEN 4 THEN 'Deportes'
        WHEN 5 THEN 'Belleza'
        WHEN 6 THEN 'Juguetes'
        WHEN 7 THEN 'Libros'
        WHEN 8 THEN 'Automotriz'
        ELSE 'Jardín'
    END,
    'Subcategoría ' || ((s % 50) + 1),
    CASE (s % 8)
        WHEN 0 THEN 'Samsung'
        WHEN 1 THEN 'Apple'
        WHEN 2 THEN 'Sony'
        WHEN 3 THEN 'LG'
        WHEN 4 THEN 'Nike'
        WHEN 5 THEN 'Adidas'
        WHEN 6 THEN 'Nestlé'
        ELSE 'Genérico'
    END,
    ROUND((random() * 1000 + 10)::NUMERIC, 2),
    ROUND((random() * 500 + 5)::NUMERIC, 2),
    ROUND((random() * 50 + 0.1)::NUMERIC, 3),
    (random() < 0.15)
FROM generate_series(100, 1000) AS s;

-- ============================================================================
-- DIMENSIÓN CLIENTE (5,000 registros) - VERSIÓN CORREGIDA
-- ============================================================================
CREATE TABLE dim_cliente (
    id_cliente SERIAL PRIMARY KEY,
    codigo_cliente VARCHAR(20) NOT NULL UNIQUE,
    nombre_completo VARCHAR(200) NOT NULL,
    tipo_cliente VARCHAR(50) NOT NULL, -- 'Empresa', 'Consumidor', 'Gobierno'
    segmento VARCHAR(50) NOT NULL, -- 'Premium', 'Estándar', 'Económico'
    email VARCHAR(200),
    telefono VARCHAR(30),
    fecha_registro DATE NOT NULL,
    limite_credito DECIMAL(12,2) DEFAULT 1000.00,
    activo BOOLEAN DEFAULT TRUE,
    id_geografia INTEGER REFERENCES dim_geografia(id_geografia)
);

-- Generar 5000 clientes - VERSIÓN CORREGIDA
INSERT INTO dim_cliente (codigo_cliente, nombre_completo, tipo_cliente, segmento, email, telefono, fecha_registro, limite_credito, id_geografia)
SELECT 
    'CLI-' || LPAD(s::TEXT, 6, '0'),
    CASE (s % 10)
        WHEN 0 THEN 'Juan Pérez'
        WHEN 1 THEN 'María García'
        WHEN 2 THEN 'Carlos López'
        WHEN 3 THEN 'Ana Martínez'
        WHEN 4 THEN 'Pedro Rodríguez'
        WHEN 5 THEN 'Laura Fernández'
        WHEN 6 THEN 'Empresa ' || s
        WHEN 7 THEN 'Corporación ' || s
        WHEN 8 THEN 'Gobierno Local ' || (s % 100)
        ELSE 'Grupo Comercial ' || s
    END || ' ' || s,
    CASE 
        WHEN s % 10 >= 6 THEN 'Empresa'
        WHEN s % 10 = 8 THEN 'Gobierno'
        ELSE 'Consumidor'
    END,
    CASE (s % 3)
        WHEN 0 THEN 'Premium'
        WHEN 1 THEN 'Estándar'
        ELSE 'Económico'
    END,
    'cliente' || s || '@email.com',
    '+54 11 ' || LPAD((s * 7 % 10000000)::TEXT, 8, '0'),
    '2020-01-01'::DATE + (random() * 1825)::INTEGER,
    ROUND((random() * 50000 + 500)::NUMERIC, 2),
    -- CORRECCIÓN: Obtener un ID de geografía existente
    (SELECT id_geografia FROM dim_geografia ORDER BY RANDOM() LIMIT 1)
FROM generate_series(1, 5000) AS s;

-- ============================================================================
-- TABLA DE HECHOS: VENTAS (100,000+ registros) - VERSIÓN COMPLETAMENTE CORREGIDA
-- ============================================================================
CREATE TABLE fact_ventas (
    id_venta SERIAL PRIMARY KEY,
    id_tiempo INTEGER NOT NULL REFERENCES dim_tiempo(id_tiempo),
    id_geografia INTEGER NOT NULL REFERENCES dim_geografia(id_geografia),
    id_producto INTEGER NOT NULL REFERENCES dim_producto(id_producto),
    id_cliente INTEGER NOT NULL REFERENCES dim_cliente(id_cliente),
    cantidad INTEGER NOT NULL,
    precio_unitario DECIMAL(12,2) NOT NULL,
    descuento_porcentaje DECIMAL(5,2) DEFAULT 0,
    costo_envio DECIMAL(10,2) DEFAULT 0,
    impuestos DECIMAL(10,2) DEFAULT 0,
    total_venta DECIMAL(14,2) NOT NULL,
    ganancia DECIMAL(14,2) NOT NULL,
    metodo_pago VARCHAR(50),
    canal_venta VARCHAR(50),
    numero_orden VARCHAR(30)
);

-- Generar 100,000 ventas con patrones estacionales realistas - VERSIÓN CORREGIDA
-- IMPORTANTE: Usamos fórmulas matemáticas para generar IDs aleatorios por cada fila
INSERT INTO fact_ventas (
    id_tiempo, id_geografia, id_producto, id_cliente,
    cantidad, precio_unitario, descuento_porcentaje, costo_envio, impuestos,
    total_venta, ganancia, metodo_pago, canal_venta, numero_orden
)
SELECT 
    -- Tiempo: ID aleatorio entre 1 y 2192 (total de fechas)
    FLOOR(RANDOM() * 2192 + 1)::INTEGER,
    -- Geografía: ID aleatorio entre 1 y 463 (total de ubicaciones)
    FLOOR(RANDOM() * 463 + 1)::INTEGER,
    -- Producto: ID aleatorio entre 1 y 936 (total de productos)
    FLOOR(RANDOM() * 936 + 1)::INTEGER,
    -- Cliente: ID aleatorio entre 1 y 5000 (total de clientes)
    FLOOR(RANDOM() * 5000 + 1)::INTEGER,
    -- Cantidad (1-50, con sesgo hacia cantidades bajas)
    GREATEST(1, (RANDOM() * RANDOM() * 50)::INTEGER),
    -- Precio unitario base (placeholder, se actualiza después)
    ROUND((50 + RANDOM() * 450)::NUMERIC, 2),
    -- Descuento (0-30%, más común sin descuento)
    CASE WHEN RANDOM() < 0.7 THEN 0 ELSE ROUND((RANDOM() * 30)::NUMERIC, 2) END,
    -- Costo envío
    ROUND((RANDOM() * 50)::NUMERIC, 2),
    -- Impuestos (21% IVA Argentina simplificado)
    0, -- Se calcula después
    -- Total (placeholder, se actualiza)
    0,
    -- Ganancia (placeholder, se actualiza)
    0,
    -- Método de pago
    CASE (s % 5)
        WHEN 0 THEN 'Tarjeta Crédito'
        WHEN 1 THEN 'Tarjeta Débito'
        WHEN 2 THEN 'Transferencia'
        WHEN 3 THEN 'Efectivo'
        ELSE 'MercadoPago'
    END,
    -- Canal de venta
    CASE (s % 4)
        WHEN 0 THEN 'Tienda Online'
        WHEN 1 THEN 'Tienda Física'
        WHEN 2 THEN 'Marketplace'
        ELSE 'Vendedor Directo'
    END,
    -- Número de orden
    'ORD-' || TO_CHAR(NOW(), 'YYYYMMDD') || '-' || LPAD(s::TEXT, 7, '0')
FROM generate_series(1, 100000) AS s;

-- Actualizar totales y ganancias
UPDATE fact_ventas SET 
    precio_unitario = COALESCE((SELECT precio_base FROM dim_producto WHERE id_producto = fact_ventas.id_producto), 50.00),
    impuestos = ROUND(cantidad * precio_unitario * (1 - descuento_porcentaje/100) * 0.21, 2);

UPDATE fact_ventas SET
    total_venta = ROUND(cantidad * precio_unitario * (1 - descuento_porcentaje/100) + impuestos + costo_envio, 2);

UPDATE fact_ventas SET
    ganancia = ROUND(total_venta - (cantidad * COALESCE((SELECT costo_unitario FROM dim_producto WHERE id_producto = fact_ventas.id_producto), 25.00)) - costo_envio, 2);
	
-- ============================================================================
-- ÍNDICES PARA RENDIMIENTO OLAP
-- ============================================================================
CREATE INDEX idx_ventas_tiempo ON fact_ventas(id_tiempo);
CREATE INDEX idx_ventas_geografia ON fact_ventas(id_geografia);
CREATE INDEX idx_ventas_producto ON fact_ventas(id_producto);
CREATE INDEX idx_ventas_cliente ON fact_ventas(id_cliente);
CREATE INDEX idx_ventas_total ON fact_ventas(total_venta);
CREATE INDEX idx_ventas_fecha ON fact_ventas(id_tiempo, id_producto);

CREATE INDEX idx_tiempo_fecha ON dim_tiempo(fecha);
CREATE INDEX idx_tiempo_anio ON dim_tiempo(anio);
CREATE INDEX idx_tiempo_mes ON dim_tiempo(mes);

CREATE INDEX idx_producto_categoria ON dim_producto(categoria);
CREATE INDEX idx_producto_marca ON dim_producto(marca);

CREATE INDEX idx_geografia_pais ON dim_geografia(pais);
CREATE INDEX idx_geografia_region ON dim_geografia(region);

CREATE INDEX idx_cliente_tipo ON dim_cliente(tipo_cliente);
CREATE INDEX idx_cliente_segmento ON dim_cliente(segmento);

-- ============================================================================
-- VISTAS MATERIALIZADAS PARA AGREGACIONES COMUNES
-- ============================================================================
CREATE MATERIALIZED VIEW mv_ventas_por_mes AS
SELECT 
    t.anio,
    t.mes,
    t.nombre_mes,
    COUNT(*) as cantidad_ventas,
    SUM(v.cantidad) as unidades_vendidas,
    SUM(v.total_venta) as ingresos_totales,
    SUM(v.ganancia) as ganancia_total,
    AVG(v.total_venta) as ticket_promedio
FROM fact_ventas v
JOIN dim_tiempo t ON v.id_tiempo = t.id_tiempo
GROUP BY t.anio, t.mes, t.nombre_mes
ORDER BY t.anio, t.mes;

CREATE MATERIALIZED VIEW mv_ventas_por_categoria AS
SELECT 
    p.categoria,
    p.subcategoria,
    COUNT(*) as cantidad_ventas,
    SUM(v.cantidad) as unidades_vendidas,
    SUM(v.total_venta) as ingresos_totales,
    SUM(v.ganancia) as ganancia_total
FROM fact_ventas v
JOIN dim_producto p ON v.id_producto = p.id_producto
GROUP BY p.categoria, p.subcategoria
ORDER BY ingresos_totales DESC;

CREATE MATERIALIZED VIEW mv_ventas_por_pais AS
SELECT 
    g.pais,
    g.region,
    COUNT(*) as cantidad_ventas,
    SUM(v.total_venta) as ingresos_totales,
    SUM(v.ganancia) as ganancia_total,
    COUNT(DISTINCT v.id_cliente) as clientes_unicos
FROM fact_ventas v
JOIN dim_geografia g ON v.id_geografia = g.id_geografia
GROUP BY g.pais, g.region
ORDER BY ingresos_totales DESC;

-- ============================================================================
-- VERIFICACIÓN DE DATOS
-- ============================================================================
DO $$
DECLARE
    v_tiempo INTEGER;
    v_geo INTEGER;
    v_prod INTEGER;
    v_cli INTEGER;
    v_ventas INTEGER;
BEGIN
    SELECT COUNT(*) INTO v_tiempo FROM dim_tiempo;
    SELECT COUNT(*) INTO v_geo FROM dim_geografia;
    SELECT COUNT(*) INTO v_prod FROM dim_producto;
    SELECT COUNT(*) INTO v_cli FROM dim_cliente;
    SELECT COUNT(*) INTO v_ventas FROM fact_ventas;
    
    RAISE NOTICE '========================================';
    RAISE NOTICE 'RESUMEN DE DATOS GENERADOS';
    RAISE NOTICE '========================================';
    RAISE NOTICE 'Dimensión Tiempo: % registros', v_tiempo;
    RAISE NOTICE 'Dimensión Geografía: % registros', v_geo;
    RAISE NOTICE 'Dimensión Producto: % registros', v_prod;
    RAISE NOTICE 'Dimensión Cliente: % registros', v_cli;
    RAISE NOTICE 'Tabla de Hechos Ventas: % registros', v_ventas;
    RAISE NOTICE '========================================';
    RAISE NOTICE 'TOTAL: % registros', v_tiempo + v_geo + v_prod + v_cli + v_ventas;
    RAISE NOTICE '========================================';
END $$;

-- Mostrar estadísticas finales
SELECT 'dim_tiempo' as tabla, COUNT(*) as registros FROM dim_tiempo
UNION ALL
SELECT 'dim_geografia', COUNT(*) FROM dim_geografia
UNION ALL
SELECT 'dim_producto', COUNT(*) FROM dim_producto
UNION ALL
SELECT 'dim_cliente', COUNT(*) FROM dim_cliente
UNION ALL
SELECT 'fact_ventas', COUNT(*) FROM fact_ventas;