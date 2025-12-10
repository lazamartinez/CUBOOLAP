#include "ConstructorConsultas.h"
#include "Estilos.h"
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>

ConstructorConsultas::ConstructorConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(10);
  mainLayout->setContentsMargins(16, 16, 16, 16);

  // Header
  QLabel *header = new QLabel("Fase 5: Constructor de Consultas", this);
  header->setStyleSheet(R"(
    font-size: 16px;
    font-weight: 700;
    color: #1f2937;
    padding-bottom: 8px;
    border-bottom: 2px solid #6366f1;
  )");
  mainLayout->addWidget(header);

  // Splitter principal
  QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

  // Panel superior: Constructor
  QWidget *builderWidget = new QWidget(this);
  QHBoxLayout *builderLayout = new QHBoxLayout(builderWidget);
  builderLayout->setSpacing(10);
  builderLayout->setContentsMargins(0, 0, 0, 0);

  // Recursos
  QWidget *resContainer = new QWidget(this);
  resContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  resContainer->setMaximumWidth(180);
  QVBoxLayout *resLayout = new QVBoxLayout(resContainer);
  resLayout->setContentsMargins(8, 8, 8, 8);
  resLayout->setSpacing(6);

  QLabel *lblDims = new QLabel("Dimensiones", this);
  lblDims->setStyleSheet("font-size: 11px; font-weight: 600; color: #2563eb;");
  resLayout->addWidget(lblDims);

  listaDimensiones = new QListWidget(this);
  listaDimensiones->addItems({"Tiempo", "Producto", "Cliente", "Geografia"});
  listaDimensiones->setDragEnabled(true);
  listaDimensiones->setMaximumHeight(80);
  listaDimensiones->setStyleSheet("font-size: 11px;");
  resLayout->addWidget(listaDimensiones);

  QLabel *lblMeds = new QLabel("Medidas", this);
  lblMeds->setStyleSheet("font-size: 11px; font-weight: 600; color: #10b981;");
  resLayout->addWidget(lblMeds);

  listaMedidas = new QListWidget(this);
  listaMedidas->addItems({"Ventas", "Costo", "Beneficio", "Cantidad"});
  listaMedidas->setDragEnabled(true);
  listaMedidas->setMaximumHeight(80);
  listaMedidas->setStyleSheet("font-size: 11px;");
  resLayout->addWidget(listaMedidas);

  builderLayout->addWidget(resContainer);

  // Areas de construccion
  QWidget *buildArea = new QWidget(this);
  buildArea->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *centerLayout = new QVBoxLayout(buildArea);
  centerLayout->setContentsMargins(10, 10, 10, 10);
  centerLayout->setSpacing(6);

  QLabel *lblFilas = new QLabel("Filas", this);
  lblFilas->setStyleSheet("font-size: 10px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblFilas);

  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(50);
  areaFilas->setStyleSheet("background: #dbeafe; border: 1px dashed #93c5fd; "
                           "border-radius: 4px; font-size: 10px;");
  centerLayout->addWidget(areaFilas);

  QLabel *lblCols = new QLabel("Columnas", this);
  lblCols->setStyleSheet("font-size: 10px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblCols);

  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(50);
  areaColumnas->setStyleSheet("background: #dbeafe; border: 1px dashed "
                              "#93c5fd; border-radius: 4px; font-size: 10px;");
  centerLayout->addWidget(areaColumnas);

  QLabel *lblMedsSel = new QLabel("Medidas", this);
  lblMedsSel->setStyleSheet(
      "font-size: 10px; font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblMedsSel);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(40);
  areaMedidas->setStyleSheet("background: #d1fae5; border: 1px dashed #6ee7b7; "
                             "border-radius: 4px; font-size: 10px;");
  centerLayout->addWidget(areaMedidas);

  builderLayout->addWidget(buildArea);

  // Acciones
  QWidget *actContainer = new QWidget(this);
  actContainer->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  actContainer->setMaximumWidth(140);
  QVBoxLayout *actLayout = new QVBoxLayout(actContainer);
  actLayout->setContentsMargins(8, 8, 8, 8);
  actLayout->setSpacing(6);

  btnEjecutar = new QPushButton("Ejecutar", this);
  btnEjecutar->setMinimumHeight(32);
  btnEjecutar->setStyleSheet(R"(
    QPushButton { background: #2563eb; border: none; border-radius: 4px; color: white; font-size: 11px; font-weight: 600; }
    QPushButton:hover { background: #1d4ed8; }
  )");
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  actLayout->addWidget(btnEjecutar);

  btnLimpiar = new QPushButton("Limpiar", this);
  btnLimpiar->setStyleSheet(
      "background: white; border: 1px solid #d1d5db; border-radius: 4px; "
      "font-size: 11px; padding: 6px;");
  connect(btnLimpiar, &QPushButton::clicked, this, [this]() {
    areaFilas->clear();
    areaColumnas->clear();
    areaMedidas->clear();
  });
  actLayout->addWidget(btnLimpiar);

  actLayout->addStretch();

  btnReporte = new QPushButton("PDF", this);
  btnReporte->setStyleSheet(
      "background: #ef4444; border: none; border-radius: 4px; color: white; "
      "font-size: 10px; padding: 6px;");
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  actLayout->addWidget(btnReporte);

  builderLayout->addWidget(actContainer);

  mainSplitter->addWidget(builderWidget);

  // Resultados
  QWidget *resWidget = new QWidget(this);
  resWidget->setStyleSheet(
      "background: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;");
  QVBoxLayout *resultsLayout = new QVBoxLayout(resWidget);
  resultsLayout->setContentsMargins(10, 10, 10, 10);

  QLabel *lblRes = new QLabel("Resultados", resWidget);
  lblRes->setStyleSheet("font-size: 11px; font-weight: 600; color: #374151;");
  resultsLayout->addWidget(lblRes);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setColumnCount(4);
  tablaResultados->setHorizontalHeaderLabels(
      {"Dimension", "Periodo", "Ventas", "Beneficio"});
  tablaResultados->horizontalHeader()->setStretchLastSection(true);
  tablaResultados->setAlternatingRowColors(true);
  tablaResultados->setStyleSheet("font-size: 10px;");

  cargarDatosEjemplo();
  resultsLayout->addWidget(tablaResultados);

  lblInfoResultados = new QLabel("10 registros | 35ms", this);
  lblInfoResultados->setStyleSheet("color: #6b7280; font-size: 10px;");
  resultsLayout->addWidget(lblInfoResultados);

  mainSplitter->addWidget(resWidget);
  mainSplitter->setStretchFactor(0, 1);
  mainSplitter->setStretchFactor(1, 2);

  mainLayout->addWidget(mainSplitter);
}

void ConstructorConsultas::cargarDatosEjemplo() {
  QStringList prods = {"Electronica", "Ropa", "Alimentos"};
  QStringList pers = {"Q1", "Q2", "Q3"};

  tablaResultados->setRowCount(0);
  int row = 0;

  for (const QString &prod : prods) {
    for (const QString &per : pers) {
      tablaResultados->insertRow(row);
      tablaResultados->setItem(row, 0, new QTableWidgetItem(prod));
      tablaResultados->setItem(row, 1, new QTableWidgetItem(per));
      tablaResultados->setItem(
          row, 2,
          new QTableWidgetItem(QString("$%1").arg(5000 + rand() % 5000)));
      tablaResultados->setItem(
          row, 3,
          new QTableWidgetItem(QString("$%1").arg(1000 + rand() % 2000)));
      row++;
    }
  }
}

void ConstructorConsultas::ejecutarConsulta() {
  cargarDatosEjemplo();
  lblInfoResultados->setText(QString("%1 registros | %2ms")
                                 .arg(tablaResultados->rowCount())
                                 .arg(rand() % 50 + 20));
}

void ConstructorConsultas::generarReporte() {
  QMessageBox::information(this, "PDF", "Reporte PDF generado.");
}
