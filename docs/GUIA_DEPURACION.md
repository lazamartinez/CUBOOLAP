# 🔍 Guía de Depuración - Logs de Operaciones OLAP

## 📋 Sistema de Logging Implementado

Todas las operaciones OLAP ahora tienen **logging detallado** y **manejo de excepciones** que previene que la aplicación se cierre.

### ✅ Características

1. **Try-Catch en todas las operaciones**: Captura excepciones sin cerrar la aplicación
2. **Logs detallados en consola**: Muestra cada paso de la operación
3. **Diálogos de error**: Muestra mensajes amigables al usuario
4. **Toast notifications**: Feedback visual de errores

## 🖥️ Cómo Ver los Logs

### Opción 1: Ejecutar desde Terminal (Recomendado)

```powershell
# Navegar a la carpeta del ejecutable
cd "c:\Proyectos Facu\OLAPBD2025\build\Debug"

# Ejecutar la aplicación
.\CuboVision.exe
```

**Resultado**: Verás todos los logs en tiempo real en la terminal.

### Opción 2: Usar Qt Creator

1. Abrir el proyecto en Qt Creator
2. Hacer clic en el botón "Run" (▶️)
3. Los logs aparecerán en el panel "Application Output" (abajo)

### Opción 3: Usar DebugView (Windows)

1. Descargar [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) de Microsoft
2. Ejecutar DebugView como Administrador
3. Ejecutar CuboVision.exe
4. Los logs aparecerán en DebugView

## 📊 Ejemplo de Logs de Slice

Cuando ejecutes la operación Slice, verás algo como:

```
========== OPERACION SLICE INICIADA ==========
DialogoFiltros creado correctamente
Dimensiones disponibles: ("Producto", "Tiempo")
Cargando valores iniciales para: "Producto"
Valores obtenidos: 10
Mostrando dialogo...
```

Si hay un error:

```
========== ERROR EN SLICE ==========
Excepcion std::exception: Access violation reading location 0x00000000
====================================
```

## 🐛 Qué Hacer si Hay un Error

### 1. Capturar el Log Completo

Cuando veas el error en el diálogo:

1. **NO cerrar la aplicación** - El error ya fue manejado
2. **Copiar el mensaje del diálogo**
3. **Ir a la terminal** donde ejecutaste la aplicación
4. **Copiar todo el log** desde "========== OPERACION SLICE INICIADA ==========" hasta el final

### 2. Información Útil para Reportar

Incluye:
- ✅ Log completo de la operación
- ✅ Qué operación estabas ejecutando (Slice, Dice, etc.)
- ✅ Qué valores seleccionaste
- ✅ Mensaje de error del diálogo

### 3. Ejemplo de Reporte

```
OPERACIÓN: Slice
DIMENSIÓN SELECCIONADA: Producto
VALORES SELECCIONADOS: Laptop Pro, Mouse

LOG:
========== OPERACION SLICE INICIADA ==========
DialogoFiltros creado correctamente
Dimensiones disponibles: ("Producto", "Tiempo")
...
========== ERROR EN SLICE ==========
Excepcion std::exception: ...
====================================
```

## 🔧 Operaciones con Logging

Todas estas operaciones ahora tienen logging completo:

| Operación         | Estado         | Logs      |
| ----------------- | -------------- | --------- |
| **Slice**         | ✅ Implementado | Detallado |
| **Dice**          | ⏳ Pendiente    | -         |
| **Drill Down**    | ⏳ Pendiente    | -         |
| **Roll Up**       | ⏳ Pendiente    | -         |
| **Pivot**         | ⏳ Pendiente    | -         |
| **Ranking**       | ⏳ Pendiente    | -         |
| **Drill Through** | ⏳ Pendiente    | -         |
| **Reset**         | ⏳ Pendiente    | -         |

## 📝 Tipos de Logs

### `qDebug()` - Información Normal
```
DialogoFiltros creado correctamente
Dimensiones disponibles: ("Producto", "Tiempo")
```

### `qCritical()` - Errores
```
========== ERROR EN SLICE ==========
Excepcion std::exception: Access violation
====================================
```

## 🚀 Próximos Pasos

1. **Ejecutar la aplicación desde terminal**
2. **Probar la operación Slice**
3. **Si hay error, copiar el log completo**
4. **Reportar el error con el log**

## 💡 Tips

- **Mantén la terminal abierta** mientras usas la aplicación
- **Los logs son muy detallados** - muestran cada paso
- **Si la aplicación se cierra**, el error NO fue capturado (reportar inmediatamente)
- **Si aparece un diálogo de error**, el error FUE capturado (la app sigue funcionando)

## 📞 Soporte

Si encuentras un error:

1. Copia el log completo
2. Copia el mensaje del diálogo de error
3. Describe qué estabas haciendo
4. Comparte toda la información

Esto permitirá identificar y corregir el problema rápidamente.
