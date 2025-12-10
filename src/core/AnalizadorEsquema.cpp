#include "AnalizadorEsquema.h"
#include "GestorBaseDatos.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

AnalizadorEsquema::AnalizadorEsquema(QObject *parent) : QObject(parent) {}

void AnalizadorEsquema::analizar() {
  emit progreso(0, "Iniciando análisis de esquema...");

  m_tablas.clear();
  m_estadisticas.clear();
  m_relacionesFK.clear();
  m_tiposDatos.clear();

  if (!GestorBaseDatos::instancia().estaConectado()) {
    qWarning() << "No hay conexión a base de datos.";
    emit analisisCompletado();
    return;
  }

  emit progreso(10, "Analizando tablas del esquema...");
  analizarTablas();

  emit progreso(40, "Detectando tipos de datos...");
  analizarTiposDatos();

  emit progreso(60, "Buscando relaciones (Foreign Keys)...");
  detectarRelaciones();

  emit progreso(80, "Calculando estadísticas generales...");
  calcularEstadisticas();

  emit progreso(100, "✅ Análisis completado.");
  emit analisisCompletado();
}

void AnalizadorEsquema::analizarTablas() {
  QSqlQuery q(GestorBaseDatos::instancia().baseDatos());

  // Consulta avanzada a pg_catalog para obtener información detallada
  QString sql = R"(
    SELECT 
      t.relname AS nombre_tabla,
      COALESCE(s.n_live_tup, 0) AS filas_estimadas,
      pg_size_pretty(pg_total_relation_size(t.oid)) AS tamano_total,
      pg_total_relation_size(t.oid) AS tamano_bytes,
      (SELECT COUNT(*) FROM information_schema.columns c 
       WHERE c.table_name = t.relname AND c.table_schema = 'public') AS num_columnas,
      COALESCE(s.last_analyze, s.last_autoanalyze) AS ultimo_analisis,
      obj_description(t.oid, 'pg_class') AS descripcion
    FROM pg_class t
    JOIN pg_namespace n ON n.oid = t.relnamespace
    LEFT JOIN pg_stat_user_tables s ON s.relname = t.relname
    WHERE t.relkind = 'r' 
      AND n.nspname = 'public'
    ORDER BY tamano_bytes DESC;
  )";

  if (q.exec(sql)) {
    while (q.next()) {
      InfoTabla info;
      info.nombre = q.value("nombre_tabla").toString();
      info.filaEstimada = q.value("filas_estimadas").toLongLong();
      info.tamano = q.value("tamano_total").toString();
      info.numColumnas = q.value("num_columnas").toInt();
      info.tamanoBytes = q.value("tamano_bytes").toLongLong();
      info.descripcion = q.value("descripcion").toString();

      m_tablas.append(info);
      qDebug() << "📊 Tabla detectada:" << info.nombre
               << "| Filas:" << info.filaEstimada << "| Tamaño:" << info.tamano;
    }
  } else {
    qCritical() << "Error analizando tablas:" << q.lastError().text();
  }
}

void AnalizadorEsquema::analizarTiposDatos() {
  QSqlQuery q(GestorBaseDatos::instancia().baseDatos());

  QString sql = R"(
    SELECT 
      data_type,
      COUNT(*) AS cantidad
    FROM information_schema.columns
    WHERE table_schema = 'public'
    GROUP BY data_type
    ORDER BY cantidad DESC;
  )";

  if (q.exec(sql)) {
    while (q.next()) {
      QString tipo = q.value("data_type").toString();
      int cantidad = q.value("cantidad").toInt();
      m_tiposDatos[tipo] = cantidad;
    }
  }
}

void AnalizadorEsquema::detectarRelaciones() {
  QSqlQuery q(GestorBaseDatos::instancia().baseDatos());

  // Consulta avanzada para detectar Foreign Keys
  QString sql = R"(
    SELECT
      tc.table_name AS tabla_origen,
      kcu.column_name AS columna_origen,
      ccu.table_name AS tabla_destino,
      ccu.column_name AS columna_destino,
      tc.constraint_name AS nombre_constraint
    FROM information_schema.table_constraints tc
    JOIN information_schema.key_column_usage kcu 
      ON tc.constraint_name = kcu.constraint_name
      AND tc.table_schema = kcu.table_schema
    JOIN information_schema.constraint_column_usage ccu 
      ON ccu.constraint_name = tc.constraint_name
      AND ccu.table_schema = tc.table_schema
    WHERE tc.constraint_type = 'FOREIGN KEY'
      AND tc.table_schema = 'public';
  )";

  if (q.exec(sql)) {
    while (q.next()) {
      InfoRelacionFK rel;
      rel.tablaOrigen = q.value("tabla_origen").toString();
      rel.columnaOrigen = q.value("columna_origen").toString();
      rel.tablaDestino = q.value("tabla_destino").toString();
      rel.columnaDestino = q.value("columna_destino").toString();
      rel.nombreConstraint = q.value("nombre_constraint").toString();

      m_relacionesFK.append(rel);
      qDebug() << "🔗 FK detectada:" << rel.tablaOrigen << "."
               << rel.columnaOrigen << "->" << rel.tablaDestino << "."
               << rel.columnaDestino;
    }
  } else {
    qCritical() << "Error detectando FKs:" << q.lastError().text();
  }
}

void AnalizadorEsquema::calcularEstadisticas() {
  long long totalFilas = 0;
  long long totalBytes = 0;
  int totalColumnas = 0;

  for (const auto &tabla : m_tablas) {
    totalFilas += tabla.filaEstimada;
    totalBytes += tabla.tamanoBytes;
    totalColumnas += tabla.numColumnas;
  }

  m_estadisticas["total_tablas"] = m_tablas.size();
  m_estadisticas["total_filas"] = totalFilas;
  m_estadisticas["total_bytes"] = totalBytes;
  m_estadisticas["total_columnas"] = totalColumnas;
  m_estadisticas["total_relaciones"] = m_relacionesFK.size();
  m_estadisticas["tipos_datos_unicos"] = m_tiposDatos.size();

  // Detectar posibles tablas de hechos (muchas filas, muchas FKs)
  QStringList posiblesFacts;
  for (const auto &tabla : m_tablas) {
    int fksCount = 0;
    for (const auto &fk : m_relacionesFK) {
      if (fk.tablaOrigen == tabla.nombre)
        fksCount++;
    }
    // Heurística: tabla de hechos tiene muchas filas y varias FKs
    if (tabla.filaEstimada > 1000 && fksCount >= 2) {
      posiblesFacts.append(tabla.nombre);
    }
  }
  m_estadisticas["posibles_tablas_hechos"] = posiblesFacts;

  // Detectar posibles dimensiones (pocas filas, son referenciadas por FKs)
  QStringList posiblesDims;
  QMap<QString, int> referencias;
  for (const auto &fk : m_relacionesFK) {
    referencias[fk.tablaDestino]++;
  }
  for (const auto &tabla : m_tablas) {
    if (referencias.contains(tabla.nombre) && tabla.filaEstimada < 10000) {
      posiblesDims.append(tabla.nombre);
    }
  }
  m_estadisticas["posibles_dimensiones"] = posiblesDims;

  qDebug() << "📈 Estadísticas:"
           << "Tablas=" << m_tablas.size() << "| Filas=" << totalFilas
           << "| FKs=" << m_relacionesFK.size();
}

QVector<InfoTabla> AnalizadorEsquema::obtenerTablas() const { return m_tablas; }

QVector<InfoRelacionFK> AnalizadorEsquema::obtenerRelaciones() const {
  return m_relacionesFK;
}

QMap<QString, int> AnalizadorEsquema::obtenerTiposDatos() const {
  return m_tiposDatos;
}

QVariantMap AnalizadorEsquema::obtenerEstadisticasGeneral() const {
  return m_estadisticas;
}
