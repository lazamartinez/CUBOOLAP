#include "IndicadorFase.h"
#include <QPainterPath>

IndicadorFase::IndicadorFase(QWidget *parent) : QWidget(parent) {
  setFixedHeight(60);
  setMinimumWidth(500);
}

void IndicadorFase::setFaseActual(int fase) {
  if (m_faseActual != fase) {
    m_faseActual = fase;
    update();
  }
}

void IndicadorFase::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  int numFases = 5;
  int circleRadius = 16;
  int lineHeight = 4;
  int spacing = (width() - 80) / (numFases - 1);
  int startX = 40;
  int centerY = 25;

  // Colores
  QColor colorCompletado("#10b981");
  QColor colorActual("#2563eb");
  QColor colorPendiente("#e2e8f0");
  QColor colorTextoPendiente("#94a3b8");
  QColor colorTextoActivo("#1e293b");

  // Dibujar lineas de conexion
  for (int i = 0; i < numFases - 1; i++) {
    int x1 = startX + i * spacing + circleRadius;
    int x2 = startX + (i + 1) * spacing - circleRadius;

    QColor lineColor = (i < m_faseActual) ? colorCompletado : colorPendiente;
    p.setPen(Qt::NoPen);
    p.setBrush(lineColor);
    p.drawRect(x1, centerY - lineHeight / 2, x2 - x1, lineHeight);
  }

  // Dibujar circulos y texto
  p.setFont(QFont("Segoe UI", 9, QFont::Bold));

  for (int i = 0; i < numFases; i++) {
    int x = startX + i * spacing;

    QColor circleColor;
    QColor textColor;

    if (i + 1 < m_faseActual) {
      // Completado
      circleColor = colorCompletado;
      textColor = colorTextoActivo;
    } else if (i + 1 == m_faseActual) {
      // Actual
      circleColor = colorActual;
      textColor = colorTextoActivo;
    } else {
      // Pendiente
      circleColor = colorPendiente;
      textColor = colorTextoPendiente;
    }

    // Circulo
    p.setPen(Qt::NoPen);
    p.setBrush(circleColor);
    p.drawEllipse(QPointF(x, centerY), circleRadius, circleRadius);

    // Numero
    p.setPen(i + 1 == m_faseActual || i + 1 < m_faseActual
                 ? Qt::white
                 : colorTextoPendiente);
    p.setFont(QFont("Segoe UI", 10, QFont::Bold));
    p.drawText(QRect(x - circleRadius, centerY - circleRadius, circleRadius * 2,
                     circleRadius * 2),
               Qt::AlignCenter, QString::number(i + 1));

    // Nombre de fase
    p.setPen(textColor);
    p.setFont(QFont("Segoe UI", 8));
    QRect textRect(x - 40, centerY + circleRadius + 4, 80, 20);
    p.drawText(textRect, Qt::AlignCenter, m_nombresFases[i]);
  }
}
