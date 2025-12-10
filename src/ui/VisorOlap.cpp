#include "VisorOlap.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

VisorOlap::VisorOlap(QWidget *parent)
    : QWidget(parent), m_rotacionX(45.0f), m_rotacionY(45.0f), m_zoom(1.0f) {
  setFocusPolicy(Qt::StrongFocus);
  setBackgroundRole(QPalette::Dark);
  setAutoFillBackground(true);
}

VisorOlap::~VisorOlap() {}

void VisorOlap::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Fondo oscuro
  painter.fillRect(rect(), QColor(30, 30, 30));

  // Centro de la pantalla
  painter.translate(width() / 2, height() / 2);
  painter.scale(m_zoom, m_zoom);

  // Dibujar algunos prismas en axonometría (Isométrica simulada)
  // Coordenadas: X (derecha), Y (arriba), Z (profundidad/izquierda)

  // Datos Dummy
  dibujarPrisma(painter, 0, 0, 0, 50, 100, 50, QColor(0, 122, 204));
  dibujarPrisma(painter, 60, 0, 0, 50, 150, 50, QColor(40, 167, 69));
  dibujarPrisma(painter, 0, 0, 60, 50, 70, 50, QColor(220, 53, 69));
  dibujarPrisma(painter, -60, 0, 0, 50, 200, 50, QColor(255, 193, 7));
}

void VisorOlap::dibujarPrisma(QPainter &painter, float x, float y, float z,
                              float w, float h, float d, const QColor &color) {
  // Proyección Isométrica Simple:
  // x_screen = (x - z) * cos(30)
  // y_screen = (x + z) * sin(30) - y

  float angle = 30.0f * M_PI / 180.0f;
  float cosA = qCos(angle);
  float sinA = qSin(angle);

  auto iso = [&](float ix, float iy, float iz) -> QPointF {
    float sx = (ix - iz) * cosA;
    float sy = (ix + iz) * sinA - iy;
    return QPointF(sx, sy);
  };

  // Vértices base
  QPointF p0 = iso(x, y, z); // Centro Base
  QPointF p1 = iso(x + w / 2, y, z + d / 2);
  QPointF p2 = iso(x + w / 2, y, z - d / 2);
  QPointF p3 = iso(x - w / 2, y, z - d / 2);
  QPointF p4 = iso(x - w / 2, y, z + d / 2);

  // Vértices top
  QPointF t1 = iso(x + w / 2, y + h, z + d / 2);
  QPointF t2 = iso(x + w / 2, y + h, z - d / 2);
  QPointF t3 = iso(x - w / 2, y + h, z - d / 2);
  QPointF t4 = iso(x - w / 2, y + h, z + d / 2);

  // Caras (Top)
  QPainterPath pathTop;
  pathTop.moveTo(t1);
  pathTop.lineTo(t2);
  pathTop.lineTo(t3);
  pathTop.lineTo(t4);
  pathTop.closeSubpath();
  painter.setBrush(color.lighter(120));
  painter.drawPath(pathTop);

  // Caras (Derecha - x+)
  QPainterPath pathRight;
  pathRight.moveTo(p1);
  pathRight.lineTo(p2);
  pathRight.lineTo(t2);
  pathRight.lineTo(t1);
  pathRight.closeSubpath();
  painter.setBrush(color.darker(110));
  painter.drawPath(pathRight);

  // Caras (Izquierda - z+)
  QPainterPath pathLeft;
  pathLeft.moveTo(p4);
  pathLeft.lineTo(p1);
  pathLeft.lineTo(t1);
  pathLeft.lineTo(t4);
  pathLeft.closeSubpath();
  painter.setBrush(color.darker(130)); // Más sombra
  painter.drawPath(pathLeft);
}

void VisorOlap::mousePressEvent(QMouseEvent *event) {
  m_lastMousePos = event->pos();
  emit celdaSeleccionada("Celda [Estática] - Valor: $1000");
}

void VisorOlap::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - m_lastMousePos.x();
  int dy = event->y() - m_lastMousePos.y();

  if (event->buttons() & Qt::LeftButton) {
    // Rotación simulada o desplazamiento
  }
  m_lastMousePos = event->pos();
}

void VisorOlap::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() > 0)
    m_zoom *= 1.1f;
  else
    m_zoom /= 1.1f;
  update();
}

void VisorOlap::cargarCuboDummy() { update(); }
