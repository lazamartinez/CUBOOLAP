# 🚀 Instrucciones para Ejecutar la Semilla de bd4-2025

## Opción 1: Usando pgAdmin (Recomendado - Más Fácil)

1. **Abrir pgAdmin** (buscar en el menú de Windows)

2. **Conectar al servidor PostgreSQL**:
   - Expandir "Servers" en el panel izquierdo
   - Hacer clic en "PostgreSQL [versión]"
   - Ingresar password: `laza`

3. **Crear la base de datos**:
   - Click derecho en "Databases"
   - Seleccionar "Create" → "Database..."
   - Nombre: `bd4-2025`
   - Owner: `postgresql`
   - Click "Save"

4. **Ejecutar el script**:
   - Click derecho en la base de datos `bd4-2025`
   - Seleccionar "Query Tool"
   - Click en el ícono de carpeta (Open File)
   - Navegar a: `C:\Proyectos Facu\OLAPBD2025\database\seed_bd4-2025.sql`
   - Click en el botón "Execute" (▶️ o F5)

5. **Esperar** (10-30 segundos) hasta que aparezca "Query returned successfully"

6. **Verificar**:
   - En el Query Tool, ejecutar:
   ```sql
   SELECT COUNT(*) FROM fact_ventas;
   ```
   - Debe retornar **1000+** registros

---

## Opción 2: Usando línea de comandos

### Paso 1: Abrir PowerShell como Administrador

1. Presionar `Win + X`
2. Seleccionar "Windows PowerShell (Admin)" o "Terminal (Admin)"

### Paso 2: Navegar a la carpeta de PostgreSQL

Buscar tu instalación de PostgreSQL (versión puede variar):

```powershell
# Probar estas rutas hasta encontrar la correcta:
cd "C:\Program Files\PostgreSQL\16\bin"
# o
cd "C:\Program Files\PostgreSQL\15\bin"
# o
cd "C:\Program Files\PostgreSQL\14\bin"
```

### Paso 3: Crear la base de datos

```powershell
.\psql.exe -U postgresql -d postgres -c "DROP DATABASE IF EXISTS `"bd4-2025`"; CREATE DATABASE `"bd4-2025`";"
```

Cuando pida password, ingresar: `laza`

### Paso 4: Ejecutar el script de semilla

```powershell
.\psql.exe -U postgresql -d bd4-2025 -f "C:\Proyectos Facu\OLAPBD2025\database\seed_bd4-2025.sql"
```

### Paso 5: Verificar

```powershell
.\psql.exe -U postgresql -d bd4-2025 -c "SELECT COUNT(*) FROM fact_ventas;"
```

Debe mostrar: **1000+** registros

---

## Opción 3: Usando DBeaver / DataGrip / Otro IDE SQL

1. **Crear conexión**:
   - Host: `localhost`
   - Port: `5432`
   - Database: `postgres` (inicialmente)
   - Username: `postgresql`
   - Password: `laza`

2. **Crear base de datos**:
   ```sql
   DROP DATABASE IF EXISTS "bd4-2025";
   CREATE DATABASE "bd4-2025";
   ```

3. **Cambiar a la nueva base de datos**:
   - Crear nueva conexión apuntando a `bd4-2025`

4. **Abrir y ejecutar el script**:
   - File → Open → `seed_bd4-2025.sql`
   - Execute All (Ctrl+Enter o botón Run)

---

## ✅ Verificación de Instalación Exitosa

Ejecutar estas queries para confirmar:

```sql
-- Debe retornar 1000+
SELECT COUNT(*) as total_ventas FROM fact_ventas;

-- Debe retornar 10, 10, 10, 10
SELECT 
    (SELECT COUNT(*) FROM dim_producto) as productos,
    (SELECT COUNT(*) FROM dim_tiempo) as meses,
    (SELECT COUNT(*) FROM dim_cliente) as clientes,
    (SELECT COUNT(*) FROM dim_geografia) as geografias;

-- Debe mostrar estadísticas del cubo
SELECT 
    COUNT(*) as total_registros,
    COUNT(DISTINCT id_producto) as productos_distintos,
    COUNT(DISTINCT id_tiempo) as meses_distintos,
    COUNT(DISTINCT id_cliente) as clientes_distintos,
    SUM(total_venta) as ventas_totales,
    SUM(ganancia) as ganancia_total
FROM fact_ventas;
```

**Resultados esperados:**
- Total ventas: **1000+**
- Productos: **10**
- Meses: **10**
- Clientes: **10**
- Geografías: **10**
- Ventas totales: **~$10,000,000+**

---

## 🔧 Solución de Problemas

### Error: "psql no se reconoce como comando"

**Solución**: PostgreSQL no está en el PATH. Usar pgAdmin (Opción 1) o navegar manualmente a la carpeta bin de PostgreSQL.

### Error: "password authentication failed"

**Solución**: 
1. Verificar que el usuario sea `postgresql` (no `postgres`)
2. Verificar que la password sea `laza`
3. Si no funciona, cambiar la password en pgAdmin:
   - Click derecho en "Login/Group Roles" → "postgresql"
   - Pestaña "Definition"
   - Cambiar password a `laza`

### Error: "database already exists"

**Solución**: Eliminar la base de datos existente primero:
```sql
DROP DATABASE IF EXISTS "bd4-2025";
```

### Error: "relation does not exist"

**Solución**: El script no se ejecutó completamente. Volver a ejecutar desde el principio.

---

## 📞 Soporte

Si tienes problemas:

1. **Verificar que PostgreSQL esté corriendo**:
   - Abrir "Servicios" de Windows (Win + R → `services.msc`)
   - Buscar "postgresql-x64-[versión]"
   - Debe estar en estado "En ejecución"

2. **Verificar la versión de PostgreSQL**:
   ```powershell
   .\psql.exe --version
   ```

3. **Ver logs de PostgreSQL**:
   - Ubicación típica: `C:\Program Files\PostgreSQL\[versión]\data\log\`

---

## 🎉 ¡Listo para usar!

Una vez ejecutado exitosamente, la base de datos **bd4-2025** estará lista para usar con **Cubo Vision**.

**Conexión en Cubo Vision:**
- Host: `localhost`
- Puerto: `5432`
- Usuario: `postgresql`
- Password: `laza`
- Database: `bd4-2025`
