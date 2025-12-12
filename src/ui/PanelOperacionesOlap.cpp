#include "PanelOperacionesOlap.h"
#include "styles/FlutterTheme.h"

PanelOperacionesOlap::PanelOperacionesOlap(QWidget *parent) : QWidget(parent) {
  configurarUi();
  aplicarEstilos();

  connect(&FlutterTheme::instance(), &FlutterTheme::themeChanged, this,
          &PanelOperacionesOlap::aplicarEstilos);
}

void PanelOperacionesOlap::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(8, 8, 8, 8);
  mainLayout->setSpacing(8);

  // Fila de botones de operaciones
  QHBoxLayout *botonesLayout = new QHBoxLayout();
  botonesLayout->setSpacing(6);

  btnDrillDown = new QPushButton("Drill Down", this);
  btnDrillDown->setToolTip(
      "Navegar a nivel mas detallado (Doble-click en celda)");
  btnDrillDown->setIcon(QIcon(":/icons/arrow-down.png"));

  btnRollUp = new QPushButton("Roll Up", this);
  btnRollUp->setToolTip("Volver a nivel mas general");
  btnRollUp->setIcon(QIcon(":/icons/arrow-up.png"));

  btnSlice = new QPushButton("Slice", this);
  btnSlice->setToolTip("Filtrar por una dimension");

  btnDice = new QPushButton("Dice", this);
  btnDice->setToolTip("Filtrar por multiples dimensiones");

  btnPivot = new QPushButton("Pivot", this);
  btnPivot->setToolTip("Rotar ejes X/Z del cubo");

  btnRanking = new QPushButton("Top N", this);
  btnRanking->setToolTip("Mostrar Top N / Bottom N");

  btnDrillThrough = new QPushButton("Detalles", this);
  btnDrillThrough->setToolTip("Ver registros individuales (transacciones)");

  btnReset = new QPushButton("Reset", this);
  btnReset->setToolTip("Volver a vista inicial (Tecla R)");

  botonesLayout->addWidget(btnDrillDown);
  botonesLayout->addWidget(btnRollUp);
  botonesLayout->addWidget(btnSlice);
  botonesLayout->addWidget(btnDice);
  botonesLayout->addWidget(btnPivot);
  botonesLayout->addWidget(btnRanking);
  botonesLayout->addWidget(btnDrillThrough);
  botonesLayout->addStretch();
  botonesLayout->addWidget(btnReset);

  mainLayout->addLayout(botonesLayout);

  // Fila de informacion de navegacion
  QHBoxLayout *infoLayout = new QHBoxLayout();

  lblNivel = new QLabel("Nivel: Vista general", this);
  lblFiltros = new QLabel("", this);

  infoLayout->addWidget(lblNivel);
  infoLayout->addStretch();
  infoLayout->addWidget(lblFiltros);

  mainLayout->addLayout(infoLayout);

  // Conectar senales
  connect(btnDrillDown, &QPushButton::clicked, this,
          &PanelOperacionesOlap::drillDown);
  connect(btnRollUp, &QPushButton::clicked, this,
          &PanelOperacionesOlap::rollUp);
  connect(btnSlice, &QPushButton::clicked, this, &PanelOperacionesOlap::slice);
  connect(btnDice, &QPushButton::clicked, this, &PanelOperacionesOlap::dice);
  connect(btnPivot, &QPushButton::clicked, this, &PanelOperacionesOlap::pivot);
  connect(btnRanking, &QPushButton::clicked, this,
          &PanelOperacionesOlap::ranking);
  connect(btnDrillThrough, &QPushButton::clicked, this,
          &PanelOperacionesOlap::drillThrough);
  connect(btnReset, &QPushButton::clicked, this,
          &PanelOperacionesOlap::resetView);
}

void PanelOperacionesOlap::aplicarEstilos() {
  bool dark = FlutterTheme::instance().darkMode();
  QString bg = dark ? "#111827" : "#ffffff";
  QString btnBg = dark ? "#3b82f6" : "#2563eb";
  QString btnText = "#ffffff";
  QString border = dark ? "#374151" : "#e2e8f0";
  QString text = dark ? "#94a3b8" : "#475569";

  setStyleSheet(QString(R"(
        PanelOperacionesOlap {
            background: %1;
            border-bottom: 1px solid %2;
        }
        QPushButton {
            background: %3;
            color: %4;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: 600;
            font-size: 12px;
        }
        QPushButton:hover {
            background: %3; /* Slightly reliable on opacity or just same color with effect */
            opacity: 0.9;
        }
        QPushButton#btnReset {
            background: #64748b;
        }
        QPushButton#btnDrillThrough {
            background: #10b981;
        }
        QLabel {
            color: %5;
            font-size: 12px;
            font-weight: 500;
        }
    )")
                    .arg(bg, border, btnBg, btnText, text));
}

void PanelOperacionesOlap::setDimensiones(const QStringList &dimensiones) {
  Q_UNUSED(dimensiones);
}

void PanelOperacionesOlap::setNivelActual(const QString &nivel) {
  lblNivel->setText("Nivel: " + nivel);
}

void PanelOperacionesOlap::setFiltrosActivos(const QStringList &filtros) {
  if (filtros.isEmpty()) {
    lblFiltros->setText("");
    lblFiltros->hide();
  } else {
    lblFiltros->setText("Filtros: " + filtros.join(" | "));
    lblFiltros->show();
    lblFiltros->setStyleSheet("color: #fbbf24; font-weight: 500;");
  }
}
