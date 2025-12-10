#include "ToastNotifier.h"
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QScreen>

void ToastNotifier::mostrar(QWidget *parent, const QString &mensaje,
                            TipoToast tipo, int duracion) {
  new ToastNotifier(parent, mensaje, tipo, duracion);
}

ToastNotifier::ToastNotifier(QWidget *parent, const QString &mensaje,
                             TipoToast tipo, int duracion)
    : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);

  configurarUi(mensaje, tipo);
  iniciarAnimacion(duracion);
}

void ToastNotifier::configurarUi(const QString &mensaje, TipoToast tipo) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(16, 12, 16, 12);
  layout->setSpacing(12);

  // Icono
  QLabel *lblIcono = new QLabel(obtenerIcono(tipo), this);
  lblIcono->setStyleSheet(
      QString("font-size: 18px; color: %1;").arg(obtenerColor(tipo)));
  layout->addWidget(lblIcono);

  // Mensaje
  QLabel *lblMensaje = new QLabel(mensaje, this);
  lblMensaje->setStyleSheet(
      "font-size: 13px; color: #1e293b; font-weight: 500;");
  lblMensaje->setWordWrap(true);
  lblMensaje->setMaximumWidth(300);
  layout->addWidget(lblMensaje);

  // Estilo del contenedor
  QString borderColor = obtenerColor(tipo);
  setStyleSheet(QString(R"(
    ToastNotifier {
      background: white;
      border: 1px solid #e2e8f0;
      border-left: 4px solid %1;
      border-radius: 8px;
    }
  )")
                    .arg(borderColor));

  // Sombra
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(20);
  shadow->setColor(QColor(0, 0, 0, 40));
  shadow->setOffset(0, 4);
  setGraphicsEffect(shadow);

  adjustSize();

  // Posicionar en esquina superior derecha
  if (parentWidget()) {
    QPoint parentPos = parentWidget()->mapToGlobal(QPoint(0, 0));
    int x = parentPos.x() + parentWidget()->width() - width() - 20;
    int y = parentPos.y() + 60;
    move(x, y);
  } else if (QScreen *screen = QApplication::primaryScreen()) {
    QRect geo = screen->availableGeometry();
    move(geo.right() - width() - 20, geo.top() + 60);
  }

  show();
}

void ToastNotifier::iniciarAnimacion(int duracion) {
  // Fade in
  setWindowOpacity(0);
  QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "opacity");
  fadeIn->setDuration(200);
  fadeIn->setStartValue(0.0);
  fadeIn->setEndValue(1.0);
  fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

  // Timer para fade out
  QTimer::singleShot(duracion, this, [this]() {
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "opacity");
    fadeOut->setDuration(300);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    connect(fadeOut, &QPropertyAnimation::finished, this, &QWidget::close);
    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

QString ToastNotifier::obtenerIcono(TipoToast tipo) {
  switch (tipo) {
  case Exito:
    return "✓";
  case Advertencia:
    return "⚠";
  case Error:
    return "✕";
  default:
    return "ℹ";
  }
}

QString ToastNotifier::obtenerColor(TipoToast tipo) {
  switch (tipo) {
  case Exito:
    return "#10b981";
  case Advertencia:
    return "#f59e0b";
  case Error:
    return "#ef4444";
  default:
    return "#2563eb";
  }
}
