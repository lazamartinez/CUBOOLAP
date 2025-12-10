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


VisorOlap::VisorOlap(QWidget *parent)
    : QWidget(parent), m_zoom(1.0f), m_panOffset(0, 0), m_arrastrando(false),
      m_celdaHover(-1), m_valorMax(1.0), m_valorMin(0.0), m_tamanoBase(40.0f),
      m_escalaAltura(2.0f), m_espaciado(50.0f) {

  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setMinimumSize(400, 300);

  // Cargar datos al iniciar
  cargarCuboDummy();
}

VisorOlap::~VisorOlap() {}

void VisorOlap::cargarCuboDummy() { cargarCuboReal(); }

// Helper para consultas seguras
static QString execValue(QSqlDatabase &db, const QString &sql) {
  QSqlQuery q(db);
  if (q.exec(sql) && q.next())
    return q.value(0).toString();
  return QString();
}

void VisorOlap::cargarCuboReal() {
  m_celdas.clear();
  QSqlDatabase db = QSqlDatabase::database("CuboVisionConnection");
  if (!db.isOpen())
    return;

  // 1. Deteccion Automatica de Tabla de Hechos (La mas grande)
  QString sqlFact = R"(
      SELECT relname FROM pg_class c 
      JOIN pg_namespace n ON n.oid = c.relnamespace 
      WHERE c.relkind = 'r' AND n.nspname = 'public' 
      ORDER BY c.reltuples DESC LIMIT 1
  )";
  QString factTable = execValue(db, sqlFact);
  if (factTable.isEmpty())
    return;

  // 2. Detectar Columnas Numericas (Medidas) en Fact
  QString sqlMeasure = QString(R"(
      SELECT column_name FROM information_schema.columns 
      WHERE table_name = '%1' AND table_schema = 'public'
      AND data_type IN ('integer', 'numeric', 'real', 'double precision', 'bigint')
      AND column_name NOT LIKE 'id_%' 
      LIMIT 1
  )")
                           .arg(factTable);
  QString measureCol = execValue(db, sqlMeasure);
  if (measureCol.isEmpty())
    measureCol = "count(*)";
  else
    measureCol = "SUM(" + measureCol + ")";

  // 3. Detectar Dimensiones via FKs
  struct DimensionInfo {
    QString table;
    QString fkCol;
    QString pkCol;
    QString labelCol;
  };
  QVector<DimensionInfo> dims;

  QString sqlFKs = QString(R"(
     SELECT
       kcu.column_name as fk_col,
       ccu.table_name as ref_table,
       ccu.column_name as ref_pk
     FROM information_schema.key_column_usage kcu
     JOIN information_schema.constraint_column_usage ccu ON kcu.constraint_name = ccu.constraint_name
     JOIN information_schema.table_constraints tc ON kcu.constraint_name = tc.constraint_name
     WHERE tc.constraint_type = 'FOREIGN KEY' AND kcu.table_name = '%1'
     LIMIT 3
  )")
                       .arg(factTable);

  QSqlQuery qFK(db);
  if (qFK.exec(sqlFKs)) {
    while (qFK.next()) {
      DimensionInfo d;
      d.fkCol = qFK.value(0).toString();
      d.table = qFK.value(1).toString();
      d.pkCol = qFK.value(2).toString();

      QString sqlLabel = QString(R"(
              SELECT column_name FROM information_schema.columns 
              WHERE table_name = '%1' 
              AND data_type IN ('character varying', 'text', 'character')
              AND column_name NOT LIKE 'id_%' AND column_name NOT LIKE 'codigo%'
              LIMIT 1
          )")
                             .arg(d.table);
      d.labelCol = execValue(db, sqlLabel);

      if (d.labelCol.isEmpty())
        d.labelCol = d.pkCol;
      dims.append(d);
    }
  }

  // Fallback si < 3 dimensiones
  if (dims.size() < 3) {
    QString sqlTextCols = QString(R"(
          SELECT column_name FROM information_schema.columns 
          WHERE table_name = '%1' 
          AND data_type IN ('character varying', 'text')
          LIMIT %2
      )")
                              .arg(factTable)
                              .arg(3 - dims.size());

    QSqlQuery qTxt(db);
    if (qTxt.exec(sqlTextCols)) {
      while (qTxt.next()) {
        DimensionInfo d;
        d.table = factTable; // Self reference pseudo-dim
        d.labelCol = qTxt.value(0).toString();
        dims.append(d);
      }
    }
  }

  while (dims.size() < 3) {
    DimensionInfo d;
    d.table = "";
    d.labelCol = "'N/A'";
    dims.append(d);
  }

  // 4. Construir Consulta Dinamica
  QString dimX = (dims[0].table == factTable) ? dims[0].labelCol
                                              : ("d0." + dims[0].labelCol);
  QString dimY = (dims[1].table == factTable) ? dims[1].labelCol
                                              : ("d1." + dims[1].labelCol);
  QString dimZ = (dims[2].table == factTable) ? dims[2].labelCol
                                              : ("d2." + dims[2].labelCol);

  QString querySql = "SELECT " + dimX + " as dx, " + dimY + " as dy, " + dimZ +
                     " as dz, " + measureCol + " as val ";
  querySql += " FROM " + factTable + " f ";

  for (int i = 0; i < 3; i++) {
    if (!dims[i].table.isEmpty() && dims[i].table != factTable) {
      querySql += QString(" JOIN %1 d%2 ON f.%3 = d%2.%4 ")
                      .arg(dims[i].table)
                      .arg(i)
                      .arg(dims[i].fkCol)
                      .arg(dims[i].pkCol);
    }
  }

  querySql += " GROUP BY 1, 2, 3 ORDER BY 4 DESC LIMIT 300";

  QSqlQuery query(db);
  if (!query.exec(querySql))
    return;

  struct DatoRaw {
    QString x, y, z;
    double val;
  };
  QVector<DatoRaw> datos;
  QStringList xKeys, yKeys, zKeys;

  m_valorMax = 0;
  m_valorMin = 9e15;

  while (query.next()) {
    DatoRaw d;
    d.x = query.value(0).toString();
    d.y = query.value(1).toString();
    d.z = query.value(2).toString();
    d.val = query.value(3).toDouble();
    datos.append(d);

    if (!xKeys.contains(d.x))
      xKeys << d.x;
    if (!yKeys.contains(d.y))
      yKeys << d.y;
    if (!zKeys.contains(d.z))
      zKeys << d.z;

    if (d.val > m_valorMax)
      m_valorMax = d.val;
    if (d.val < m_valorMin)
      m_valorMin = d.val;
  }

  if (xKeys.size() > 6)
    xKeys = xKeys.mid(0, 6);
  if (yKeys.size() > 6)
    yKeys = yKeys.mid(0, 6);
  if (zKeys.size() > 6)
    zKeys = zKeys.mid(0, 6);

  for (const auto &d : datos) {
    int ix = xKeys.indexOf(d.x);
    int iy = yKeys.indexOf(d.y);
    int iz = zKeys.indexOf(d.z);

    if (ix >= 0 && iy >= 0 && iz >= 0) {
      CeldaCubo celda;
      celda.dimX = ix;
      celda.dimY = iy;
      celda.dimZ = iz;
      celda.nombreDimX = d.x;
      celda.nombreDimY = d.y;
      celda.nombreDimZ = d.z;
      celda.valor = d.val;
      celda.valorSecundario = 0;
      celda.etiqueta =
          QString("%1\n%2\n%3\n%4").arg(d.x, d.y, d.z).arg(d.val, 0, 'f', 0);
      m_celdas.append(celda);
    }
  }
  update();
}

void VisorOlap::cargarDatos(const QVector<CeldaCubo> &celdas) {
  m_celdas = celdas;

  m_valorMax = 0;
  m_valorMin = 999999;
  for (const auto &c : m_celdas) {
    if (c.valor > m_valorMax)
      m_valorMax = c.valor;
    if (c.valor < m_valorMin)
      m_valorMin = c.valor;
  }

  update();
}

void VisorOlap::limpiarSeleccion() {
  for (auto &c : m_celdas) {
    c.seleccionada = false;
  }
  emit seleccionCambiada(0);
  update();
}

QVector<CeldaCubo> VisorOlap::obtenerSeleccion() const {
  QVector<CeldaCubo> sel;
  for (const auto &c : m_celdas) {
    if (c.seleccionada)
      sel.append(c);
  }
  return sel;
}

void VisorOlap::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  dibujarFondo(painter);

  // Transformacion global
  painter.save();
  painter.translate(width() / 2 + m_panOffset.x(),
                    height() / 2 + m_panOffset.y());
  painter.scale(m_zoom, m_zoom);

  dibujarEjes(painter);

  // Ordenar celdas por profundidad (pintor algorithm)
  QVector<int> indices(m_celdas.size());
  for (int i = 0; i < m_celdas.size(); i++)
    indices[i] = i;

  std::sort(indices.begin(), indices.end(), [this](int a, int b) {
    const CeldaCubo &ca = m_celdas[a];
    const CeldaCubo &cb = m_celdas[b];
    return (ca.dimX + ca.dimZ) < (cb.dimX + cb.dimZ);
  });

  // Dibujar celdas ordenadas
  for (int idx : indices) {
    bool hover = (idx == m_celdaHover);
    bool sel = m_celdas[idx].seleccionada;
    dibujarPrisma(painter, m_celdas[idx], hover, sel);
  }

  painter.restore();

  // UI superpuesta
  dibujarLeyenda(painter);
  dibujarInfoPanel(painter);

  // Tooltip si hay hover
  if (m_celdaHover >= 0 && m_celdaHover < m_celdas.size()) {
    dibujarTooltip(painter, m_celdas[m_celdaHover],
                   mapFromGlobal(QCursor::pos()));
  }
}

void VisorOlap::dibujarFondo(QPainter &painter) {
  QLinearGradient grad(0, 0, 0, height());
  grad.setColorAt(0, QColor("#f8fafc"));
  grad.setColorAt(1, QColor("#e2e8f0"));
  painter.fillRect(rect(), grad);

  painter.setPen(QPen(QColor("#cbd5e1"), 1, Qt::DotLine));
  int step = 50;
  for (int x = 0; x < width(); x += step) {
    painter.drawLine(x, 0, x, height());
  }
  for (int y = 0; y < height(); y += step) {
    painter.drawLine(0, y, width(), y);
  }
}

void VisorOlap::dibujarEjes(QPainter &painter) {
  painter.setPen(QPen(QColor("#64748b"), 2));

  QPointF origen = proyectarIso(0, 0, 0);
  QPointF ejeX = proyectarIso(250, 0, 0);
  QPointF ejeY = proyectarIso(0, 200, 0);
  QPointF ejeZ = proyectarIso(0, 0, 250);

  painter.drawLine(origen, ejeX);
  painter.drawLine(origen, ejeY);
  painter.drawLine(origen, ejeZ);

  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#2563eb"));
  painter.drawText(ejeX + QPointF(5, 0), "Eje X (Dim 1)");
  painter.setPen(QColor("#10b981"));
  painter.drawText(ejeY + QPointF(5, -5), "Valor");
  painter.setPen(QColor("#f59e0b"));
  painter.drawText(ejeZ + QPointF(-30, 15), "Eje Z (Dim 3)");
}

void VisorOlap::dibujarPrisma(QPainter &painter, const CeldaCubo &celda,
                              bool hover, bool seleccionada) {
  float x = celda.dimX * m_espaciado;
  float z = celda.dimZ * m_espaciado;
  float y = celda.dimY * (m_espaciado * 0.8f);

  float alturaBase = 10.0f;
  // Ajuste logaritmico para valores grandes
  float altura =
      alturaBase + (float)(log10(celda.valor + 1) * m_escalaAltura * 8.0f);

  float w = m_tamanoBase;
  float d = m_tamanoBase;

  QColor colorBase = obtenerColorGradiente(celda.valor, m_valorMin, m_valorMax);

  if (hover)
    colorBase = colorBase.lighter(130);
  if (seleccionada)
    colorBase = QColor("#2563eb");

  auto iso = [this](float ix, float iy, float iz) -> QPointF {
    return proyectarIso(ix, iy, iz);
  };

  QPointF p1 = iso(x + w / 2, y, z + d / 2);
  QPointF p2 = iso(x + w / 2, y, z - d / 2);
  QPointF p3 = iso(x - w / 2, y, z - d / 2);
  QPointF p4 = iso(x - w / 2, y, z + d / 2);

  QPointF t1 = iso(x + w / 2, y + altura, z + d / 2);
  QPointF t2 = iso(x + w / 2, y + altura, z - d / 2);
  QPointF t3 = iso(x - w / 2, y + altura, z - d / 2);
  QPointF t4 = iso(x - w / 2, y + altura, z + d / 2);

  QPainterPath pathTop;
  pathTop.moveTo(t1);
  pathTop.lineTo(t2);
  pathTop.lineTo(t3);
  pathTop.lineTo(t4);
  pathTop.closeSubpath();
  painter.setPen(QPen(colorBase.darker(150), hover ? 2 : 1));
  painter.setBrush(colorBase.lighter(115));
  painter.drawPath(pathTop);

  QPainterPath pathRight;
  pathRight.moveTo(p1);
  pathRight.lineTo(p2);
  pathRight.lineTo(t2);
  pathRight.lineTo(t1);
  pathRight.closeSubpath();
  painter.setBrush(colorBase);
  painter.drawPath(pathRight);

  QPainterPath pathLeft;
  pathLeft.moveTo(p4);
  pathLeft.lineTo(p1);
  pathLeft.lineTo(t1);
  pathLeft.lineTo(t4);
  pathLeft.closeSubpath();
  painter.setBrush(colorBase.darker(120));
  painter.drawPath(pathLeft);

  qreal minX = std::min({t1.x(), t2.x(), t3.x(), t4.x()});
  qreal minY = std::min({t1.y(), t2.y(), t3.y(), t4.y()});
  qreal maxX = std::max({p1.x(), p2.x(), p3.x(), p4.x()});
  qreal maxY = std::max({p1.y(), p2.y(), p3.y(), p4.y()});

  const_cast<CeldaCubo &>(celda).areaPantalla =
      QRectF(minX, minY, maxX - minX, maxY - minY);

  if (seleccionada || hover) {
    painter.setFont(QFont("Segoe UI", 8, QFont::Bold));
    painter.setPen(Qt::white);
    QPointF centro = (t1 + t3) / 2;
    QString txt = QString("$%1").arg(celda.valor, 0, 'f', 0);
    QRectF txtBg(centro.x() - 25, centro.y() - 18, 50, 16);
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(txtBg, 4, 4);
    painter.setPen(Qt::white);
    painter.drawText(txtBg, Qt::AlignCenter, txt);
  }
}

void VisorOlap::dibujarTooltip(QPainter &painter, const CeldaCubo &celda,
                               const QPoint &pos) {
  QString texto = QString("X: %1\nY: %2\nZ: %3\n\nValor: %4")
                      .arg(celda.nombreDimX)
                      .arg(celda.nombreDimY)
                      .arg(celda.nombreDimZ)
                      .arg(celda.valor, 0, 'f', 2);

  QFont font("Segoe UI", 10);
  painter.setFont(font);
  QFontMetrics fm(font);
  int padding = 12;
  int lineHeight = fm.height();
  QStringList lineas = texto.split('\n');
  int maxWidth = 0;
  for (const QString &l : lineas)
    maxWidth = qMax(maxWidth, fm.horizontalAdvance(l));

  int tooltipWidth = maxWidth + padding * 2;
  int tooltipHeight = lineas.size() * lineHeight + padding * 2;
  int x = pos.x() + 15;
  int y = pos.y() + 15;
  if (x + tooltipWidth > width())
    x = pos.x() - tooltipWidth - 15;
  if (y + tooltipHeight > height())
    y = pos.y() - tooltipHeight - 15;

  QRectF bgRect(x, y, tooltipWidth, tooltipHeight);
  painter.setBrush(QColor(255, 255, 255, 240));
  painter.setPen(QPen(QColor("#2563eb"), 1));
  painter.drawRoundedRect(bgRect, 8, 8);

  painter.setPen(QColor("#1f2937"));
  int ty = y + padding + fm.ascent();
  for (const QString &linea : lineas) {
    painter.drawText(x + padding, ty, linea);
    ty += lineHeight;
  }
}

void VisorOlap::dibujarLeyenda(QPainter &painter) {
  int legendaW = 150;
  int legendaH = 80;
  int x = width() - legendaW - 20;
  int y = height() - legendaH - 20;

  painter.setBrush(QColor(255, 255, 255, 230));
  painter.setPen(QPen(QColor("#e5e7eb"), 1));
  painter.drawRoundedRect(x, y, legendaW, legendaH, 8, 8);

  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#374151"));
  painter.drawText(x + 10, y + 18, "Escala");

  QLinearGradient grad(x + 10, y + 30, x + legendaW - 10, y + 30);
  grad.setColorAt(0, QColor("#10b981"));
  grad.setColorAt(0.5, QColor("#f59e0b"));
  grad.setColorAt(1, QColor("#ef4444"));

  painter.setBrush(grad);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(x + 10, y + 30, legendaW - 20, 15, 4, 4);

  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#6b7280"));
  painter.drawText(x + 10, y + 60, QString("$%1").arg(m_valorMin, 0, 'f', 0));
  painter.drawText(x + legendaW - 50, y + 60,
                   QString("$%1").arg(m_valorMax, 0, 'f', 0));
}

void VisorOlap::dibujarInfoPanel(QPainter &painter) {
  painter.setFont(QFont("Segoe UI", 10));
  painter.setPen(QColor("#374151"));

  int selCount = 0;
  for (const auto &c : m_celdas)
    if (c.seleccionada)
      selCount++;

  QString info =
      QString("Items: %1  |  Sel: %2").arg(m_celdas.size()).arg(selCount);
  QRectF bgRect(10, 10, painter.fontMetrics().horizontalAdvance(info) + 20, 28);
  painter.setBrush(QColor(255, 255, 255, 220));
  painter.setPen(QPen(QColor("#e5e7eb"), 1));
  painter.drawRoundedRect(bgRect, 6, 6);
  painter.setPen(QColor("#374151"));
  painter.drawText(bgRect.adjusted(10, 0, 0, 0), Qt::AlignVCenter, info);
}

void VisorOlap::mousePressEvent(QMouseEvent *event) {
  m_lastMousePos = event->pos();
  if (event->button() == Qt::LeftButton) {
    int idx = detectarCeldaEnPunto(event->pos());
    if (idx >= 0) {
      if (!(event->modifiers() & Qt::ControlModifier)) {
        for (auto &c : m_celdas)
          c.seleccionada = false;
      }
      m_celdas[idx].seleccionada = !m_celdas[idx].seleccionada;
      emit celdaSeleccionada(m_celdas[idx].etiqueta);
      update();
    } else {
      m_arrastrando = true;
    }
  }
}

void VisorOlap::mouseMoveEvent(QMouseEvent *event) {
  if (m_arrastrando && (event->buttons() & Qt::LeftButton)) {
    m_panOffset += QPointF(event->pos() - m_lastMousePos);
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

void VisorOlap::mouseDoubleClickEvent(QMouseEvent *event) {
  // Drill down logic here in future
}

void VisorOlap::wheelEvent(QWheelEvent *event) {
  float factor = event->angleDelta().y() > 0 ? 1.15f : 0.87f;
  float newZoom = m_zoom * factor;
  if (newZoom >= 0.3f && newZoom <= 4.0f) {
    m_zoom = newZoom;
    update();
  }
}

void VisorOlap::leaveEvent(QEvent *event) {
  Q_UNUSED(event);
  m_celdaHover = -1;
  update();
}

void VisorOlap::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape)
    limpiarSeleccion();
}

QPointF VisorOlap::proyectarIso(float x, float y, float z) {
  float angle = 30.0f * M_PI / 180.0f;
  return QPointF((x - z) * qCos(angle), (x + z) * qSin(angle) - y);
}

QColor VisorOlap::obtenerColorGradiente(double valor, double min, double max) {
  if (max <= min)
    return QColor("#10b981");
  double t = qBound(0.0, (valor - min) / (max - min), 1.0);
  if (t < 0.5) {
    double u = t * 2;
    return QColor((int)(16 + u * (245 - 16)), (int)(185 + u * (158 - 185)),
                  (int)(129 + u * (11 - 129)));
  } else {
    double u = (t - 0.5) * 2;
    return QColor((int)(245 + u * (239 - 245)), (int)(158 - u * 158),
                  (int)(11 + u * (68 - 11)));
  }
}

int VisorOlap::detectarCeldaEnPunto(const QPoint &punto) {
  QPointF p =
      (punto - QPointF(width() / 2, height() / 2) - m_panOffset) / m_zoom;
  for (int i = m_celdas.size() - 1; i >= 0; i--) {
    if (m_celdas[i].areaPantalla.contains(p))
      return i;
  }
  return -1;
}

void VisorOlap::recalcularAreasPantalla() { update(); }
