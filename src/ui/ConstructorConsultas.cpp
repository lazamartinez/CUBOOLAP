#include "ConstructorConsultas.h"
#include "Estilos.h"
#include "TablaDinamica.h"
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>

ConstructorConsultas::ConstructorConsultas(QWidget *parent) : QWidget(parent) {
  configurarUi();
}

void ConstructorConsultas::configurarUi() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(24, 24, 24, 24);

  // Header
  QLabel *header = new QLabel("Fase 5: Constructor de Consultas OLAP", this);
  header->setStyleSheet(Estilos::obtenerEstiloTituloSeccion("#6366f1"));
  mainLayout->addWidget(header);

  // Splitter principal
  QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

  // Panel superior: Constructor
  QWidget *builderWidget = new QWidget(this);
  QHBoxLayout *builderLayout = new QHBoxLayout(builderWidget);
  builderLayout->setSpacing(16);

  // Panel Izquierdo: Recursos disponibles
  QGroupBox *grpRecursos = new QGroupBox("Recursos Disponibles", this);
  QVBoxLayout *leftLayout = new QVBoxLayout(grpRecursos);

  QLabel *lblDims = new QLabel("Dimensiones", this);
  lblDims->setStyleSheet("font-weight: 600; color: #2563eb;");
  leftLayout->addWidget(lblDims);

  listaDimensiones = new QListWidget(this);
  listaDimensiones->addItems({"Tiempo", "Producto", "Cliente", "Geografia"});
  listaDimensiones->setDragEnabled(true);
  listaDimensiones->setStyleSheet(R"(
    QListWidget::item {
      padding: 8px;
      border-radius: 4px;
      margin: 2px 0;
    }
    QListWidget::item:hover {
      background: #dbeafe;
    }
    QListWidget::item:selected {
      background: #2563eb;
      color: white;
    }
  )");
  leftLayout->addWidget(listaDimensiones);

  QLabel *lblMeds = new QLabel("Medidas", this);
  lblMeds->setStyleSheet("font-weight: 600; color: #10b981;");
  leftLayout->addWidget(lblMeds);

  listaMedidas = new QListWidget(this);
  listaMedidas->addItems(
      {"Ventas Totales", "Costo", "Beneficio", "Margen %", "Cantidad"});
  listaMedidas->setDragEnabled(true);
  listaMedidas->setStyleSheet(listaDimensiones->styleSheet());
  leftLayout->addWidget(listaMedidas);

  builderLayout->addWidget(grpRecursos, 1);

  // Panel Central: Areas de construccion
  QWidget *buildArea = new QWidget(this);
  buildArea->setStyleSheet(Estilos::obtenerEstiloPanelGlass());
  QVBoxLayout *centerLayout = new QVBoxLayout(buildArea);

  // Filas
  QLabel *lblFilas = new QLabel("Filas (Dimension para agrupar)", this);
  lblFilas->setStyleSheet("font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblFilas);

  areaFilas = new QListWidget(this);
  areaFilas->setAcceptDrops(true);
  areaFilas->setMaximumHeight(80);
  areaFilas->setStyleSheet(R"(
    QListWidget {
      background: rgba(219, 234, 254, 0.5);
      border: 2px dashed #93c5fd;
      border-radius: 8px;
    }
  )");
  centerLayout->addWidget(areaFilas);

  // Columnas
  QLabel *lblCols = new QLabel("Columnas (Dimension secundaria)", this);
  lblCols->setStyleSheet("font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblCols);

  areaColumnas = new QListWidget(this);
  areaColumnas->setAcceptDrops(true);
  areaColumnas->setMaximumHeight(80);
  areaColumnas->setStyleSheet(areaFilas->styleSheet());
  centerLayout->addWidget(areaColumnas);

  // Medidas seleccionadas
  QLabel *lblMedsSel = new QLabel("Medidas a calcular", this);
  lblMedsSel->setStyleSheet("font-weight: 600; color: #374151;");
  centerLayout->addWidget(lblMedsSel);

  areaMedidas = new QListWidget(this);
  areaMedidas->setAcceptDrops(true);
  areaMedidas->setMaximumHeight(60);
  areaMedidas->setStyleSheet(R"(
    QListWidget {
      background: rgba(209, 250, 229, 0.5);
      border: 2px dashed #6ee7b7;
      border-radius: 8px;
    }
  )");
  centerLayout->addWidget(areaMedidas);

  builderLayout->addWidget(buildArea, 2);

  // Panel Derecho: Acciones y preview
  QGroupBox *grpAcciones = new QGroupBox("Acciones", this);
  QVBoxLayout *rightLayout = new QVBoxLayout(grpAcciones);

  btnEjecutar = new QPushButton("Ejecutar Consulta", this);
  btnEjecutar->setMinimumHeight(44);
  btnEjecutar->setCursor(Qt::PointingHandCursor);
  btnEjecutar->setStyleSheet(Estilos::obtenerEstiloBotonPrimario());
  connect(btnEjecutar, &QPushButton::clicked, this,
          &ConstructorConsultas::ejecutarConsulta);
  rightLayout->addWidget(btnEjecutar);

  btnLimpiar = new QPushButton("Limpiar Consulta", this);
  btnLimpiar->setStyleSheet(R"(
    QPushButton {
      background: #f3f4f6;
      border: 1px solid #d1d5db;
      border-radius: 8px;
      padding: 10px;
      color: #374151;
    }
    QPushButton:hover {
      background: #e5e7eb;
    }
  )");
  connect(btnLimpiar, &QPushButton::clicked, this, [this]() {
    areaFilas->clear();
    areaColumnas->clear();
    areaMedidas->clear();
  });
  rightLayout->addWidget(btnLimpiar);

  rightLayout->addSpacing(20);

  QLabel *lblExport = new QLabel("Exportar Resultados", this);
  lblExport->setStyleSheet(
      "font-weight: 600; color: #374151; margin-top: 10px;");
  rightLayout->addWidget(lblExport);

  btnReporte = new QPushButton("Descargar PDF", this);
  btnReporte->setStyleSheet(R"(
    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #ef4444, stop:1 #dc2626);
      border: none;
      border-radius: 8px;
      padding: 10px;
      color: white;
      font-weight: 600;
    }
    QPushButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #dc2626, stop:1 #b91c1c);
    }
  )");
  connect(btnReporte, &QPushButton::clicked, this,
          &ConstructorConsultas::generarReporte);
  rightLayout->addWidget(btnReporte);

  QPushButton *btnExcel = new QPushButton("Exportar Excel", this);
  btnExcel->setStyleSheet(R"(
    QPushButton {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #22c55e, stop:1 #16a34a);
      border: none;
      border-radius: 8px;
      padding: 10px;
      color: white;
      font-weight: 600;
    }
    QPushButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #16a34a, stop:1 #15803d);
    }
  )");
  rightLayout->addWidget(btnExcel);

  rightLayout->addStretch();
  builderLayout->addWidget(grpAcciones, 1);

  mainSplitter->addWidget(builderWidget);

  // Panel inferior: Resultados
  QGroupBox *grpResultados = new QGroupBox("Resultados de la Consulta", this);
  QVBoxLayout *resultsLayout = new QVBoxLayout(grpResultados);

  tablaResultados = new QTableWidget(this);
  tablaResultados->setColumnCount(5);
  tablaResultados->setHorizontalHeaderLabels(
      {"Dimension", "Periodo", "Ventas", "Costo", "Beneficio"});
  tablaResultados->horizontalHeader()->setStretchLastSection(true);
  tablaResultados->setAlternatingRowColors(true);
  tablaResultados->setStyleSheet(R"(
    QTableWidget {
      gridline-color: #e5e7eb;
      background: white;
      alternate-background-color: #f9fafb;
    }
    QHeaderView::section {
      background: #f1f5f9;
      padding: 10px;
      border: none;
      border-bottom: 2px solid #2563eb;
      font-weight: 600;
      color: #374151;
    }
  )");

  // Datos de ejemplo
  cargarDatosEjemplo();

  resultsLayout->addWidget(tablaResultados);

  // Info de resultados
  lblInfoResultados =
      new QLabel("Mostrando 10 de 48 registros | Tiempo: 45ms", this);
  lblInfoResultados->setStyleSheet("color: #6b7280; font-size: 12px;");
  resultsLayout->addWidget(lblInfoResultados);

  mainSplitter->addWidget(grpResultados);
  mainSplitter->setStretchFactor(0, 1);
  mainSplitter->setStretchFactor(1, 2);

  mainLayout->addWidget(mainSplitter);
}

void ConstructorConsultas::cargarDatosEjemplo() {
  QStringList productos = {"Electronica", "Ropa", "Alimentos", "Hogar"};
  QStringList periodos = {"Q1 2024", "Q2 2024", "Q3 2024"};

  tablaResultados->setRowCount(0);
  int row = 0;

  for (const QString &prod : productos) {
    for (const QString &per : periodos) {
      tablaResultados->insertRow(row);
      tablaResultados->setItem(row, 0, new QTableWidgetItem(prod));
      tablaResultados->setItem(row, 1, new QTableWidgetItem(per));

      double ventas = 5000 + (rand() % 10000);
      double costo = ventas * 0.6;
      double beneficio = ventas - costo;

      tablaResultados->setItem(
          row, 2, new QTableWidgetItem(QString("$%1").arg(ventas, 0, 'f', 2)));
      tablaResultados->setItem(
          row, 3, new QTableWidgetItem(QString("$%1").arg(costo, 0, 'f', 2)));
      tablaResultados->setItem(
          row, 4,
          new QTableWidgetItem(QString("$%1").arg(beneficio, 0, 'f', 2)));

      // Color para beneficio
      if (beneficio > 3000) {
        tablaResultados->item(row, 4)->setForeground(QColor("#10b981"));
      } else {
        tablaResultados->item(row, 4)->setForeground(QColor("#f59e0b"));
      }

      row++;
    }
  }
}

void ConstructorConsultas::ejecutarConsulta() {
  // Simular ejecucion
  lblInfoResultados->setText("Ejecutando consulta...");
  lblInfoResultados->setStyleSheet("color: #f59e0b; font-size: 12px;");

  // Refrescar datos
  cargarDatosEjemplo();

  lblInfoResultados->setText(QString("Mostrando %1 registros | Tiempo: %2ms")
                                 .arg(tablaResultados->rowCount())
                                 .arg(rand() % 100 + 20));
  lblInfoResultados->setStyleSheet(
      "color: #10b981; font-size: 12px; font-weight: 500;");
}

void ConstructorConsultas::generarReporte() {
  QMessageBox::information(
      this, "Exportar PDF",
      "El reporte PDF se generaria aqui.\n\n"
      "En una implementacion completa, se usaria QPdfWriter o wkhtmltopdf.");
}
