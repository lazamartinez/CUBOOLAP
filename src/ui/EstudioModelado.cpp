#include "EstudioModelado.h"
#include "PanelDimensiones.h"
#include "PanelMedidas.h"
#include "SimuladorRendimiento.h"
#include "styles/FlutterTheme.h"
#include <QHBoxLayout>

EstudioModelado::EstudioModelado(QWidget *parent) : QWidget(parent) {
  configurarUi();
  FlutterTheme::instance().applyThemeToWidget(this);
}

EstudioModelado::~EstudioModelado() {}

void EstudioModelado::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header Title
  QLabel *lblTitulo = new QLabel("Fase 2: Modelado Multidimensional", this);
  lblTitulo->setStyleSheet(
      "font-size: 20px; font-weight: 700; color: #1f2937;");
  mainLayout->addWidget(lblTitulo);

  // Contenido dividido en 3 paneles - OCUPA TODO EL ESPACIO
  QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
  splitter->setStyleSheet(
      "QSplitter::handle { background: #e5e7eb; width: 1px; }");

  // Panel Dimensiones
  FlutterCard *dimContainer = new FlutterCard(this);
  QVBoxLayout *dimLayout = new QVBoxLayout(dimContainer);
  dimLayout->setContentsMargins(12, 12, 12, 12);
  dimLayout->setSpacing(8);

  QLabel *lblDim = new QLabel("Dimensiones", dimContainer);
  lblDim->setStyleSheet("font-size: 14px; font-weight: 600; color: #2563eb; "
                        "text-transform: uppercase;");
  dimLayout->addWidget(lblDim);

  panelDimensiones = new PanelDimensiones(dimContainer);
  dimLayout->addWidget(panelDimensiones, 1);
  splitter->addWidget(dimContainer);

  // Panel Medidas
  FlutterCard *medContainer = new FlutterCard(this);
  QVBoxLayout *medLayout = new QVBoxLayout(medContainer);
  medLayout->setContentsMargins(12, 12, 12, 12);
  medLayout->setSpacing(8);

  QLabel *lblMed = new QLabel("Medidas", medContainer);
  lblMed->setStyleSheet("font-size: 14px; font-weight: 600; color: #10b981; "
                        "text-transform: uppercase;");
  medLayout->addWidget(lblMed);

  panelMedidas = new PanelMedidas(medContainer);
  medLayout->addWidget(panelMedidas, 1);
  splitter->addWidget(medContainer);

  // Panel Simulador
  FlutterCard *simContainer = new FlutterCard(this);
  QVBoxLayout *simLayout = new QVBoxLayout(simContainer);
  simLayout->setContentsMargins(12, 12, 12, 12);
  simLayout->setSpacing(8);

  QLabel *lblSim = new QLabel("Rendimiento", simContainer);
  lblSim->setStyleSheet("font-size: 14px; font-weight: 600; color: #f59e0b; "
                        "text-transform: uppercase;");
  simLayout->addWidget(lblSim);

  SimuladorRendimiento *simulador = new SimuladorRendimiento(simContainer);
  simLayout->addWidget(simulador, 1);
  splitter->addWidget(simContainer);

  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 2);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter, 1);

  // Footer
  QHBoxLayout *footerLayout = new QHBoxLayout();
  footerLayout->addStretch();

  btnConfirmarModelo = new FlutterFilledButton("📦 Construir Cubo OLAP", this);
  btnConfirmarModelo->setMinimumHeight(48);

  connect(btnConfirmarModelo, &QPushButton::clicked, this,
          &EstudioModelado::modeloConfirmado);
  footerLayout->addWidget(btnConfirmarModelo);

  mainLayout->addLayout(footerLayout);
}
