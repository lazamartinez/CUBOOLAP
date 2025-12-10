#include "VisorOlap.h"
#include "Estilos.h"
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QToolTip>
#include <QtMath>
#include <algorithm>

VisorOlap::VisorOlap(QWidget *parent)
    : QWidget(parent), m_zoom(1.0f), m_panOffset(0, 0), m_arrastrando(false),
      m_celdaHover(-1), m_valorMax(1.0), m_valorMin(0.0), m_tamanoBase(40.0f),
      m_escalaAltura(2.0f), m_espaciado(50.0f) {

  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true); // Importante para hover
  setMinimumSize(400, 300);

  // Cargar datos de ejemplo al iniciar
  cargarCuboDummy();
}

VisorOlap::~VisorOlap() {}

void VisorOlap::cargarCuboDummy() {
  m_celdas.clear();

  // Generar datos de ejemplo: cubo 4x4x3 con valores aleatorios
  QStringList productos = {"Electronica", "Ropa", "Alimentos", "Hogar"};
  QStringList regiones = {"Norte", "Sur", "Este", "Oeste"};
  QStringList trimestres = {"Q1", "Q2", "Q3"};

  m_valorMax = 0;
  m_valorMin = 999999;

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 3; y++) {
      for (int z = 0; z < 4; z++) {
        CeldaCubo celda;
        celda.dimX = x;
        celda.dimY = y;
        celda.dimZ = z;
        celda.nombreDimX = productos[x];
        celda.nombreDimY = trimestres[y];
        celda.nombreDimZ = regiones[z];

        // Generar valor con patron (no totalmente aleatorio para demo)
        double baseVal = 5000 + (x * 1000) + (y * 2000) + (z * 500);
        double variacion = QRandomGenerator::global()->bounded(3000) - 1500;
        celda.valor = baseVal + variacion;
        celda.valorSecundario =
            QRandomGenerator::global()->bounded(100) / 100.0;

        celda.etiqueta = QString("%1\n%2 - %3\n$%4")
                             .arg(celda.nombreDimX)
                             .arg(celda.nombreDimY)
                             .arg(celda.nombreDimZ)
                             .arg(celda.valor, 0, 'f', 0);

        if (celda.valor > m_valorMax)
          m_valorMax = celda.valor;
        if (celda.valor < m_valorMin)
          m_valorMin = celda.valor;

        m_celdas.append(celda);
      }
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

// ============================================================================
// RENDERIZADO
// ============================================================================

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
  // Gradiente suave de fondo
  QLinearGradient grad(0, 0, 0, height());
  grad.setColorAt(0, QColor("#f8fafc"));
  grad.setColorAt(1, QColor("#e2e8f0"));
  painter.fillRect(rect(), grad);

  // Grid sutil
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

  // Eje X (derecha)
  QPointF origen = proyectarIso(0, 0, 0);
  QPointF ejeX = proyectarIso(200, 0, 0);
  QPointF ejeY = proyectarIso(0, 150, 0);
  QPointF ejeZ = proyectarIso(0, 0, 200);

  painter.drawLine(origen, ejeX);
  painter.drawLine(origen, ejeY);
  painter.drawLine(origen, ejeZ);

  // Etiquetas de ejes
  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#2563eb"));
  painter.drawText(ejeX + QPointF(5, 0), "Producto");
  painter.setPen(QColor("#10b981"));
  painter.drawText(ejeY + QPointF(5, -5), "Valor");
  painter.setPen(QColor("#f59e0b"));
  painter.drawText(ejeZ + QPointF(-30, 15), "Region");
}

void VisorOlap::dibujarPrisma(QPainter &painter, const CeldaCubo &celda,
                              bool hover, bool seleccionada) {
  float x = celda.dimX * m_espaciado;
  float z = celda.dimZ * m_espaciado;
  float y =
      celda.dimY * (m_espaciado * 0.8f); // Separacion vertical entre capas

  // Altura basada en valor (logaritmica para mejor visualizacion)
  float alturaBase = 10.0f;
  float altura =
      alturaBase + (float)(log10(celda.valor + 1) * m_escalaAltura * 15.0f);

  float w = m_tamanoBase;
  float d = m_tamanoBase;

  // Color basado en valor
  QColor colorBase = obtenerColorGradiente(celda.valor, m_valorMin, m_valorMax);

  if (hover) {
    colorBase = colorBase.lighter(130);
  }
  if (seleccionada) {
    colorBase = QColor("#2563eb");
  }

  // Proyeccion isometrica
  auto iso = [this](float ix, float iy, float iz) -> QPointF {
    return proyectarIso(ix, iy, iz);
  };

  // Vertices base
  QPointF p1 = iso(x + w / 2, y, z + d / 2);
  QPointF p2 = iso(x + w / 2, y, z - d / 2);
  QPointF p3 = iso(x - w / 2, y, z - d / 2);
  QPointF p4 = iso(x - w / 2, y, z + d / 2);

  // Vertices top
  QPointF t1 = iso(x + w / 2, y + altura, z + d / 2);
  QPointF t2 = iso(x + w / 2, y + altura, z - d / 2);
  QPointF t3 = iso(x - w / 2, y + altura, z - d / 2);
  QPointF t4 = iso(x - w / 2, y + altura, z + d / 2);

  // Cara superior
  QPainterPath pathTop;
  pathTop.moveTo(t1);
  pathTop.lineTo(t2);
  pathTop.lineTo(t3);
  pathTop.lineTo(t4);
  pathTop.closeSubpath();

  painter.setPen(QPen(colorBase.darker(150), hover ? 2 : 1));
  painter.setBrush(colorBase.lighter(115));
  painter.drawPath(pathTop);

  // Cara derecha
  QPainterPath pathRight;
  pathRight.moveTo(p1);
  pathRight.lineTo(p2);
  pathRight.lineTo(t2);
  pathRight.lineTo(t1);
  pathRight.closeSubpath();
  painter.setBrush(colorBase);
  painter.drawPath(pathRight);

  // Cara izquierda (sombra)
  QPainterPath pathLeft;
  pathLeft.moveTo(p4);
  pathLeft.lineTo(p1);
  pathLeft.lineTo(t1);
  pathLeft.lineTo(t4);
  pathLeft.closeSubpath();
  painter.setBrush(colorBase.darker(120));
  painter.drawPath(pathLeft);

  // Guardar area para deteccion de clicks
  qreal minX = std::min({t1.x(), t2.x(), t3.x(), t4.x()});
  qreal minY = std::min({t1.y(), t2.y(), t3.y(), t4.y()});
  qreal maxX = std::max({p1.x(), p2.x(), p3.x(), p4.x()});
  qreal maxY = std::max({p1.y(), p2.y(), p3.y(), p4.y()});

  QRectF bounds(minX, minY, maxX - minX, maxY - minY);

  // Hack: guardar bounds transformados (esto es aproximado)
  const_cast<CeldaCubo &>(celda).areaPantalla = bounds;

  // Etiqueta sobre prisma seleccionado o hover
  if (seleccionada || hover) {
    painter.setFont(QFont("Segoe UI", 8, QFont::Bold));
    painter.setPen(Qt::white);
    QPointF centro = (t1 + t3) / 2;
    QString txt = QString("$%1").arg(celda.valor, 0, 'f', 0);

    // Fondo de texto
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
  QString texto = QString("Producto: %1\nPeriodo: %2\nRegion: %3\n\nValor: "
                          "$%4\nRendimiento: %5%")
                      .arg(celda.nombreDimX)
                      .arg(celda.nombreDimY)
                      .arg(celda.nombreDimZ)
                      .arg(celda.valor, 0, 'f', 2)
                      .arg(celda.valorSecundario * 100, 0, 'f', 1);

  QFont font("Segoe UI", 10);
  painter.setFont(font);
  QFontMetrics fm(font);

  int padding = 12;
  int lineHeight = fm.height();
  QStringList lineas = texto.split('\n');
  int maxWidth = 0;
  for (const QString &l : lineas) {
    maxWidth = qMax(maxWidth, fm.horizontalAdvance(l));
  }

  int tooltipWidth = maxWidth + padding * 2;
  int tooltipHeight = lineas.size() * lineHeight + padding * 2;

  int x = pos.x() + 15;
  int y = pos.y() + 15;

  // Ajustar si sale de la pantalla
  if (x + tooltipWidth > width())
    x = pos.x() - tooltipWidth - 15;
  if (y + tooltipHeight > height())
    y = pos.y() - tooltipHeight - 15;

  // Fondo glassmorphism
  QRectF bgRect(x, y, tooltipWidth, tooltipHeight);
  painter.setBrush(QColor(255, 255, 255, 240));
  painter.setPen(QPen(QColor("#2563eb"), 1));
  painter.drawRoundedRect(bgRect, 8, 8);

  // Texto
  painter.setPen(QColor("#1f2937"));
  int ty = y + padding + fm.ascent();
  for (const QString &linea : lineas) {
    painter.drawText(x + padding, ty, linea);
    ty += lineHeight;
  }
}

void VisorOlap::dibujarLeyenda(QPainter &painter) {
  // Leyenda de colores en esquina inferior derecha
  int legendaW = 150;
  int legendaH = 80;
  int x = width() - legendaW - 20;
  int y = height() - legendaH - 20;

  // Fondo
  painter.setBrush(QColor(255, 255, 255, 230));
  painter.setPen(QPen(QColor("#e5e7eb"), 1));
  painter.drawRoundedRect(x, y, legendaW, legendaH, 8, 8);

  // Titulo
  painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
  painter.setPen(QColor("#374151"));
  painter.drawText(x + 10, y + 18, "Escala de Valores");

  // Gradiente de colores
  QLinearGradient grad(x + 10, y + 30, x + legendaW - 10, y + 30);
  grad.setColorAt(0, QColor("#10b981"));   // Verde = bajo
  grad.setColorAt(0.5, QColor("#f59e0b")); // Amarillo = medio
  grad.setColorAt(1, QColor("#ef4444"));   // Rojo = alto

  painter.setBrush(grad);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(x + 10, y + 30, legendaW - 20, 15, 4, 4);

  // Etiquetas
  painter.setFont(QFont("Segoe UI", 8));
  painter.setPen(QColor("#6b7280"));
  painter.drawText(x + 10, y + 60, QString("$%1").arg(m_valorMin, 0, 'f', 0));
  QString maxStr = QString("$%1").arg(m_valorMax, 0, 'f', 0);
  painter.drawText(x + legendaW - 10 -
                       painter.fontMetrics().horizontalAdvance(maxStr),
                   y + 60, maxStr);
}

void VisorOlap::dibujarInfoPanel(QPainter &painter) {
  // Panel de info en esquina superior izquierda
  painter.setFont(QFont("Segoe UI", 10));
  painter.setPen(QColor("#374151"));

  int selCount = 0;
  for (const auto &c : m_celdas) {
    if (c.seleccionada)
      selCount++;
  }

  QString info = QString("Celdas: %1  |  Seleccionadas: %2  |  Zoom: %3%")
                     .arg(m_celdas.size())
                     .arg(selCount)
                     .arg((int)(m_zoom * 100));

  QRectF bgRect(10, 10, painter.fontMetrics().horizontalAdvance(info) + 20, 28);
  painter.setBrush(QColor(255, 255, 255, 220));
  painter.setPen(QPen(QColor("#e5e7eb"), 1));
  painter.drawRoundedRect(bgRect, 6, 6);

  painter.setPen(QColor("#374151"));
  painter.drawText(bgRect.adjusted(10, 0, 0, 0), Qt::AlignVCenter, info);

  // Instrucciones
  QString instrucciones = "Click: Seleccionar | Ctrl+Click: Multi-seleccion | "
                          "Scroll: Zoom | Arrastrar: Pan";
  painter.setFont(QFont("Segoe UI", 9));
  painter.setPen(QColor("#6b7280"));
  painter.drawText(10, height() - 10, instrucciones);
}

// ============================================================================
// INTERACCION
// ============================================================================

void VisorOlap::mousePressEvent(QMouseEvent *event) {
  m_lastMousePos = event->pos();

  if (event->button() == Qt::LeftButton) {
    int idx = detectarCeldaEnPunto(event->pos());

    if (idx >= 0) {
      bool ctrl = event->modifiers() & Qt::ControlModifier;

      if (!ctrl) {
        // Sin Ctrl: limpiar seleccion anterior
        for (auto &c : m_celdas)
          c.seleccionada = false;
      }

      m_celdas[idx].seleccionada = !m_celdas[idx].seleccionada;

      QString info = QString("Seleccionado: %1 | %2 | %3 - Valor: $%4")
                         .arg(m_celdas[idx].nombreDimX)
                         .arg(m_celdas[idx].nombreDimY)
                         .arg(m_celdas[idx].nombreDimZ)
                         .arg(m_celdas[idx].valor, 0, 'f', 2);

      emit celdaSeleccionada(info);

      int count = 0;
      for (const auto &c : m_celdas)
        if (c.seleccionada)
          count++;
      emit seleccionCambiada(count);

      update();
    } else {
      m_arrastrando = true;
    }
  }
}

void VisorOlap::mouseMoveEvent(QMouseEvent *event) {
  if (m_arrastrando && (event->buttons() & Qt::LeftButton)) {
    // Pan
    m_panOffset += QPointF(event->pos() - m_lastMousePos);
    m_lastMousePos = event->pos();
    update();
  } else {
    // Hover detection
    int idx = detectarCeldaEnPunto(event->pos());
    if (idx != m_celdaHover) {
      m_celdaHover = idx;
      update();
    }
  }
}

void VisorOlap::mouseDoubleClickEvent(QMouseEvent *event) {
  int idx = detectarCeldaEnPunto(event->pos());
  if (idx >= 0) {
    emit drillDown(m_celdas[idx].dimX, m_celdas[idx].dimY, m_celdas[idx].dimZ);
  }
}

void VisorOlap::wheelEvent(QWheelEvent *event) {
  float factor = event->angleDelta().y() > 0 ? 1.15f : 0.87f;
  float newZoom = m_zoom * factor;

  // Limitar zoom
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
  if (event->key() == Qt::Key_Escape) {
    limpiarSeleccion();
  } else if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
    m_zoom = qMin(4.0f, m_zoom * 1.15f);
    update();
  } else if (event->key() == Qt::Key_Minus) {
    m_zoom = qMax(0.3f, m_zoom * 0.87f);
    update();
  } else if (event->key() == Qt::Key_0) {
    m_zoom = 1.0f;
    m_panOffset = QPointF(0, 0);
    update();
  }
}

// ============================================================================
// UTILIDADES
// ============================================================================

QPointF VisorOlap::proyectarIso(float x, float y, float z) {
  float angle = 30.0f * M_PI / 180.0f;
  float cosA = qCos(angle);
  float sinA = qSin(angle);

  float sx = (x - z) * cosA;
  float sy = (x + z) * sinA - y;

  return QPointF(sx, sy);
}

QColor VisorOlap::obtenerColorGradiente(double valor, double min, double max) {
  if (max <= min)
    return QColor("#10b981");

  double t = (valor - min) / (max - min);
  t = qBound(0.0, t, 1.0);

  // Gradiente: verde -> amarillo -> rojo
  if (t < 0.5) {
    // Verde a amarillo
    double u = t * 2;
    int r = (int)(16 + u * (245 - 16));
    int g = (int)(185 + u * (158 - 185));
    int b = (int)(129 + u * (11 - 129));
    return QColor(r, g, b);
  } else {
    // Amarillo a rojo
    double u = (t - 0.5) * 2;
    int r = (int)(245 + u * (239 - 245));
    int g = (int)(158 - u * 158);
    int b = (int)(11 + u * (68 - 11));
    return QColor(r, g, b);
  }
}

int VisorOlap::detectarCeldaEnPunto(const QPoint &punto) {
  // Transformar punto a coordenadas del visor
  QPointF p = punto - QPointF(width() / 2, height() / 2) - m_panOffset;
  p /= m_zoom;

  // Buscar celda (de atras hacia adelante para prioridad correcta)
  for (int i = m_celdas.size() - 1; i >= 0; i--) {
    const CeldaCubo &c = m_celdas[i];
    if (c.areaPantalla.contains(p)) {
      return i;
    }
  }

  return -1;
}

void VisorOlap::recalcularAreasPantalla() {
  // Se recalculan durante el renderizado
  update();
}
