#include "VisorOlap.h"
#include "Estilos.h"
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QToolTip>
#include <QtMath>
#include <algorithm>

// ============================================================================
// CONSTRUCTOR Y DESTRUCTOR
// ============================================================================

VisorOlap::VisorOlap(QWidget *parent)
    : QWidget(parent), m_zoom(1.0f), m_panOffset(0, 0), m_arrastrando(false),
      m_celdaHover(-1), m_valorMax(1.0), m_valorMin(0.0), m_tamanoBase(25.0f),
      m_escalaAltura(1.5f), m_espaciado(32.0f) {

  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setMinimumSize(800, 600);

  // Cargar datos al iniciar
  cargarCuboDummy();
}

VisorOlap::~VisorOlap() {}

// ============================================================================
// CARGA DE DATOS
// ============================================================================

void VisorOlap::cargarCuboDummy() { cargarCuboReal(); }

void VisorOlap::cargarDatos(const QVector<CeldaCubo> &celdas) {
  m_celdas = celdas;
  update();
}

void VisorOlap::recargarCubo() { cargarCuboReal(); }

void VisorOlap::aplicarConfig(const ConfigVisualizacion &config) {
  m_config = config;
  update();
}

// ============================================================================
// HELPERS PARA QUERIES SQL
// ============================================================================

static QString execValue(QSqlDatabase &db, const QString &sql) {
  QSqlQuery q(db);
  if (q.exec(sql) && q.next())
    return q.value(0).toString();
  return QString();
}

static int execValueInt(QSqlDatabase &db, const QString &sql) {
  QSqlQuery q(db);
  if (q.exec(sql) && q.next())
    return q.value(0).toInt();
  return 0;
}

// ============================================================================
// DETECCIÓN AUTOMÁTICA DE ESQUEMA
// ============================================================================

QString VisorOlap::elegirMejorColumnaDimension(const QString &tabla) {
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return QString();

  QSqlQuery q(db);
  QString sqlCols = QString(R"(
        SELECT column_name, data_type 
        FROM information_schema.columns 
        WHERE table_name = '%1' AND table_schema = 'public'
        AND column_name NOT LIKE 'id_%' 
        AND column_name NOT LIKE '%_id' 
        AND column_name NOT LIKE '%uuid%'
        AND column_name != 'password'
    )")
                        .arg(tabla);

  QString bestCol;
  int bestScore = -1;

  if (q.exec(sqlCols)) {
    while (q.next()) {
      QString col = q.value(0).toString();
      QString type = q.value(1).toString();
      int score = 0;

      // CAMBIO: Priorizar columnas con MAYOR VARIEDAD
      // Subcategorías y detalles específicos tienen más variedad que categorías
      // amplias
      if (col.contains("subcategoria", Qt::CaseInsensitive))
        score += 100; // MÁS VARIEDAD
      else if (col.contains("marca", Qt::CaseInsensitive))
        score += 95;
      else if (col.contains("ciudad", Qt::CaseInsensitive))
        score += 90;
      else if (col.contains("nombre_producto", Qt::CaseInsensitive))
        score += 85;
      else if (col.contains("codigo", Qt::CaseInsensitive) &&
               col.contains("postal", Qt::CaseInsensitive))
        score += 80;
      else if (col.contains("categoria", Qt::CaseInsensitive))
        score += 70; // Menos prioridad (muy general)
      else if (col.contains("region", Qt::CaseInsensitive))
        score += 65;
      else if (col.contains("pais", Qt::CaseInsensitive))
        score += 60;
      else if (col.contains("tipo", Qt::CaseInsensitive))
        score += 55;
      else if (col.contains("segmento", Qt::CaseInsensitive))
        score += 75;
      else if (col.contains("mes", Qt::CaseInsensitive))
        score += 50; // Más variedad que año
      else if (col.contains("trimestre", Qt::CaseInsensitive))
        score += 45;
      else if (col.contains("anio", Qt::CaseInsensitive))
        score += 30; // Menos prioridad (poca variedad)
      else if (col.contains("nombre", Qt::CaseInsensitive))
        score += 10;

      if (type.contains("char") || type.contains("text"))
        score += 5;

      if (score > bestScore) {
        bestScore = score;
        bestCol = col;
      }
    }
  }

  if (bestCol.isEmpty()) {
    QString fallback = QString(R"(
            SELECT column_name FROM information_schema.columns 
            WHERE table_name = '%1' AND data_type IN ('character varying', 'text')
            AND column_name NOT LIKE 'id_%' LIMIT 1
       )")
                           .arg(tabla);
    return execValue(db, fallback);
  }

  qDebug() << "Tabla:" << tabla << "-> Mejor columna:" << bestCol
           << "(score:" << bestScore << ")";
  return bestCol;
}

QStringList VisorOlap::obtenerTopKeys(const QString &factTable,
                                      const QString &dimTable,
                                      const QString &dimCol,
                                      const QString &fkCol,
                                      const QString &pkCol, int limit) {
  QStringList keys;
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return keys;

  QString sql;
  if (factTable == dimTable) {
    // Dimensión degenerada
    sql =
        QString("SELECT %1 FROM %2 GROUP BY %1 ORDER BY COUNT(*) DESC LIMIT %3")
            .arg(dimCol, factTable)
            .arg(limit);
  } else {
    // Join normal
    sql = QString("SELECT d.%1 FROM %2 d JOIN %3 f ON d.%4 = f.%5 GROUP BY "
                  "d.%1 ORDER BY COUNT(*) DESC LIMIT %6")
              .arg(dimCol, dimTable, factTable, pkCol, fkCol)
              .arg(limit);
  }

  QSqlQuery q(db);
  qDebug() << "=== obtenerTopKeys DEBUG ===";
  qDebug() << "SQL:" << sql;
  qDebug() << "Limit:" << limit;

  if (q.exec(sql)) {
    int count = 0;
    while (q.next()) {
      QString val = q.value(0).toString();
      if (!val.isEmpty()) {
        keys << val;
        count++;
        qDebug() << "  Key" << count << ":" << val;
      }
    }
    qDebug() << "Total keys obtenidos:" << keys.size();
  } else {
    qDebug() << "ERROR en query:" << q.lastError().text();
  }

  return keys;
}

int VisorOlap::calcularCardinalidad(const QString &tabla,
                                    const QString &columna) {
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return 0;

  QString sql =
      QString("SELECT COUNT(DISTINCT %1) FROM %2").arg(columna, tabla);
  return execValueInt(db, sql);
}

DimensionInfo VisorOlap::analizarDimension(const QString &tablaHechos,
                                           const QString &fkCol,
                                           const QString &dimTable,
                                           const QString &pkCol) {
  DimensionInfo dim;
  dim.tabla = dimTable;
  dim.fkColumna = fkCol;
  dim.pkColumna = pkCol;
  dim.esDegenerada = false;

  // SOLUCIÓN: Usar directamente el PK (ID) porque tiene más variedad
  // Los datos descriptivos están muy concentrados (1 mes, 1 ciudad, etc.)
  dim.columna = pkCol; // Usar ID directamente

  qDebug() << "USANDO PK DIRECTAMENTE:" << pkCol << "para tabla" << dimTable;

  // Nombre de la dimensión (capitalizar nombre de tabla)
  dim.nombre = dimTable;
  if (dim.nombre.startsWith("dim_"))
    dim.nombre = dim.nombre.mid(4);
  dim.nombre[0] = dim.nombre[0].toUpper();

  // Calcular cardinalidad
  dim.cardinalidad = calcularCardinalidad(dimTable, dim.columna);

  // Obtener top valores (ahora serán IDs: 1, 2, 3, ...)
  dim.valores = obtenerTopKeys(tablaHechos, dimTable, dim.columna, fkCol, pkCol,
                               m_config.maxCeldasPorEje);

  return dim;
}

DimensionInfo VisorOlap::crearDimensionDegenerada(const QString &tablaHechos,
                                                  const QString &columna) {
  DimensionInfo dim;
  dim.tabla = tablaHechos;
  dim.columna = columna;
  dim.fkColumna = "";
  dim.pkColumna = "";
  dim.esDegenerada = true;

  // Nombre
  dim.nombre = columna;
  dim.nombre[0] = dim.nombre[0].toUpper();

  // Cardinalidad
  dim.cardinalidad = calcularCardinalidad(tablaHechos, columna);

  // Top valores
  dim.valores = obtenerTopKeys(tablaHechos, tablaHechos, columna, "", "",
                               m_config.maxCeldasPorEje);

  return dim;
}

QStringList VisorOlap::detectarMedidasDisponibles(const QString &tablaHechos) {
  QStringList medidas;
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return medidas;

  QString sql = QString(R"(
      SELECT column_name FROM information_schema.columns 
      WHERE table_name = '%1' AND table_schema = 'public'
      AND data_type IN ('integer', 'numeric', 'real', 'double precision', 'bigint', 'decimal')
      AND column_name NOT LIKE 'id_%'
      AND column_name NOT LIKE '%_id'
  )")
                    .arg(tablaHechos);

  QSqlQuery q(db);
  if (q.exec(sql)) {
    while (q.next()) {
      medidas << q.value(0).toString();
    }
  }

  return medidas;
}

QVector<DimensionInfo>
VisorOlap::detectarDimensionesDisponibles(const QString &tablaHechos) {
  QVector<DimensionInfo> dims;
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return dims;

  // Detectar foreign keys
  QString sqlFKs = QString(R"(
     SELECT kcu.column_name, ccu.table_name, ccu.column_name
     FROM information_schema.key_column_usage kcu
     JOIN information_schema.constraint_column_usage ccu ON kcu.constraint_name = ccu.constraint_name
     JOIN information_schema.table_constraints tc ON kcu.constraint_name = tc.constraint_name
     WHERE tc.constraint_type = 'FOREIGN KEY' AND kcu.table_name = '%1'
  )")
                       .arg(tablaHechos);

  QSqlQuery qFK(db);
  if (qFK.exec(sqlFKs)) {
    while (qFK.next()) {
      QString fkCol = qFK.value(0).toString();
      QString dimTable = qFK.value(1).toString();
      QString pkCol = qFK.value(2).toString();

      DimensionInfo dim =
          analizarDimension(tablaHechos, fkCol, dimTable, pkCol);
      dims.append(dim);
    }
  }

  // Fallback: dimensiones degeneradas
  if (dims.size() < 3) {
    QString sqlText = QString(R"(
          SELECT column_name FROM information_schema.columns 
          WHERE table_name = '%1' AND data_type LIKE '%char%' 
          AND column_name NOT LIKE 'id_%' 
          AND column_name NOT LIKE '%_id'
          LIMIT %2
      )")
                          .arg(tablaHechos)
                          .arg(3 - dims.size());

    QSqlQuery qT(db);
    if (qT.exec(sqlText)) {
      while (qT.next()) {
        QString col = qT.value(0).toString();
        DimensionInfo dim = crearDimensionDegenerada(tablaHechos, col);
        dims.append(dim);
      }
    }
  }

  return dims;
}

MetadataCubo VisorOlap::detectarEsquemaOptimo() {
  MetadataCubo metadata;
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return metadata;

  // 1. Detectar tabla de hechos (la más grande)
  QString sqlFact = R"(
      SELECT relname FROM pg_class c 
      JOIN pg_namespace n ON n.oid = c.relnamespace 
      WHERE c.relkind = 'r' AND n.nspname = 'public' 
      ORDER BY c.reltuples DESC LIMIT 1
  )";
  metadata.nombreTablaHechos = execValue(db, sqlFact);
  if (metadata.nombreTablaHechos.isEmpty())
    return metadata;

  // 2. Contar registros totales
  QString sqlCount =
      QString("SELECT COUNT(*) FROM %1").arg(metadata.nombreTablaHechos);
  metadata.totalRegistros = execValueInt(db, sqlCount);

  // === DIAGNÓSTICO: Verificar distribución real de datos ===
  qDebug() << "\n========== DIAGNÓSTICO DISTRIBUCIÓN DE DATOS ==========";
  QSqlQuery diagQ(db);

  // Contar distintos por cada FK
  QString sqlDiag = QString(R"(
    SELECT 
      (SELECT COUNT(DISTINCT id_producto) FROM %1) as productos,
      (SELECT COUNT(DISTINCT id_cliente) FROM %1) as clientes,
      (SELECT COUNT(DISTINCT id_tiempo) FROM %1) as tiempos,
      (SELECT COUNT(DISTINCT id_geografia) FROM %1) as geografias
  )")
                        .arg(metadata.nombreTablaHechos);

  if (diagQ.exec(sqlDiag) && diagQ.next()) {
    qDebug() << "Distintos id_producto en fact_ventas:"
             << diagQ.value(0).toInt();
    qDebug() << "Distintos id_cliente en fact_ventas:"
             << diagQ.value(1).toInt();
    qDebug() << "Distintos id_tiempo en fact_ventas:" << diagQ.value(2).toInt();
    qDebug() << "Distintos id_geografia en fact_ventas:"
             << diagQ.value(3).toInt();
  }

  // Mostrar top 5 productos
  QSqlQuery topQ(db);
  QString sqlTop = QString("SELECT id_producto, COUNT(*) as cnt FROM %1 GROUP "
                           "BY id_producto ORDER BY cnt DESC LIMIT 5")
                       .arg(metadata.nombreTablaHechos);
  if (topQ.exec(sqlTop)) {
    qDebug() << "TOP 5 productos por ventas:";
    while (topQ.next()) {
      qDebug() << "  id_producto:" << topQ.value(0).toInt()
               << "-> ventas:" << topQ.value(1).toInt();
    }
  }
  qDebug() << "========================================================\n";

  // 3. Detectar medidas disponibles
  QStringList medidas = detectarMedidasDisponibles(metadata.nombreTablaHechos);
  if (!medidas.isEmpty()) {
    metadata.medidaActual = medidas.first();
    metadata.medidaTipo = "SUM";
    metadata.medidaFormula = QString("SUM(%1)").arg(metadata.medidaActual);
  } else {
    metadata.medidaActual = "Cantidad";
    metadata.medidaTipo = "COUNT";
    metadata.medidaFormula = "COUNT(*)";
  }

  // 4. Detectar dimensiones
  QVector<DimensionInfo> dims =
      detectarDimensionesDisponibles(metadata.nombreTablaHechos);

  // Asegurar 3 dimensiones
  while (dims.size() < 3) {
    DimensionInfo dummy;
    dummy.nombre = "N/A";
    dummy.tabla = "";
    dummy.columna = "'N/A'";
    dummy.valores << "N/A";
    dummy.cardinalidad = 1;
    dims.append(dummy);
  }

  // Asignar a ejes - PRIORIZAR POR NOMBRE SEMÁNTICO
  std::sort(dims.begin(), dims.end(),
            [](const DimensionInfo &a, const DimensionInfo &b) {
              // Scoring semántico por nombre de dimensión
              auto getScore = [](const DimensionInfo &d) -> int {
                QString nombre = d.nombre.toLower();
                QString tabla = d.tabla.toLower();

                // Prioridad alta para dimensiones comunes
                if (nombre.contains("producto") || tabla.contains("producto"))
                  return 100;
                if (nombre.contains("cliente") || tabla.contains("cliente"))
                  return 95;
                if (nombre.contains("geografia") || tabla.contains("geografia"))
                  return 90;
                if (nombre.contains("tiempo") || tabla.contains("tiempo"))
                  return 85;
                if (nombre.contains("categoria"))
                  return 80;
                if (nombre.contains("region"))
                  return 75;
                if (nombre.contains("pais"))
                  return 70;

                // Penalizar dimensiones con cardinalidad muy baja
                if (d.cardinalidad < 3)
                  return 0;

                // Score por cardinalidad (preferir entre 5-15)
                return 50 - qAbs(d.cardinalidad - 10);
              };

              return getScore(a) > getScore(b);
            });

  metadata.dimensionX = dims[0];
  metadata.dimensionZ = dims.size() > 1 ? dims[1] : dims[0];
  metadata.dimensionY.nombre = metadata.medidaActual;

  return metadata;
}

// ============================================================================
// CARGA DEL CUBO REAL
// ============================================================================

void VisorOlap::cargarCuboReal() {
  m_celdas.clear();
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return;

  // Detectar esquema óptimo
  m_metadata = detectarEsquemaOptimo();
  if (m_metadata.nombreTablaHechos.isEmpty())
    return;

  // Construir query principal
  QString querySql = "SELECT ";

  // Columnas de dimensiones
  QString colX = m_metadata.dimensionX.esDegenerada
                     ? ("f." + m_metadata.dimensionX.columna)
                     : ("d0." + m_metadata.dimensionX.columna);
  QString colZ = m_metadata.dimensionZ.esDegenerada
                     ? ("f." + m_metadata.dimensionZ.columna)
                     : ("d1." + m_metadata.dimensionZ.columna);

  querySql += colX + ", " + colZ + ", " + m_metadata.medidaFormula;
  querySql += " FROM " + m_metadata.nombreTablaHechos + " f ";

  // Joins
  if (!m_metadata.dimensionX.esDegenerada) {
    querySql += QString(" JOIN %1 d0 ON f.%2 = d0.%3 ")
                    .arg(m_metadata.dimensionX.tabla)
                    .arg(m_metadata.dimensionX.fkColumna)
                    .arg(m_metadata.dimensionX.pkColumna);
  }
  if (!m_metadata.dimensionZ.esDegenerada) {
    querySql += QString(" JOIN %1 d1 ON f.%2 = d1.%3 ")
                    .arg(m_metadata.dimensionZ.tabla)
                    .arg(m_metadata.dimensionZ.fkColumna)
                    .arg(m_metadata.dimensionZ.pkColumna);
  }

  // Filtros WHERE para Top N
  QStringList whereList;
  if (!m_metadata.dimensionX.valores.isEmpty()) {
    QStringList quotedKeys;
    for (const auto &k : m_metadata.dimensionX.valores)
      quotedKeys << "'" + k + "'";
    whereList << QString("%1 IN (%2)").arg(colX, quotedKeys.join(","));
  }
  if (!m_metadata.dimensionZ.valores.isEmpty()) {
    QStringList quotedKeys;
    for (const auto &k : m_metadata.dimensionZ.valores)
      quotedKeys << "'" + k + "'";
    whereList << QString("%1 IN (%2)").arg(colZ, quotedKeys.join(","));
  }

  if (!whereList.isEmpty()) {
    querySql += " WHERE " + whereList.join(" AND ");
  }

  querySql += " GROUP BY 1, 2";

  // Ejecutar query
  QSqlQuery query(db);
  if (!query.exec(querySql))
    return;

  // Mapear resultados
  QMap<QString, double> dataMap; // Key: "dimX|dimZ"
  m_valorMin = 9e15;
  m_valorMax = 0;

  while (query.next()) {
    QString k = query.value(0).toString() + "|" + query.value(1).toString();
    double v = query.value(2).toDouble();
    dataMap[k] = v;
    if (v > m_valorMax)
      m_valorMax = v;
    if (v < m_valorMin)
      m_valorMin = v;
  }

  // Construir grid completo - MOSTRAR TODAS LAS CELDAS (incluso con valor 0)
  QVector<CeldaCubo> nuevasCeldas;
  QStringList keysX = m_metadata.dimensionX.valores;
  QStringList keysZ = m_metadata.dimensionZ.valores;

  m_metadata.totalCeldas = keysX.size() * keysZ.size();
  m_metadata.celdasConDatos = 0;

  for (int x = 0; x < keysX.size(); x++) {
    for (int z = 0; z < keysZ.size(); z++) {
      QString key = keysX[x] + "|" + keysZ[z];
      double val = dataMap.value(key, 0.0);

      // CAMBIO CRÍTICO: Mostrar TODAS las celdas, incluso con valor 0
      CeldaCubo celda;
      celda.dimX = x;
      celda.dimY = 0;
      celda.dimZ = z;
      celda.nombreDimX = keysX[x];
      celda.nombreDimY = m_metadata.medidaActual;
      celda.nombreDimZ = keysZ[z];
      celda.valor = val;
      celda.valorSecundario = 0;
      celda.etiqueta =
          QString("%1\n%2\n%3").arg(keysX[x], keysZ[z]).arg(val, 0, 'f', 0);
      nuevasCeldas.append(celda);

      if (val > 0) {
        m_metadata.celdasConDatos++;
      }
    }
  }

  m_metadata.valorMinimo = m_valorMin;
  m_metadata.valorMaximo = m_valorMax;

  cargarDatos(nuevasCeldas);
  emit metadataCambiada(m_metadata);
}

// ============================================================================
// SELECCIÓN
// ============================================================================

void VisorOlap::limpiarSeleccion() {
  for (auto &c : m_celdas)
    c.seleccionada = false;
  emit seleccionCambiada(0);
  update();
}

QVector<CeldaCubo> VisorOlap::obtenerSeleccion() const {
  QVector<CeldaCubo> sel;
  for (const auto &c : m_celdas)
    if (c.seleccionada)
      sel.append(c);
  return sel;
}

// ============================================================================
// RENDERIZADO PRINCIPAL
// ============================================================================

void VisorOlap::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  dibujarFondo(painter);

  // Transformación global
  painter.save();
  painter.translate(width() / 2 + m_panOffset.x(),
                    height() / 2 + m_panOffset.y());
  painter.scale(m_zoom, m_zoom);

  // Elementos 3D
  if (m_config.mostrarPlanos)
    dibujarPlanosReferencia(painter);
  if (m_config.mostrarGrid)
    dibujarGridIsometrico(painter);

  dibujarEjesConEtiquetas(painter);

  // Ordenar celdas por profundidad (painter's algorithm)
  QVector<int> indices(m_celdas.size());
  for (int i = 0; i < m_celdas.size(); i++)
    indices[i] = i;

  std::sort(indices.begin(), indices.end(), [this](int a, int b) {
    const CeldaCubo &ca = m_celdas[a];
    const CeldaCubo &cb = m_celdas[b];
    return (ca.dimX + ca.dimZ - ca.dimY) < (cb.dimX + cb.dimZ - cb.dimY);
  });

  // Dibujar celdas ordenadas
  for (int idx : indices) {
    bool hover = (idx == m_celdaHover);
    bool sel = m_celdas[idx].seleccionada;
    dibujarPrisma(painter, m_celdas[idx], hover, sel);
  }

  painter.restore();

  // UI superpuesta
  dibujarPanelControl(painter);
  dibujarLeyenda(painter);

  // Tooltip
  if (m_celdaHover >= 0 && m_celdaHover < m_celdas.size()) {
    dibujarTooltip(painter, m_celdas[m_celdaHover],
                   mapFromGlobal(QCursor::pos()));
  }
}

// ============================================================================
// RENDERIZADO: FONDO Y GRID
// ============================================================================

void VisorOlap::dibujarFondo(QPainter &painter) {
  QLinearGradient grad(0, 0, 0, height());
  grad.setColorAt(0, m_config.colorFondo1);
  grad.setColorAt(1, m_config.colorFondo2);
  painter.fillRect(rect(), grad);
}

void VisorOlap::dibujarGridIsometrico(QPainter &painter) {
  if (!m_config.mostrarGrid)
    return;

  painter.save();
  QColor gridColor = m_config.colorGrid;
  gridColor.setAlphaF(m_config.opacidadGrid);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  int maxX = m_metadata.dimensionX.valores.size();
  int maxZ = m_metadata.dimensionZ.valores.size();

  // Líneas paralelas al eje X
  for (int z = 0; z <= maxZ; z++) {
    QPointF p1 = proyectarIso(0, 0, z * m_espaciado);
    QPointF p2 = proyectarIso(maxX * m_espaciado, 0, z * m_espaciado);
    painter.drawLine(p1, p2);
  }

  // Líneas paralelas al eje Z
  for (int x = 0; x <= maxX; x++) {
    QPointF p1 = proyectarIso(x * m_espaciado, 0, 0);
    QPointF p2 = proyectarIso(x * m_espaciado, 0, maxZ * m_espaciado);
    painter.drawLine(p1, p2);
  }

  painter.restore();
}

void VisorOlap::dibujarPlanosReferencia(QPainter &painter) {
  if (!m_config.mostrarPlanos)
    return;

  painter.save();
  QColor planoColor = QColor(150, 150, 150);
  planoColor.setAlphaF(m_config.opacidadPlanos);
  painter.setBrush(planoColor);
  painter.setPen(Qt::NoPen);

  int maxX = m_metadata.dimensionX.valores.size();
  int maxZ = m_metadata.dimensionZ.valores.size();

  // Plano base XZ
  QPolygonF planoBase;
  planoBase << proyectarIso(0, 0, 0) << proyectarIso(maxX * m_espaciado, 0, 0)
            << proyectarIso(maxX * m_espaciado, 0, maxZ * m_espaciado)
            << proyectarIso(0, 0, maxZ * m_espaciado);
  painter.drawPolygon(planoBase);

  painter.restore();
}

// ============================================================================
// RENDERIZADO: EJES Y ETIQUETAS
// ============================================================================

void VisorOlap::dibujarEjesConEtiquetas(QPainter &painter) {
  painter.save();

  // Ejes principales
  dibujarEjeConFlecha(painter, EjeX, m_metadata.dimensionX.nombre);
  dibujarEjeConFlecha(painter, EjeZ, m_metadata.dimensionZ.nombre);
  dibujarEjeConFlecha(painter, EjeY, m_metadata.medidaActual);

  // Etiquetas de valores
  dibujarEtiquetasEje(painter, EjeX, m_metadata.dimensionX.valores);
  dibujarEtiquetasEje(painter, EjeZ, m_metadata.dimensionZ.valores);
  dibujarEscalaEje(painter, m_metadata.valorMinimo, m_metadata.valorMaximo);

  painter.restore();
}

void VisorOlap::dibujarEjeConFlecha(QPainter &painter, int tipoEje,
                                    const QString &nombre) {
  QPointF origen = proyectarIso(0, 0, 0);
  QPointF destino;
  QColor color;

  switch (tipoEje) {
  case EjeX:
    destino = proyectarIso(350, 0, 0);
    color = m_config.colorEjeX;
    break;
  case EjeZ:
    destino = proyectarIso(0, 0, 350);
    color = m_config.colorEjeZ;
    break;
  case EjeY:
    destino = proyectarIso(0, 300, 0);
    color = m_config.colorEjeY;
    break;
  }

  // Línea del eje
  painter.setPen(QPen(color, 2.5));
  painter.drawLine(origen, destino);

  // Flecha
  QPointF dir = destino - origen;
  double angle = qAtan2(dir.y(), dir.x());
  QPointF arrowP1 = destino - QPointF(qCos(angle + M_PI / 6) * 12,
                                      qSin(angle + M_PI / 6) * 12);
  QPointF arrowP2 = destino - QPointF(qCos(angle - M_PI / 6) * 12,
                                      qSin(angle - M_PI / 6) * 12);
  painter.setBrush(color);
  QPolygonF arrow;
  arrow << destino << arrowP1 << arrowP2;
  painter.drawPolygon(arrow);

  // Etiqueta del eje
  QFont font("Segoe UI", 10, QFont::Bold);
  painter.setFont(font);
  painter.setPen(color);
  painter.drawText(destino + QPointF(10, 5), nombre.toUpper());
}

void VisorOlap::dibujarEtiquetasEje(QPainter &painter, int tipoEje,
                                    const QStringList &valores) {
  painter.save();
  QFont font("Segoe UI", 7);
  painter.setFont(font);
  painter.setPen(QColor("#475569"));

  for (int i = 0; i < valores.size(); i++) {
    QPointF pos;
    QString texto = valores[i];
    if (texto.length() > 12)
      texto = texto.left(10) + "..";

    switch (tipoEje) {
    case EjeX:
      pos = proyectarIso((i + 0.5) * m_espaciado, 0, -15);
      break;
    case EjeZ:
      pos = proyectarIso(-15, 0, (i + 0.5) * m_espaciado);
      break;
    }

    painter.drawText(pos, texto);
  }

  painter.restore();
}

void VisorOlap::dibujarEscalaEje(QPainter &painter, double minVal,
                                 double maxVal) {
  painter.save();
  QFont font("Segoe UI", 7);
  painter.setFont(font);
  painter.setPen(m_config.colorEjeY);

  // Dibujar 5 marcas en el eje Y
  for (int i = 0; i <= 4; i++) {
    double valor = minVal + (maxVal - minVal) * i / 4.0;
    float altura = calcularAltura(valor);
    QPointF pos = proyectarIso(-20, altura, 0);
    painter.drawText(pos, QString::number(valor, 'f', 0));
  }

  painter.restore();
}

// ============================================================================
// RENDERIZADO: PRISMAS
// ============================================================================

float VisorOlap::calcularAltura(double valor) {
  if (m_valorMax <= m_valorMin)
    return 5.0f;

  // Para valores en 0, mostrar altura mínima visible
  if (valor <= 0)
    return 3.0f;

  // Normalizar valor entre 0 y 1
  double t = (valor - m_valorMin) / (m_valorMax - m_valorMin);

  // Escala logarítmica para mejor distribución visual
  float altura = 5.0f + (float)(qLn(1.0 + t * 10.0) * m_escalaAltura * 20.0f);

  // Limitar altura máxima
  if (altura > 150)
    altura = 150;

  return altura;
}

QPointF VisorOlap::calcularPosicionBase(const CeldaCubo &celda) {
  float x = celda.dimX * m_espaciado;
  float z = celda.dimZ * m_espaciado;
  return proyectarIso(x, 0, z);
}

void VisorOlap::dibujarPrisma(QPainter &painter, const CeldaCubo &celda,
                              bool hover, bool seleccionada) {
  float x = celda.dimX * m_espaciado;
  float z = celda.dimZ * m_espaciado;
  float altura = calcularAltura(celda.valor);
  float w = m_tamanoBase;
  float d = m_tamanoBase;

  QColor colorBase = obtenerColorGradiente(celda.valor, m_valorMin, m_valorMax);
  if (hover)
    colorBase = colorBase.lighter(130);
  if (seleccionada)
    colorBase = QColor("#2563eb");

  // Sombra
  if (m_config.mostrarSombras) {
    dibujarSombraPrisma(painter, proyectarIso(x, 0, z), altura, w);
  }

  // Vértices
  QPointF b1 = proyectarIso(x + w / 2, 0, z + d / 2);
  QPointF b2 = proyectarIso(x + w / 2, 0, z - d / 2);
  QPointF b3 = proyectarIso(x - w / 2, 0, z - d / 2);
  QPointF b4 = proyectarIso(x - w / 2, 0, z + d / 2);

  QPointF t1 = proyectarIso(x + w / 2, altura, z + d / 2);
  QPointF t2 = proyectarIso(x + w / 2, altura, z - d / 2);
  QPointF t3 = proyectarIso(x - w / 2, altura, z - d / 2);
  QPointF t4 = proyectarIso(x - w / 2, altura, z + d / 2);

  // Cara superior
  QPolygonF polyTop;
  polyTop << t1 << t2 << t3 << t4;
  dibujarCaraSuperior(painter, polyTop, colorBase.lighter(115));

  // Cara frontal derecha
  QPolygonF polyFrontal;
  polyFrontal << b1 << b2 << t2 << t1;
  QLinearGradient gradFrontal(b1, t1);
  gradFrontal.setColorAt(0, colorBase);
  gradFrontal.setColorAt(1, colorBase.darker(110));
  dibujarCaraFrontal(painter, polyFrontal, gradFrontal);

  // Cara lateral izquierda
  QPolygonF polyLateral;
  polyLateral << b4 << b1 << t1 << t4;
  dibujarCaraLateral(painter, polyLateral, colorBase.darker(125));

  // Guardar polígonos para detección
  const_cast<CeldaCubo &>(celda).poligonoTop = polyTop;
  const_cast<CeldaCubo &>(celda).poligonoFrontal = polyFrontal;
  const_cast<CeldaCubo &>(celda).poligonoLateral = polyLateral;
  const_cast<CeldaCubo &>(celda).areaPantalla =
      polyTop.boundingRect().united(polyFrontal.boundingRect());

  // Borde de selección
  if (seleccionada || hover) {
    dibujarBordeDestacado(painter, celda,
                          hover ? QColor("#fbbf24") : QColor("#06b6d4"));
  }
}

void VisorOlap::dibujarSombraPrisma(QPainter &painter, const QPointF &base,
                                    float altura, float ancho) {
  Q_UNUSED(altura);
  painter.save();
  QRadialGradient grad(base, ancho * 0.8);
  grad.setColorAt(0, QColor(0, 0, 0, 40));
  grad.setColorAt(1, QColor(0, 0, 0, 0));
  painter.setBrush(grad);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(base, ancho * 0.7, ancho * 0.4);
  painter.restore();
}

void VisorOlap::dibujarCaraSuperior(QPainter &painter, const QPolygonF &poly,
                                    const QColor &color) {
  painter.setBrush(color);
  // Borde más grueso y oscuro para efecto de cubito
  painter.setPen(QPen(color.darker(150), 1.5, Qt::SolidLine, Qt::SquareCap,
                      Qt::MiterJoin));
  painter.drawPolygon(poly);
}

void VisorOlap::dibujarCaraFrontal(QPainter &painter, const QPolygonF &poly,
                                   const QBrush &brush) {
  painter.setBrush(brush);
  // Borde más grueso para efecto 3D más definido
  painter.setPen(QPen(brush.color().darker(160), 1.5, Qt::SolidLine,
                      Qt::SquareCap, Qt::MiterJoin));
  painter.drawPolygon(poly);
}

void VisorOlap::dibujarCaraLateral(QPainter &painter, const QPolygonF &poly,
                                   const QColor &color) {
  painter.setBrush(color);
  // Borde más grueso para efecto 3D más definido
  painter.setPen(QPen(color.darker(170), 1.5, Qt::SolidLine, Qt::SquareCap,
                      Qt::MiterJoin));
  painter.drawPolygon(poly);
}

void VisorOlap::dibujarBordeDestacado(QPainter &painter, const CeldaCubo &celda,
                                      const QColor &color) {
  painter.save();
  painter.setPen(QPen(color, 3));
  painter.setBrush(Qt::NoBrush);
  painter.drawPolygon(celda.poligonoTop);
  painter.drawPolygon(celda.poligonoFrontal);
  painter.drawPolygon(celda.poligonoLateral);
  painter.restore();
}

// ============================================================================
// RENDERIZADO: UI SUPERPUESTA
// ============================================================================

void VisorOlap::dibujarTooltip(QPainter &painter, const CeldaCubo &celda,
                               const QPoint &pos) {
  QString texto = QString("📊 %1: %2\n📦 %3: %4\n💰 %5: %6")
                      .arg(m_metadata.dimensionX.nombre, celda.nombreDimX)
                      .arg(m_metadata.dimensionZ.nombre, celda.nombreDimZ)
                      .arg(m_metadata.medidaActual)
                      .arg(celda.valor, 0, 'f', 2);

  QFont font("Segoe UI", 9);
  painter.setFont(font);
  QFontMetrics fm(font);

  QStringList lines = texto.split('\n');
  int maxWidth = 0;
  for (const QString &line : lines) {
    maxWidth = qMax(maxWidth, fm.horizontalAdvance(line));
  }

  QRect bg(pos.x() + 15, pos.y() + 15, maxWidth + 20,
           fm.height() * lines.size() + 20);

  // Fondo con sombra
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 30));
  painter.drawRoundedRect(bg.adjusted(3, 3, 3, 3), 8, 8);
  painter.restore();

  // Fondo principal
  painter.setBrush(QColor(255, 255, 255, 250));
  painter.setPen(QPen(QColor("#cbd5e1"), 1));
  painter.drawRoundedRect(bg, 8, 8);

  // Texto
  painter.setPen(QColor("#1e293b"));
  painter.drawText(bg.adjusted(10, 10, -10, -10), Qt::AlignLeft, texto);
}

void VisorOlap::dibujarPanelControl(QPainter &painter) {
  int panelX = 20;
  int panelY = 20;
  int panelW = 280;
  int panelH = qMin(height() - 40, 400);

  // Sombra
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 20));
  painter.drawRoundedRect(panelX + 3, panelY + 3, panelW, panelH, 12, 12);
  painter.restore();

  // Fondo glassmorphism
  painter.setBrush(QColor(255, 255, 255, 235));
  painter.setPen(QPen(QColor("#e2e8f0"), 1));
  painter.drawRoundedRect(panelX, panelY, panelW, panelH, 12, 12);

  int y = panelY + 25;
  int x = panelX + 20;

  // Título
  painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
  painter.setPen(QColor("#0f172a"));
  painter.drawText(x, y, "📊 Configuración del Cubo");
  y += 35;

  // Línea separadora
  painter.setPen(QPen(QColor("#cbd5e1"), 1));
  painter.drawLine(x, y, panelX + panelW - 20, y);
  y += 20;

  // Tabla de hechos
  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#64748b"));
  painter.drawText(x, y, "Tabla de Hechos:");
  y += 15;
  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#1e293b"));
  painter.drawText(x, y, m_metadata.nombreTablaHechos);
  y += 25;

  // Medida
  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#64748b"));
  painter.drawText(x, y, "Medida:");
  y += 15;
  painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
  painter.setPen(m_config.colorEjeY);
  painter.drawText(x, y, m_metadata.medidaActual);
  y += 25;

  // Dimensiones
  dibujarInfoDimension(painter, x, y, "Eje X", m_metadata.dimensionX);
  y += 55;
  dibujarInfoDimension(painter, x, y, "Eje Z", m_metadata.dimensionZ);
  y += 60;

  // Estadísticas
  painter.setPen(QPen(QColor("#cbd5e1"), 1));
  painter.drawLine(x, y, panelX + panelW - 20, y);
  y += 15;

  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#64748b"));
  painter.drawText(x, y, "Estadísticas:");
  y += 18;

  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#475569"));
  painter.drawText(x, y,
                   QString("• Registros: %1").arg(m_metadata.totalRegistros));
  y += 15;
  painter.drawText(x, y,
                   QString("• Celdas: %1 / %2")
                       .arg(m_metadata.celdasConDatos)
                       .arg(m_metadata.totalCeldas));
  y += 15;
  painter.drawText(x, y,
                   QString("• Rango: %1 - %2")
                       .arg(m_metadata.valorMinimo, 0, 'f', 0)
                       .arg(m_metadata.valorMaximo, 0, 'f', 0));
}

void VisorOlap::dibujarInfoDimension(QPainter &painter, int x, int y,
                                     const QString &titulo,
                                     const DimensionInfo &dim) {
  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#64748b"));
  painter.drawText(x, y, titulo + ":");
  y += 15;

  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  QColor color = (titulo == "Eje X") ? m_config.colorEjeX : m_config.colorEjeZ;
  painter.setPen(color);
  painter.drawText(x, y, dim.nombre);
  y += 15;

  painter.setFont(QFont("Segoe UI", 7));
  painter.setPen(QColor("#94a3b8"));
  QString info = QString("Cardinalidad: %1").arg(dim.cardinalidad);
  painter.drawText(x, y, info);
}

void VisorOlap::dibujarLeyenda(QPainter &painter) {
  int x = width() - 180;
  int y = height() - 100;

  // Sombra
  painter.save();
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 20));
  painter.drawRoundedRect(x + 3, y + 3, 170, 90, 8, 8);
  painter.restore();

  // Fondo
  painter.setBrush(QColor(255, 255, 255, 235));
  painter.setPen(QPen(QColor("#e2e8f0"), 1));
  painter.drawRoundedRect(x, y, 170, 90, 8, 8);

  // Título
  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#0f172a"));
  painter.drawText(x + 15, y + 20, "Escala de Valores");

  // Gradiente
  QLinearGradient g(x + 15, y + 35, x + 155, y + 35);
  g.setColorAt(0, QColor("#10b981"));
  g.setColorAt(0.5, QColor("#f59e0b"));
  g.setColorAt(1, QColor("#ef4444"));
  painter.setBrush(g);
  painter.setPen(Qt::NoPen);
  painter.drawRect(x + 15, y + 30, 140, 25);

  // Etiquetas
  painter.setFont(QFont("Segoe UI", 7));
  painter.setPen(QColor("#475569"));
  painter.drawText(x + 15, y + 70, QString::number(m_valorMin, 'f', 0));
  painter.drawText(x + 120, y + 70, QString::number(m_valorMax, 'f', 0));
}

// ============================================================================
// UTILIDADES
// ============================================================================

QPointF VisorOlap::proyectarIso(float x, float y, float z) {
  float angle = 30.0f * M_PI / 180.0f;
  return QPointF((x - z) * qCos(angle), (x + z) * qSin(angle) - y);
}

QColor VisorOlap::obtenerColorGradiente(double valor, double min, double max) {
  if (max <= min)
    return QColor("#10b981");
  double t = qBound(0.0, (valor - min) / (max - min), 1.0);

  // Gradiente verde -> amarillo -> rojo
  if (t < 0.5) {
    return QColor::fromHsv(120 - t * 120, 180, 230);
  } else {
    return QColor::fromHsv(60 - (t - 0.5) * 120, 200, 230);
  }
}

QColor VisorOlap::obtenerColorPorValor(double valor) {
  return obtenerColorGradiente(valor, m_valorMin, m_valorMax);
}

int VisorOlap::detectarCeldaEnPunto(const QPoint &punto) {
  // La transformación en paintEvent es:
  // 1. translate(width/2 + panOffset.x, height/2 + panOffset.y)
  // 2. scale(zoom, zoom)
  //
  // Los polígonos se guardan en coordenadas ya transformadas (después de scale)
  // así que necesito transformar el punto del mouse de la misma manera

  // Primero restamos la traslación del centro + panOffset
  QPointF centro(width() / 2.0 + m_panOffset.x(),
                 height() / 2.0 + m_panOffset.y());
  QPointF puntoTransformado = QPointF(punto) - centro;

  // Luego dividimos por el zoom
  puntoTransformado /= m_zoom;

  // Iterar de atrás hacia adelante (los últimos dibujados están encima)
  for (int i = m_celdas.size() - 1; i >= 0; i--) {
    // Verificar si el punto está dentro de alguno de los polígonos del cubo
    if (m_celdas[i].poligonoTop.containsPoint(puntoTransformado,
                                              Qt::OddEvenFill) ||
        m_celdas[i].poligonoFrontal.containsPoint(puntoTransformado,
                                                  Qt::OddEvenFill) ||
        m_celdas[i].poligonoLateral.containsPoint(puntoTransformado,
                                                  Qt::OddEvenFill)) {
      return i;
    }
  }
  return -1;
}

void VisorOlap::recalcularAreasPantalla() { update(); }

void VisorOlap::emitirEstadisticasSeleccion() {
  int cantidad = 0;
  double suma = 0.0;

  for (const CeldaCubo &celda : m_celdas) {
    if (celda.seleccionada) {
      cantidad++;
      suma += celda.valor;
    }
  }

  emit estadisticasActualizadas(cantidad, suma);
}

// ============================================================================
// EVENTOS DE INTERACCIÓN
// ============================================================================

void VisorOlap::mousePressEvent(QMouseEvent *event) {
  m_lastMousePos = event->pos();
  if (event->button() == Qt::LeftButton) {
    int idx = detectarCeldaEnPunto(event->pos());
    if (idx >= 0) {
      m_celdas[idx].seleccionada = !m_celdas[idx].seleccionada;

      // Construir información clara y útil para mostrar
      const CeldaCubo &celda = m_celdas[idx];
      QString info = QString("📊 %1: %2\n"
                             "📦 %3: %4\n"
                             "💰 %5: $%6\n"
                             "📍 Posición: (%7, %8)")
                         .arg(m_metadata.dimensionX.nombre, celda.nombreDimX)
                         .arg(m_metadata.dimensionZ.nombre, celda.nombreDimZ)
                         .arg(m_metadata.medidaActual)
                         .arg(celda.valor, 0, 'f', 2)
                         .arg(celda.dimX)
                         .arg(celda.dimZ);

      emit celdaSeleccionada(info);

      // También emitir estadísticas de todas las celdas seleccionadas
      emitirEstadisticasSeleccion();

      update();
    } else {
      m_arrastrando = true;
    }
  }
}

void VisorOlap::mouseMoveEvent(QMouseEvent *event) {
  if (m_arrastrando) {
    m_panOffset += event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();
    update();
  } else {
    int idx = detectarCeldaEnPunto(event->pos());
    if (idx != m_celdaHover) {
      m_celdaHover = idx;
      update();
    }
  }
}

void VisorOlap::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event);
  m_arrastrando = false;
}

void VisorOlap::mouseDoubleClickEvent(QMouseEvent *event) {
  int idx = detectarCeldaEnPunto(event->pos());
  if (idx >= 0) {
    emit drillDown(m_celdas[idx].dimX, m_celdas[idx].dimY, m_celdas[idx].dimZ);
  }
}

void VisorOlap::wheelEvent(QWheelEvent *e) {
  if (e->angleDelta().y() > 0)
    m_zoom *= 1.1;
  else
    m_zoom *= 0.9;

  m_zoom = qBound(0.3f, m_zoom, 3.0f);
  update();
}

void VisorOlap::leaveEvent(QEvent *) {
  m_celdaHover = -1;
  update();
}

void VisorOlap::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Escape)
    limpiarSeleccion();
  else if (e->key() == Qt::Key_R)
    recargarCubo();
  else if (e->key() == Qt::Key_G) {
    m_config.mostrarGrid = !m_config.mostrarGrid;
    update();
  }
}

void VisorOlap::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  update();
}
