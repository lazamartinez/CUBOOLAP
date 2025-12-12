#include "PantallaIntroduccion.h"
#include "icons/MaterialIcons.h"
#include "styles/FlutterTheme.h"
#include "widgets/FlutterWidgets.h"
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
  FlutterTheme::instance().applyThemeToWidget(this);
}

void PantallaIntroduccion::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  // Background from theme
  bool dark = FlutterTheme::instance().darkMode();
  QColor bgColor = palette().color(QPalette::Window);
  p.fillRect(rect(), bgColor);

  // Decorative Wave (adapted for connection/modern look)
  QPainterPath path;
  path.moveTo(0, height());
  path.lineTo(0, height() * 0.75);
  path.cubicTo(width() * 0.25, height() * 0.65, width() * 0.75, height() * 0.85,
               width(), height() * 0.75);
  path.lineTo(width(), height());
  path.closeSubpath();

  QLinearGradient grad(0, height() * 0.6, width(), height());
  if (dark) {
    grad.setColorAt(0, QColor(103, 80, 164, 40)); // Primary low opacity
    grad.setColorAt(1, QColor(156, 39, 176, 40));
  } else {
    grad.setColorAt(0, QColor(99, 102, 241, 100)); // Indigo
    grad.setColorAt(1, QColor(168, 85, 247, 100)); // Violet
  }
  p.fillPath(path, grad);
}

void PantallaIntroduccion::configurarUi() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(30);
  layout->setAlignment(Qt::AlignCenter);

  // --- LOGO / HEADER ---
  QLabel *lblLogo = new QLabel(this);
  lblLogo->setText("⬢"); // Placeholder if no image
  lblLogo->setStyleSheet("font-size: 80px; color: #6750a4;"); // Primary color
  lblLogo->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblLogo);

  // Titulo
  QLabel *lblTitulo = new QLabel("CUBO VISION", this);
  lblTitulo->setStyleSheet(
      "font-size: 48px; font-weight: 900; letter-spacing: 2px;");
  lblTitulo->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblTitulo);

  // Subtitulo
  QLabel *lblSub =
      new QLabel("Sistema de Analisis Multidimensional Inteligente", this);
  lblSub->setStyleSheet("font-size: 18px; opacity: 0.8; margin-bottom: 20px;");
  lblSub->setAlignment(Qt::AlignCenter);
  layout->addWidget(lblSub);

  // Fases (Cards)
  QHBoxLayout *fasesLayout = new QHBoxLayout();
  fasesLayout->setSpacing(16);
  fasesLayout->setAlignment(Qt::AlignCenter);

  auto crearCard = [](const QString &titulo, const QString &icono) {
    FlutterCard *card = new FlutterCard(nullptr, 2);
    card->setFixedSize(140, 160);

    QVBoxLayout *vbox = new QVBoxLayout(card);
    QLabel *lblIcon = new QLabel(icono);
    lblIcon->setStyleSheet(
        "font-size: 32px; border: none; background: transparent;");
    lblIcon->setAlignment(Qt::AlignCenter);

    QLabel *lblText = new QLabel(titulo);
    lblText->setStyleSheet("font-size: 13px; font-weight: 600; border: none; "
                           "background: transparent;");
    lblText->setAlignment(Qt::AlignCenter);
    lblText->setWordWrap(true);

    vbox->addWidget(lblIcon);
    vbox->addWidget(lblText);

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
  FlutterElevatedButton *btnInicio =
      new FlutterElevatedButton("Comenzar Ahora", this);
  btnInicio->setFixedSize(220, 50);
  // Additional styling handled by FlutterTheme class but we can override fonts
  btnInicio->setStyleSheet(
      btnInicio->styleSheet() +
      "font-size: 18px; font-weight: bold; border-radius: 25px;");

  connect(btnInicio, &QPushButton::clicked, this,
          &PantallaIntroduccion::iniciarSistema);
  layout->addWidget(btnInicio);

  layout->addStretch();

  QLabel *lblVersion = new QLabel("v1.0.0 - Build 2025", this);
  lblVersion->setStyleSheet("font-size: 11px; opacity: 0.6;");
  layout->addWidget(lblVersion);
}
