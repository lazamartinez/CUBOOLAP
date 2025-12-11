#include "EstudioModelado.h"
#include "Estilos.h"
#include "PanelDimensiones.h"
#include "PanelMedidas.h"
#include "SimuladorRendimiento.h"
#include <QHBoxLayout>

EstudioModelado::EstudioModelado(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

EstudioModelado::~EstudioModelado() {}

void EstudioModelado::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(8);
  mainLayout->setContentsMargins(16, 12, 16, 12);

  // Header compacto
  QLabel *lblTitulo = new QLabel("Fase 2: Modelado Multidimensional", this);
  lblTitulo->setStyleSheet(R"(
    font-size: 16px;
    font-weight: 700;
    color: #1f2937;
    padding-bottom: 4px;
    border-bottom: 2px solid #2563eb;
  )");
  mainLayout->addWidget(lblTitulo);

  // Contenido dividido en 3 paneles - OCUPA TODO EL ESPACIO
  QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
  splitter->setStyleSheet(
      "QSplitter::handle { background: #e5e7eb; width: 1px; }");

  // Panel Dimensiones
  QWidget *dimContainer = new QWidget(this);
  dimContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *dimLayout = new QVBoxLayout(dimContainer);
  dimLayout->setContentsMargins(8, 8, 8, 8);
  dimLayout->setSpacing(6);

  QLabel *lblDim = new QLabel("Dimensiones", dimContainer);
  lblDim->setStyleSheet("font-size: 12px; font-weight: 600; color: #2563eb;");
  dimLayout->addWidget(lblDim);

  panelDimensiones = new PanelDimensiones(dimContainer);
  dimLayout->addWidget(panelDimensiones, 1); // Stretch factor 1
  splitter->addWidget(dimContainer);

  // Panel Medidas
  QWidget *medContainer = new QWidget(this);
  medContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *medLayout = new QVBoxLayout(medContainer);
  medLayout->setContentsMargins(8, 8, 8, 8);
  medLayout->setSpacing(6);

  QLabel *lblMed = new QLabel("Medidas", medContainer);
  lblMed->setStyleSheet("font-size: 12px; font-weight: 600; color: #10b981;");
  medLayout->addWidget(lblMed);

  panelMedidas = new PanelMedidas(medContainer);
  medLayout->addWidget(panelMedidas, 1); // Stretch factor 1
  splitter->addWidget(medContainer);

  // Panel Simulador
  QWidget *simContainer = new QWidget(this);
  simContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *simLayout = new QVBoxLayout(simContainer);
  simLayout->setContentsMargins(8, 8, 8, 8);
  simLayout->setSpacing(6);

  QLabel *lblSim = new QLabel("Rendimiento", simContainer);
  lblSim->setStyleSheet("font-size: 12px; font-weight: 600; color: #f59e0b;");
  simLayout->addWidget(lblSim);

  SimuladorRendimiento *simulador = new SimuladorRendimiento(simContainer);
  simLayout->addWidget(simulador, 1); // Stretch factor 1
  splitter->addWidget(simContainer);

  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 2);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(
      splitter, 1); // Stretch factor 1 - OCUPA TODO EL ESPACIO DISPONIBLE

  // Footer
  QHBoxLayout *footerLayout = new QHBoxLayout();
  footerLayout->addStretch();

  btnConfirmarModelo = new QPushButton("📦 Construir Cubo OLAP", this);
  btnConfirmarModelo->setMinimumHeight(36);
  btnConfirmarModelo->setCursor(Qt::PointingHandCursor);
  btnConfirmarModelo->setStyleSheet(R"(
    QPushButton {
      background: #10b981;
      border: none;
      border-radius: 6px;
      color: white;
      font-size: 12px;
      font-weight: 600;
      padding: 0 24px;
    }
    QPushButton:hover { background: #059669; }
  )");
  connect(btnConfirmarModelo, &QPushButton::clicked, this,
          &EstudioModelado::modeloConfirmado);
  footerLayout->addWidget(btnConfirmarModelo);

  mainLayout->addLayout(footerLayout);
}
