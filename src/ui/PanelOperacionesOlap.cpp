#include "PanelOperacionesOlap.h"
#include "Estilos.h"

PanelOperacionesOlap::PanelOperacionesOlap(QWidget *parent) : QWidget(parent) {
  configurarUi();
  aplicarEstilos();
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
  setStyleSheet(R"(
        PanelOperacionesOlap {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1e293b, stop:1 #0f172a);
            border-bottom: 1px solid #334155;
        }
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3b82f6, stop:1 #2563eb);
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: 600;
            font-size: 11px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #60a5fa, stop:1 #3b82f6);
        }
        QPushButton:pressed {
            background: #1d4ed8;
        }
        QPushButton#btnReset {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #64748b, stop:1 #475569);
        }
        QPushButton#btnReset:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #94a3b8, stop:1 #64748b);
        }
        QPushButton#btnDrillThrough {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #10b981, stop:1 #059669);
        }
        QPushButton#btnDrillThrough:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #34d399, stop:1 #10b981);
        }
        QLabel {
            color: #94a3b8;
            font-size: 11px;
        }
    )");

  btnReset->setObjectName("btnReset");
  btnDrillThrough->setObjectName("btnDrillThrough");
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
