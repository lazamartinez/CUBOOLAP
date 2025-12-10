#include "PantallaIntroduccion.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

PantallaIntroduccion::PantallaIntroduccion(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PantallaIntroduccion::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Fondo blanco con gradiente sutil
  QLinearGradient grad(0, 0, width(), height());
  grad.setColorAt(0, QColor("#ffffff"));
  grad.setColorAt(0.5, QColor("#f8fafc"));
  grad.setColorAt(1, QColor("#f1f5f9"));
  painter.fillRect(rect(), grad);

  // Decoracion superior - onda azul suave
  QPainterPath wave;
  wave.moveTo(0, 0);
  wave.lineTo(width(), 0);
  wave.lineTo(width(), 80);
  wave.quadTo(width() * 0.75, 100, width() * 0.5, 85);
  wave.quadTo(width() * 0.25, 70, 0, 90);
  wave.closeSubpath();

  QLinearGradient waveGrad(0, 0, width(), 0);
  waveGrad.setColorAt(0, QColor("#2563eb"));
  waveGrad.setColorAt(1, QColor("#6366f1"));
  painter.fillPath(wave, waveGrad);
}

void PantallaIntroduccion::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // Espaciador para la onda decorativa
  mainLayout->addSpacing(100);

  // Contenido centrado
  QWidget *content = new QWidget(this);
  QVBoxLayout *contentLayout = new QVBoxLayout(content);
  contentLayout->setAlignment(Qt::AlignCenter);
  contentLayout->setSpacing(24);
  contentLayout->setContentsMargins(40, 20, 40, 40);

  // Logo/Titulo
  QLabel *lblLogo = new QLabel("CUBO VISION", content);
  lblLogo->setStyleSheet(R"(
    font-size: 36px;
    font-weight: 800;
    color: #1e3a8a;
    letter-spacing: 4px;
  )");
  lblLogo->setAlignment(Qt::AlignCenter);
  contentLayout->addWidget(lblLogo);

  QLabel *lblSubtitulo =
      new QLabel("Sistema de Analisis OLAP Multidimensional", content);
  lblSubtitulo->setStyleSheet("font-size: 14px; color: #64748b;");
  lblSubtitulo->setAlignment(Qt::AlignCenter);
  contentLayout->addWidget(lblSubtitulo);

  contentLayout->addSpacing(20);

  // Descripcion breve
  QLabel *lblDesc =
      new QLabel("Transforme sus datos en decisiones inteligentes.\n"
                 "Analice, modele y visualice cubos OLAP de forma intuitiva.",
                 content);
  lblDesc->setStyleSheet("font-size: 13px; color: #475569; line-height: 1.6;");
  lblDesc->setAlignment(Qt::AlignCenter);
  lblDesc->setWordWrap(true);
  contentLayout->addWidget(lblDesc);

  contentLayout->addSpacing(24);

  // Fases en fila horizontal
  QHBoxLayout *fasesLayout = new QHBoxLayout();
  fasesLayout->setSpacing(16);
  fasesLayout->setAlignment(Qt::AlignCenter);

  fasesLayout->addWidget(crearTarjetaFase(
      1, "Diagnostico", "Analisis automatico\ndel esquema", "1", "#2563eb"));

  // Flecha
  QLabel *arrow1 = new QLabel(">", content);
  arrow1->setStyleSheet("font-size: 24px; color: #cbd5e1;");
  fasesLayout->addWidget(arrow1);

  fasesLayout->addWidget(crearTarjetaFase(
      2, "Modelado", "Definir dimensiones\ny medidas", "2", "#6366f1"));

  QLabel *arrow2 = new QLabel(">", content);
  arrow2->setStyleSheet("font-size: 24px; color: #cbd5e1;");
  fasesLayout->addWidget(arrow2);

  fasesLayout->addWidget(crearTarjetaFase(
      3, "Carga", "Procesamiento\noptimizado", "3", "#10b981"));

  QLabel *arrow3 = new QLabel(">", content);
  arrow3->setStyleSheet("font-size: 24px; color: #cbd5e1;");
  fasesLayout->addWidget(arrow3);

  fasesLayout->addWidget(crearTarjetaFase(
      4, "Explorar", "Visualizacion\n3D interactiva", "4", "#f59e0b"));

  QLabel *arrow4 = new QLabel(">", content);
  arrow4->setStyleSheet("font-size: 24px; color: #cbd5e1;");
  fasesLayout->addWidget(arrow4);

  fasesLayout->addWidget(crearTarjetaFase(
      5, "Reportes", "Consultas y\nexportacion", "5", "#ef4444"));

  contentLayout->addLayout(fasesLayout);

  contentLayout->addSpacing(32);

  // Boton de inicio
  QPushButton *btnIniciar = new QPushButton("Comenzar Ahora", content);
  btnIniciar->setMinimumSize(200, 48);
  btnIniciar->setCursor(Qt::PointingHandCursor);
  btnIniciar->setStyleSheet(R"(
    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #2563eb, stop:1 #6366f1);
      border: none;
      border-radius: 24px;
      color: white;
      font-size: 14px;
      font-weight: 600;
      padding: 12px 32px;
    }
    QPushButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #1d4ed8, stop:1 #4f46e5);
    }
  )");
  connect(btnIniciar, &QPushButton::clicked, this,
          &PantallaIntroduccion::iniciarSistema);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  btnLayout->setAlignment(Qt::AlignCenter);
  btnLayout->addWidget(btnIniciar);
  contentLayout->addLayout(btnLayout);

  contentLayout->addStretch();

  // Footer
  QLabel *lblFooter = new QLabel("Version 1.0 | PostgreSQL + Qt6", content);
  lblFooter->setStyleSheet("font-size: 11px; color: #94a3b8;");
  lblFooter->setAlignment(Qt::AlignCenter);
  contentLayout->addWidget(lblFooter);

  mainLayout->addWidget(content);
}

QWidget *PantallaIntroduccion::crearTarjetaFase(int numero,
                                                const QString &titulo,
                                                const QString &descripcion,
                                                const QString &icono,
                                                const QString &color) {
  QWidget *card = new QWidget(this);
  card->setFixedSize(120, 130);
  card->setStyleSheet(QString(R"(
    QWidget {
      background: white;
      border: 2px solid %1;
      border-radius: 12px;
    }
    QWidget:hover {
      background: %1;
    }
  )")
                          .arg(color));

  QVBoxLayout *layout = new QVBoxLayout(card);
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(6);
  layout->setContentsMargins(8, 12, 8, 12);

  // Numero/Icono
  QLabel *lblNum = new QLabel(icono, card);
  lblNum->setStyleSheet(QString(R"(
    font-size: 28px;
    font-weight: 800;
    color: %1;
    background: transparent;
    border: none;
  )")
                            .arg(color));
  lblNum->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblNum);

  // Titulo
  QLabel *lblTitulo = new QLabel(titulo, card);
  lblTitulo->setStyleSheet(R"(
    font-size: 12px;
    font-weight: 700;
    color: #1e293b;
    background: transparent;
    border: none;
  )");
  lblTitulo->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblTitulo);

  // Descripcion
  QLabel *lblDesc = new QLabel(descripcion, card);
  lblDesc->setStyleSheet(R"(
    font-size: 10px;
    color: #64748b;
    background: transparent;
    border: none;
  )");
  lblDesc->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblDesc);

  return card;
}
