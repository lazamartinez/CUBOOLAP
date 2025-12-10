# Lista de Mejoras Futuras - Cubo Vision

## Prioridad Alta

### Funcionalidad
- [ ] Drag-drop real para dimensiones y medidas en Fase 2
- [ ] Consultas MDX reales contra el cubo OLAP
- [ ] Exportacion PDF funcional con QPdfWriter
- [ ] Exportacion Excel con formato .xlsx
- [ ] Guardado de modelos OLAP en archivos

### Visualizacion
- [ ] Animaciones de transicion entre fases
- [ ] Graficos de lineas temporales en Dashboard
- [ ] Mapa de calor 2D como alternativa al 3D
- [ ] Modo oscuro como opcion de usuario

## Prioridad Media

### Performance
- [ ] Carga lazy de datos grandes (paginacion)
- [ ] Cache de consultas frecuentes
- [ ] Indices automaticos en PostgreSQL
- [ ] Compresion de datos en memoria

### UX/UI
- [ ] Tutoriales interactivos en cada fase
- [ ] Atajos de teclado globales
- [ ] Panel de preferencias de usuario
- [ ] Historial de consultas ejecutadas
- [ ] Favoritos y bookmarks de vistas

### Datos
- [ ] Conexion a multiples bases de datos
- [ ] Soporte para MySQL, SQLite, Oracle
- [ ] Importacion desde CSV/Excel
- [ ] Scheduling de actualizaciones

## Prioridad Baja

### Colaboracion
- [ ] Compartir dashboards via URL
- [ ] Exportar como imagen PNG/SVG
- [ ] Comentarios en celdas del cubo

### Avanzado
- [ ] Machine Learning para deteccion de anomalias
- [ ] Predicciones temporales
- [ ] Alertas automaticas por umbral
- [ ] API REST para integraciones

## Bugs Conocidos
- Ninguno reportado actualmente

## Notas de Desarrollo
- Framework: Qt 6.5+ con C++20
- Base de datos: PostgreSQL 14+
- Compilador: MinGW 13.1 / MSVC 2022
