#include "PantallaIntroduccion.h"
#include <QApplication>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

PantallaIntroduccion::PantallaIntroduccion(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void PantallaIntroduccion::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  // Fondo blanco limpio
  p.fillRect(rect(), Qt::white);

  // Onda decorativa azul-violeta en la parte inferior
  QPainterPath path;
  path.moveTo(0, height());
  path.lineTo(0, height() * 0.75);
  path.cubicTo(width() * 0.25, height() * 0.65, width() * 0.75, height() * 0.85,
               width(), height() * 0.75);
  path.lineTo(width(), height());
  path.closeSubpath();

  QLinearGradient grad(0, height() * 0.6, width(), height());
  grad.setColorAt(0, QColor(99, 102, 241, 100)); // Indigo suave
  grad.setColorAt(1, QColor(168, 85, 247, 100)); // Violeta suave
  p.fillPath(path, grad);
}

void PantallaIntroduccion::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(30);
  layout->setAlignment(Qt::AlignCenter);

  // --- LOGO / CUBO 3D ---
  // --- LOGO / CUBO 3D ---
  QLabel *lblLogo = new QLabel(this);

  // Buscar logo en varias ubicaciones posibles
  QStringList paths = {
      QDir::currentPath() + "/src/ui/assets/logo.png",      // Dev mode
      QCoreApplication::applicationDirPath() + "/logo.png", // Distributed
      QCoreApplication::applicationDirPath() +
          "/src/ui/assets/logo.png",                        // Inside build
      "C:/Proyectos Facu/OLAPBD2025/src/ui/assets/logo.png" // Absolute fallback
  };

  QPixmap logo;
  for (const QString &p : paths) {
    if (QFile::exists(p)) {
      logo.load(p);
      break;
    }
  }

  if (!logo.isNull()) {
    lblLogo->setPixmap(
        logo.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  } else {
    // Fallback si no hay imagen: Dibujar un cubo simple con texto
    lblLogo->setText("⬢"); // Hexagon/Cube UTF-8 char as placeholder
    lblLogo->setStyleSheet("font-size: 100px; color: #6366f1;");
  }
  lblLogo->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblLogo);

  // Titulo
  QLabel *lblTitulo = new QLabel("CUBO VISION", this);
  lblTitulo->setStyleSheet("font-size: 48px; font-weight: 900; color: #1e293b; "
                           "letter-spacing: 2px;");
  lblTitulo->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblTitulo);

  // Subtitulo
  QLabel *lblSub =
      new QLabel("Sistema de Analisis Multidimensional Inteligente", this);
  lblSub->setStyleSheet(
      "font-size: 18px; color: #64748b; margin-bottom: 20px;");
  lblSub->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblSub);

  // Texto académico
  QLabel *lblAcademico =
      new QLabel("Desarrollado para la materia de Bases de datos\n"
                 "Universidad Nacional de Misiones\n"
                 "Facultad de Ciencias Exactas Químicas y Naturales\n"
                 "Módulo de Apóstoles",
                 this);
  lblAcademico->setStyleSheet(
      "font-size: 12px; color: #475569; line-height: 1.6; "
      "margin-top: 10px; margin-bottom: 20px;");
  lblAcademico->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblAcademico);

  // Fases (Cards)
  QHBoxLayout *fasesLayout = new QHBoxLayout();
  fasesLayout->setSpacing(20);
  fasesLayout->setAlignment(Qt::AlignCenter);

  auto crearCard = [](const QString &titulo, const QString &icono) {
    QFrame *card = new QFrame();
    card->setFixedSize(140, 160);
    card->setStyleSheet(R"(
            QFrame {
                background: white;
                border: 1px solid #e2e8f0;
                border-radius: 12px;
            }
            QFrame:hover {
                border: 2px solid #6366f1;
                background: #f8fafc;
            }
        )");

    QVBoxLayout *vbox = new QVBoxLayout(card);
    QLabel *lblIcon = new QLabel(icono);
    lblIcon->setStyleSheet("font-size: 32px;");
    lblIcon->setAlignment(Qt::AlignCenter);

    QLabel *lblText = new QLabel(titulo);
    lblText->setStyleSheet(
        "font-size: 13px; font-weight: 600; color: #475569;");
    lblText->setAlignment(Qt::AlignCenter);
    lblText->setWordWrap(true);

    vbox->addWidget(lblIcon);
    vbox->addWidget(lblText);

    // Sombra
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    return card;
  };

  fasesLayout->addWidget(crearCard("1. Diagnostico", "🔍"));
  fasesLayout->addWidget(new QLabel("→"));
  fasesLayout->addWidget(crearCard("2. Modelado", "📐"));
  fasesLayout->addWidget(new QLabel("→"));
  fasesLayout->addWidget(crearCard("3. Carga", "⚡"));
  fasesLayout->addWidget(new QLabel("→"));
  fasesLayout->addWidget(crearCard("4. Explorar", "🧊"));
  fasesLayout->addWidget(new QLabel("→"));
  fasesLayout->addWidget(crearCard("5. Reportes", "📊"));

  layout->addLayout(fasesLayout);

  layout->addSpacing(40);

  // Boton Comenzar
  QPushButton *btnInicio = new QPushButton("Comenzar Ahora", this);
  btnInicio->setCursor(Qt::PointingHandCursor);
  btnInicio->setFixedSize(220, 60);
  btnInicio->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed);
            color: white;
            font-size: 18px;
            font-weight: bold;
            border-radius: 30px;
            border: none;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4338ca, stop:1 #6d28d9);
            margin-top: -2px;
        }
        QPushButton:pressed {
            margin-top: 2px;
        }
    )");

  // Sombra boton
  QGraphicsDropShadowEffect *btnShadow = new QGraphicsDropShadowEffect();
  btnShadow->setBlurRadius(20);
  btnShadow->setColor(QColor(79, 70, 229, 80));
  btnShadow->setOffset(0, 8);
  btnInicio->setGraphicsEffect(btnShadow);

  connect(btnInicio, &QPushButton::clicked, this,
          &PantallaIntroduccion::iniciarSistema);
  layout->addWidget(btnInicio);

  layout->addStretch();

  QLabel *lblVersion = new QLabel("v1.0.0 - Build 2025", this);
  lblVersion->setStyleSheet("color: #94a3b8; font-size: 11px;");
  layout->addWidget(lblVersion);
}
