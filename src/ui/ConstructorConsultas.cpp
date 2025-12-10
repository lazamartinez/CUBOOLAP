#include "ConstructorConsultas.h"

ConstructorConsultas::ConstructorConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  QLabel *header = new QLabel("Fase 5: Constructor de Consultas OLAP", this);
  header->setStyleSheet("font-size: 16px; font-weight: bold; color: #4FC3F7;");
  mainLayout->addWidget(header);

  QHBoxLayout *workspace = new QHBoxLayout();

  // Panel Izquierdo: Recursos
  QVBoxLayout *leftPanel = new QVBoxLayout();
  leftPanel->addWidget(new QLabel("Dimensiones:"));
  listaDimensiones = new QListWidget();
  listaDimensiones->addItems({"Tiempo", "Geografía", "Producto", "Cliente"});
  listaDimensiones->setDragEnabled(true);
  leftPanel->addWidget(listaDimensiones);

  leftPanel->addWidget(new QLabel("Medidas:"));
  listaMedidas = new QListWidget();
  listaMedidas->addItems({"Ventas", "Costo", "Beneficio", "Margen %"});
  listaMedidas->setDragEnabled(true);
  leftPanel->addWidget(listaMedidas);

  workspace->addLayout(leftPanel, 1);

  // Panel Central: Construcción
  QVBoxLayout *centerPanel = new QVBoxLayout();

  centerPanel->addWidget(new QLabel("Filas (Arrastrar aquí):"));
  areaFilas = new QListWidget();
  areaFilas->setAcceptDrops(true);
  centerPanel->addWidget(areaFilas);

  centerPanel->addWidget(new QLabel("Columnas (Arrastrar aquí):"));
  areaColumnas = new QListWidget();
  areaColumnas->setAcceptDrops(true);
  centerPanel->addWidget(areaColumnas);

  workspace->addLayout(centerPanel, 2);

  // Panel Derecho: Acciones
  QVBoxLayout *rightPanel = new QVBoxLayout();
  btnEjecutar = new QPushButton("Ejecutar Consulta MDX");
  btnEjecutar->setStyleSheet(
      "background-color: #007ACC; color: white; padding: 10px;");
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  rightPanel->addWidget(btnEjecutar);

  btnReporte = new QPushButton("Descargar Reporte PDF");
  btnReporte->setStyleSheet(
      "background-color: #E65100; color: white; padding: 10px;");
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  rightPanel->addWidget(btnReporte);

  rightPanel->addStretch();
  workspace->addLayout(rightPanel, 1);

  mainLayout->addLayout(workspace);
}
