#include "EstudioModelado.h"
#include "PanelDimensiones.h"
#include "PanelMedidas.h"
#include <QHBoxLayout>

EstudioModelado::EstudioModelado(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

EstudioModelado::~EstudioModelado() {}

void EstudioModelado::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Header
  QLabel *lblTitulo =
      new QLabel("Fase 2: Estudio de Modelado Multidimensional", this);
  lblTitulo->setStyleSheet(
      "font-size: 18px; font-weight: bold; color: #4FC3F7;");
  mainLayout->addWidget(lblTitulo);

  // Contenido principal dividido
  QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

  panelDimensiones = new PanelDimensiones(this);
  panelMedidas = new PanelMedidas(this);

  splitter->addWidget(panelDimensiones);
  splitter->addWidget(panelMedidas);
  // TODO: Agregar simulador de rendimiento aquí como 3er panel

  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);

  // Footer con acciones
  QHBoxLayout *footerLayout = new QHBoxLayout();
  footerLayout->addStretch();

  btnConfirmarModelo = new QPushButton("Construir Cubo (Fase 3)", this);
  btnConfirmarModelo->setStyleSheet(
      "background-color: #007ACC; color: white; padding: 10px;");
  connect(btnConfirmarModelo, &QPushButton::clicked, this,
          &EstudioModelado::modeloConfirmado);

  footerLayout->addWidget(btnConfirmarModelo);
  mainLayout->addLayout(footerLayout);
}
