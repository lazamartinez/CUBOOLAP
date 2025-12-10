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
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(24, 24, 24, 24);

  // Header
  QLabel *lblTitulo = new QLabel("Fase 2: Modelado Multidimensional", this);
  lblTitulo->setStyleSheet(Estilos::obtenerEstiloTituloSeccion("#2563eb"));
  mainLayout->addWidget(lblTitulo);

  QLabel *lblDescripcion =
      new QLabel("Defina las dimensiones y medidas para su cubo OLAP. "
                 "Arrastre columnas desde las tablas detectadas hacia los "
                 "paneles correspondientes.",
                 this);
  lblDescripcion->setWordWrap(true);
  lblDescripcion->setStyleSheet(
      "color: #6b7280; font-size: 13px; margin-bottom: 12px;");
  mainLayout->addWidget(lblDescripcion);

  // Contenido principal dividido en 3 paneles
  QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
  splitter->setStyleSheet(R"(
    QSplitter::handle {
      background: #e5e7eb;
      width: 2px;
    }
    QSplitter::handle:hover {
      background: #2563eb;
    }
  )");

  // Panel de Dimensiones
  QWidget *dimContainer = new QWidget(this);
  dimContainer->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *dimLayout = new QVBoxLayout(dimContainer);

  QLabel *lblDim = new QLabel("Dimensiones", dimContainer);
  lblDim->setStyleSheet("font-size: 15px; font-weight: 600; color: #2563eb;");
  dimLayout->addWidget(lblDim);

  panelDimensiones = new PanelDimensiones(dimContainer);
  dimLayout->addWidget(panelDimensiones);
  splitter->addWidget(dimContainer);

  // Panel de Medidas
  QWidget *medContainer = new QWidget(this);
  medContainer->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *medLayout = new QVBoxLayout(medContainer);

  QLabel *lblMed = new QLabel("Medidas", medContainer);
  lblMed->setStyleSheet("font-size: 15px; font-weight: 600; color: #10b981;");
  medLayout->addWidget(lblMed);

  panelMedidas = new PanelMedidas(medContainer);
  medLayout->addWidget(panelMedidas);
  splitter->addWidget(medContainer);

  // Panel de Simulador de Rendimiento
  QWidget *simContainer = new QWidget(this);
  simContainer->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *simLayout = new QVBoxLayout(simContainer);

  QLabel *lblSim = new QLabel("Simulador de Rendimiento", simContainer);
  lblSim->setStyleSheet("font-size: 15px; font-weight: 600; color: #f59e0b;");
  simLayout->addWidget(lblSim);

  SimuladorRendimiento *simulador = new SimuladorRendimiento(simContainer);
  simLayout->addWidget(simulador);
  splitter->addWidget(simContainer);

  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 2);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter);

  // Footer con acciones
  QHBoxLayout *footerLayout = new QHBoxLayout();
  footerLayout->setSpacing(12);

  // Info de modelo
  QLabel *lblModeloInfo =
      new QLabel("Modelo: 4 dimensiones, 6 medidas configuradas", this);
  lblModeloInfo->setStyleSheet("color: #6b7280; font-size: 12px;");
  footerLayout->addWidget(lblModeloInfo);

  footerLayout->addStretch();

  QPushButton *btnValidar = new QPushButton("Validar Modelo", this);
  btnValidar->setStyleSheet(R"(
    QPushButton {
      background: #f3f4f6;
      border: 1px solid #d1d5db;
      border-radius: 8px;
      padding: 10px 20px;
      color: #374151;
      font-weight: 600;
    }
    QPushButton:hover {
      background: #e5e7eb;
      border-color: #2563eb;
    }
  )");
  connect(btnValidar, &QPushButton::clicked, this, [this]() {
    // Simular validacion
  });
  footerLayout->addWidget(btnValidar);

  btnConfirmarModelo = new QPushButton("Construir Cubo OLAP", this);
  btnConfirmarModelo->setMinimumHeight(44);
  btnConfirmarModelo->setCursor(Qt::PointingHandCursor);
  btnConfirmarModelo->setStyleSheet(Estilos::obtenerEstiloBotonExito());
  connect(btnConfirmarModelo, &QPushButton::clicked, this,
          &EstudioModelado::modeloConfirmado);
  footerLayout->addWidget(btnConfirmarModelo);

  mainLayout->addLayout(footerLayout);
}
